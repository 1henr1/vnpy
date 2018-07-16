# encoding: UTF-8

'''
行情曲线绘制模块相关的GUI控制组件
'''

import json
from vnpy.trader.uiQt import QtWidgets, QtCore
from vnpy.trader.vtEvent import *
from vnpy.trader.vtObject import *
from vnpy.trader.vtFunction import todayDate, getJsonPath
import numpy as np
import pyqtgraph as pg
from pymongo import MongoClient
from pymongo.errors import *
from datetime import datetime, timedelta
from PyQt4 import QtGui

from vnpy.event import *

#----------------------------------------------------------------------
def print_dict(d):
    """按照键值打印一个字典"""
    for key,value in d.items():
        print key + ':' + str(value),
    print
#----------------------------------------------------------------------

class CandlestickItem(pg.GraphicsObject):
    def __init__(self, data):
        pg.GraphicsObject.__init__(self)
        self.data = data  ## data must have fields: time, open, close, min, max
        self.generatePicture()

    def generatePicture(self):
        ## pre-computing a QPicture object allows paint() to run much more quickly,
        ## rather than re-drawing the shapes every time.
        self.picture = QtGui.QPicture()
        p = QtGui.QPainter(self.picture)
        p.setPen(pg.mkPen(color='w', width=0.4))  # 0.4 means w*2
        # w = (self.data[1][0] - self.data[0][0]) / 3.
        w = 0.2
        for (t, open, close, min, max) in self.data:
            if min != max:
                p.drawLine(QtCore.QPointF(t, min), QtCore.QPointF(t, max))
            if open > close:
                p.setBrush(pg.mkBrush('g'))
            else:
                p.setBrush(pg.mkBrush('r'))
            p.drawRect(QtCore.QRectF(t-w, open, w*2, close-open))
        p.end()

    def paint(self, p, *args):
        p.drawPicture(0, 0, self.picture)

    def boundingRect(self):
        ## boundingRect _must_ indicate the entire area that will be drawn on
        ## or else we will get artifacts and possibly crashing.
        ## (in this case, QPicture does all the work of computing the bouning rect for us)
        return QtCore.QRectF(self.picture.boundingRect())


########################################################################
# 时间序列，横坐标支持
########################################################################
class MyStringAxis(pg.AxisItem):
    """时间序列横坐标支持"""

    # 初始化
    #----------------------------------------------------------------------
    def __init__(self, xdict, *args, **kwargs):
        pg.AxisItem.__init__(self, *args, **kwargs)
        self.minVal = 0
        self.maxVal = 0
        self.xdict  = xdict
        self.x_values = np.asarray(xdict.keys())
        self.x_strings = xdict.values()
        self.setPen(color=(255, 255, 255, 255), width=0.8)
        self.setStyle(autoExpandTextSpace=True)

    # 更新坐标映射表
    #----------------------------------------------------------------------
    def update_xdict(self, xdict):
        self.xdict.update(xdict)
        self.x_values  = np.asarray(self.xdict.keys())
        self.x_strings = self.xdict.values()

    # 将原始横坐标转换为时间字符串,第一个坐标包含日期
    #----------------------------------------------------------------------
    def tickStrings(self, values, scale, spacing):
        strings = []
        for v in values:
            vs = v * scale
            if vs in self.x_values:
                vstr = self.x_strings[int(vs)]
                vstr = vstr.strftime('%H:%M')
            else:
                vstr = ""
            strings.append(vstr)
        return strings

########################################################################
class CurveWidget(QtWidgets.QWidget):
    """用于显示价格走势图"""
    signal = QtCore.pyqtSignal(type(Event()))

    fastMA = 0
    midMA = 0
    slowMA = 0
    tickFastAlpha = 0.0333    # 快速均线的参数,30
    tickMidAlpha = 0.0167     # 中速均线的参数,60
    tickSlowAlpha = 0.0083    # 慢速均线的参数,120

    # ticktime = None  # tick数据时间

    # K线图EMA均线的参数、变量
    EMAFastAlpha = 0.0167    # 快速EMA的参数,60
    EMASlowAlpha = 0.0083  # 慢速EMA的参数,120
    fastEMA = 0        # 快速EMA的数值
    slowEMA = 0        # 慢速EMA的数值

    #----------------------------------------------------------------------
    def __init__(self,  mainEngine, eventEngine, parent=None):
        """Constructor"""
        super(CurveWidget, self).__init__(parent)

        self.__eventEngine = eventEngine
        self.__mainEngine = mainEngine
        # MongoDB数据库相关
        self.__mongoConnected = False
        self.__mongoConnection = None
        self.__mongoTickDB = None

        # 对象字典
        self.tickSymbolSet = set()
        self.barSymbolSet = set()

        # K线合成器字典
        self.bgDict = {}
        self.listBar = []

        # 配置文件
        self.settingFileName = 'CurveDrawer_setting.json'
        self.settingFilePath = getJsonPath(self.settingFileName, __file__)

        # 调用函数
        self.__connectMongo()
        self.initUi()

    #----------------------------------------------------------------------
    def initUi(self):
        """初始化界面"""
        self.setWindowTitle(u'Price')

        # 创建按钮
        buttonInit = QtWidgets.QPushButton(u'初始化')
        buttonInit.clicked.connect(self.init)

        #self.vbl_1 = QtGui.QVBoxLayout()
        #self.initplotTick()  # plotTick初始化

        self.vbl_2 = QtGui.QVBoxLayout()
        self.initplotKline()  # plotKline初始化

        # 整体布局
        self.hbl = QtGui.QHBoxLayout()
        self.hbl.addWidget(buttonInit)
        #self.hbl.addLayout(self.vbl_1)
        self.hbl.addLayout(self.vbl_2)
        self.setLayout(self.hbl)

    def init(self):
        """初始化"""
        self.loadSetting()
        ## 初始化绘图数据
        self.initData()
        ## 读取历史数据
        self.initHistoricalData()
        ## 注册事件， 开始实时绘制
        self.registerEvent()

    def initData(self):
        self.ptr = 0
        self.barNum = 0

        self.bar = VtBarData()

        # tick图的相关参数、变量
        self.listlastPrice = np.empty(1000)
        self.listfastMA = np.empty(1000)
        self.listmidMA = np.empty(1000)
        self.listslowMA = np.empty(1000)
        # K线缓存对象
        self.barOpen = 0
        self.barHigh = 0
        self.barLow = 0
        self.barClose = 0
        self.barTime = None
        self.barOpenInterest = 0
        # 保存K线数据的列表对象
        self.listBar = []
        self.listClose = []
        self.listHigh = []
        self.listLow = []
        self.listOpen = []
        # listOpenInterest = []

        # 是否完成了历史数据的读取
        self.initCompleted = False

    #----------------------------------------------------------------------
    def loadSetting(self):
        """加载配置"""
        with open(self.settingFilePath) as f:
            drSetting = json.load(f)

            # 如果working设为False则不启动行情绘制功能
            working = drSetting['working']
            if not working:
                return

            # Tick记录配置
            if 'tick' in drSetting:
                l = drSetting['tick']

                for setting in l:
                    symbol = setting[0]
                    gateway = setting[1]
                    vtSymbol = ".".join([symbol, gateway])
                    self.tickSymbolSet.add(vtSymbol)

            # Bar记录配置
            if 'bar' in drSetting:
                l = drSetting['bar']

                for setting in l:
                    symbol = setting[0]
                    gateway = setting[1]
                    vtSymbol = ".".join([symbol, gateway])
                    self.barSymbolSet.add(vtSymbol)

    #----------------------------------------------------------------------
    def initplotTick(self):
        """"""
        self.pw1 = pg.PlotWidget(name='Plot1')
        self.vbl_1.addWidget(self.pw1)
        self.pw1.setRange(xRange=[-360, 0])
        self.pw1.setLimits(xMax=5)
        self.pw1.setDownsampling(mode='peak')
        self.pw1.setClipToView(True)
        self.pw1.showGrid(x=True, y=True)

        self.curve1 = self.pw1.plot()
        self.curve2 = self.pw1.plot()
        self.curve3 = self.pw1.plot()
        self.curve4 = self.pw1.plot()

    #----------------------------------------------------------------------
    def initplotKline(self):
        """Kline"""
        xdict = {}
        self.axisTime = MyStringAxis(xdict, orientation='bottom')

        self.pw2 = pg.PlotWidget(name='Plot2', axisItems={'bottom': self.axisTime})  # K线图
        self.vbl_2.addWidget(self.pw2)
        self.pw2.setDownsampling(mode='peak')
        self.pw2.setClipToView(True)
        self.pw2.showGrid(x=True, y=True)

        self.candle = CandlestickItem(self.listBar)
        self.pw2.addItem(self.candle)

    #----------------------------------------------------------------------
    def initHistoricalData(self, startDate=None):
        """初始历史数据"""

        ## 对bar下载历史数据
        for vtSymbol in self.barSymbolSet:
            if startDate:
                cx = self.loadTick(self.symbol, startDate)
            else:
                td = timedelta(days=1)     # 读取1天的历史数据
                today = datetime.today().replace(hour=0, minute=0, second=0, microsecond=0)
                cx = self.loadTick(vtSymbol, today-td)

            if cx:
                for data in cx:
                    #bar = VtBarData()

                    self.bar.vtSymbol = data["vtSymbol"]  # vt系统代码
                    self.bar.symbol = data["symbol"]          # 代码
                    self.bar.exchange = data["exchange"]        # 交易所

                    self.bar.open = data["open"]             # OHLC
                    self.bar.high = data["high"]
                    self.bar.low = data["low"]
                    self.bar.close = data["close"]

                    self.bar.date = data["date"]            # bar开始的时间，日期
                    self.bar.time = data["time"]            # 时间
                    self.bar.datetime = data["datetime"]                # python的datetime时间对象

                    self.bar.volume = data["volume"]             # 成交量
                    self.bar.openInterest = data["openInterest"]       # 持仓量

                    self.onBar(self.bar)
                    #self.onBar(bar.open, bar.close, bar.low, bar.high, bar.openInterest)
        """
        if cx:
            for data in cx:
                tick = VtTickData()

                # 代码相关
                tick.symbol = data['symbol'] # 合约代码
                tick.exchange = data['exchange'] # 交易所代码
                tick.vtSymbol = data['vtSymbol'] # 合约在vt系统中的唯一代码，通常是 合约代码.交易所代码

                # 成交数据
                tick.lastPrice = data['lastPrice'] # 最新成交价
                tick.lastVolume = data['lastVolume'] # 最新成交量
                tick.volume =  data['volume']# 今天总成交量
                tick.openInterest = data['openInterest'] # 持仓量
                tick.time = data['time'] # 时间 11:20:56.5
                tick.date = data['date'] # 日期 20151009
                tick.datetime = data['datetime']                    # python的datetime时间对象

                # 常规行情
                tick.openPrice =  data['openPrice']   # 今日开盘价
                tick.highPrice =  data['highPrice']   # 今日最高价
                tick.lowPrice =  data['lowPrice']  # 今日最低价
                tick.preClosePrice = data['preClosePrice']   # 昨收盘价
                tick.upperLimit = data['upperLimit']  # 涨停价
                tick.lowerLimit = data['lowerLimit']  # 跌停价

                # 五档行情
                tick.bidPrice1 = data['bidPrice1']
                tick.bidPrice2 = data['bidPrice2']
                tick.bidPrice3 = data['bidPrice3']
                tick.bidPrice4 = data['bidPrice4']
                tick.bidPrice5 = data['bidPrice5']

                tick.askPrice1 = data['askPrice1']
                tick.askPrice2 = data['askPrice2']
                tick.askPrice3 = data['askPrice3']
                tick.askPrice4 = data['askPrice4']
                tick.askPrice5 = data['askPrice5']

                tick.bidVolume1 = data['bidVolume1']
                tick.bidVolume2 = data['bidVolume2']
                tick.bidVolume3 = data['bidVolume3']
                tick.bidVolume4 = data['bidVolume4']
                tick.bidVolume5 = data['bidVolume5']

                tick.askVolume1 = data['askVolume1']
                tick.askVolume2 = data['askVolume2']
                tick.askVolume3 = data['askVolume3']
                tick.askVolume4 = data['askVolume4']
                tick.askVolume5 = data['askVolume5']

                self.onTick(tick)
        """

        self.initCompleted = True    # 读取历史数据完成

    #----------------------------------------------------------------------
    def plotTick(self):
        """画tick图"""
        self.initCompleted = True
        if self.initCompleted:
            self.curve1.setData(self.listlastPrice[:self.ptr])
            #self.curve2.setData(self.listfastMA[:self.ptr], pen=(255, 0, 0), name="Red curve")
            #self.curve3.setData(self.listmidMA[:self.ptr], pen=(0, 255, 0), name="Green curve")
            #self.curve4.setData(self.listslowMA[:self.ptr], pen=(0, 0, 255), name="Blue curve")
            self.curve1.setPos(-self.ptr, 0)
            #self.curve2.setPos(-self.ptr, 0)
            #self.curve3.setPos(-self.ptr, 0)
            #self.curve4.setPos(-self.ptr, 0)

    #----------------------------------------------------------------------
    def plotKline(self):
        """K线图"""
        self.initCompleted = True
        if self.initCompleted:
            # 均线
            #self.curve5.setData(self.listfastEMA, pen=(255, 0, 0), name="Red curve")
            #self.curve6.setData(self.listslowEMA, pen=(0, 255, 0), name="Green curve")

            # 画K线
            self.pw2.removeItem(self.candle)
            self.candle = CandlestickItem(self.listBar)
            self.pw2.addItem(self.candle)
            # self.pw2.setYRange(min(self.listLow), max(self.listHigh))

    #----------------------------------------------------------------------
    def updateMarketData(self, event):
        """更新行情"""
        data = event.dict_['data']
        vtSymbol = data.vtSymbol
        if vtSymbol not in self.tickSymbolSet:
            return

        tick = VtTickData()
        # 代码相关
        tick.symbol = data.symbol # 合约代码
        tick.exchange = data.exchange # 交易所代码
        tick.vtSymbol = data.vtSymbol # 合约在vt系统中的唯一代码，通常是 合约代码.交易所代码

        # 成交数据
        tick.lastPrice = data.lastPrice # 最新成交价
        tick.lastVolume = data.lastVolume # 最新成交量
        tick.volume =  data.volume# 今天总成交量
        tick.openInterest = data.openInterest # 持仓量
        tick.time = data.time # 时间 11:20:56.5
        tick.date = data.date # 日期 20151009
        tick.datetime = data.datetime                    # python的datetime时间对象

        # 常规行情
        tick.openPrice =  data.openPrice   # 今日开盘价
        tick.highPrice =  data.highPrice   # 今日最高价
        tick.lowPrice =  data.lowPrice  # 今日最低价
        tick.preClosePrice = data.preClosePrice   # 昨收盘价
        tick.upperLimit = data.upperLimit  # 涨停价
        tick.lowerLimit = data.lowerLimit  # 跌停价

        # 五档行情
        tick.bidPrice1 = data.bidPrice1
        tick.bidPrice2 = data.bidPrice2
        tick.bidPrice3 = data.bidPrice3
        tick.bidPrice4 = data.bidPrice4
        tick.bidPrice5 = data.bidPrice5

        tick.askPrice1 = data.askPrice1
        tick.askPrice2 = data.askPrice2
        tick.askPrice3 = data.askPrice3
        tick.askPrice4 = data.askPrice4
        tick.askPrice5 = data.askPrice5

        tick.bidVolume1 = data.bidVolume1
        tick.bidVolume2 = data.bidVolume2
        tick.bidVolume3 = data.bidVolume3
        tick.bidVolume4 = data.bidVolume4
        tick.bidVolume5 = data.bidVolume5

        tick.askVolume1 = data.askVolume1
        tick.askVolume2 = data.askVolume2
        tick.askVolume3 = data.askVolume3
        tick.askVolume4 = data.askVolume4
        tick.askVolume5 = data.askVolume5

        self.onTick(tick)  # tick数据更新

    #----------------------------------------------------------------------
    def onTick(self, tick):
        """tick数据更新"""

        # 首先生成datetime.time格式的时间（便于比较）,从字符串时间转化为time格式的时间
        #self.ticktime = tick.datetime   # python的datetime时间对象

        # 计算tick图的相关参数
        if self.ptr == 0:
            self.fastMA = tick.lastPrice
            self.midMA = tick.lastPrice
            self.slowMA = tick.lastPrice
        else:
            self.fastMA = (1-self.tickFastAlpha) * self.fastMA + self.tickFastAlpha * tick.lastPrice
            self.midMA = (1-self.tickMidAlpha) * self.midMA + self.tickMidAlpha * tick.lastPrice
            self.slowMA = (1-self.tickSlowAlpha) * self.slowMA + self.tickSlowAlpha * tick.lastPrice

        self.listlastPrice[self.ptr] = tick.lastPrice
        self.listfastMA[self.ptr] = self.fastMA
        self.listmidMA[self.ptr] = self.midMA
        self.listslowMA[self.ptr] = self.slowMA

        self.ptr += 1
        if self.ptr >= self.listlastPrice.shape[0]:
            tmp = self.listlastPrice
            self.listlastPrice = np.empty(self.listlastPrice.shape[0] * 2)
            self.listlastPrice[:tmp.shape[0]] = tmp
            tmp = self.listfastMA
            self.listfastMA = np.empty(self.listfastMA.shape[0] * 2)
            self.listfastMA[:tmp.shape[0]] = tmp
            tmp = self.listmidMA
            self.listmidMA = np.empty(self.listmidMA.shape[0] * 2)
            self.listmidMA[:tmp.shape[0]] = tmp
            tmp = self.listslowMA
            self.listslowMA = np.empty(self.listslowMA.shape[0] * 2)
            self.listslowMA[:tmp.shape[0]] = tmp

        # 调用画图函数
        # self.plotTick()      # tick图

        # K线数据
        # 假设是收到的第一个TICK
        if self.bar.datetime == None:
            # 初始化新的K线数据
            self.bar.vtSymbol = tick.vtSymbol  # vt系统代码
            # bar.symbol = data["symbol"]          # 代码
            # bar.exchange = data["exchange"]        # 交易所
            self.bar.open = tick.lastPrice             # OHLC
            self.bar.high = tick.lastPrice
            self.bar.low = tick.lastPrice
            self.bar.close = tick.lastPrice
            self.bar.date = tick.datetime.strftime('%Y-%m-%d')            # bar开始的时间，日期
            self.bar.time = tick.datetime.strftime('%H:%M:%S')            # 时间
            self.bar.datetime = tick.datetime                # python的datetime时间对象
            # bar.volume = data["volume"]             # 成交量
            # bar.openInterest = data["openInterest"]       # 持仓量

            # self.barOpen = tick.lastPrice
            # self.barHigh = tick.lastPrice
            # self.barLow = tick.lastPrice
            # self.barClose = tick.lastPrice
            # self.barTime = self.ticktime
            # self.barOpenInterest = tick.openInterest
            # self.onBar(self.barOpen, self.barClose, self.barLow, self.barHigh, self.barOpenInterest)
            self.lastClose = tick.lastPrice
            self.onBar(self.bar)
        else:
            # 如果是当前一分钟内的数据
            if tick.datetime.minute == self.bar.datetime.minute:
                # 汇总TICK生成K线
                self.bar.high = max(self.bar.high, tick.lastPrice)
                self.bar.low = min(self.bar.low, tick.lastPrice)
                self.bar.close = tick.lastPrice
                self.bar.datetime = tick.datetime
            # 如果是新一分钟的数据
            else:
                # 先保存K线收盘价
                self.lastClose = self.bar.close
                self.onBar(self.bar)
                # 初始化新的K线数据
                self.bar.open = self.lastClose
                self.bar.high = max(self.lastClose, tick.lastPrice)
                self.bar.low = min(self.lastClose, tick.lastPrice)
                self.bar.close = tick.lastPrice
                self.bar.datetime = tick.datetime
                #self.barOpenInterest = tick.openInterest

    #----------------------------------------------------------------------
    def onBar(self, bar):
        self.listBar.append((self.barNum, bar.open, bar.close, bar.low, bar.high))
        self.listOpen.append(bar.open)
        self.listClose.append(bar.close)
        self.listHigh.append(bar.high)
        self.listLow.append(bar.low)
        #self.listOpenInterest.append(oi)

        self.axisTime.update_xdict({self.barNum : bar.datetime})
        self.barNum += 1

        # 调用画图函数
        self.plotKline()     # K线图
        #self.plotTendency()  # K线副图，持仓量

    #----------------------------------------------------------------------
    def __connectMongo(self):
        """连接MongoDB数据库"""
        try:
            self.__mongoConnection = MongoClient()
            self.__mongoConnected = True
            self.__mongoTickDB = self.__mongoConnection['VnTrader_1Min_Db']
        except ConnectionFailure:
            pass

    #----------------------------------------------------------------------
    def __recordTick(self, data):
        """将Tick数据插入到MongoDB中"""
        if self.__mongoConnected:
            symbol = data['InstrumentID']
            data['date'] = self.today
            self.__mongoTickDB[symbol].insert(data)

    #----------------------------------------------------------------------
    def loadTick(self, symbol, startDate, endDate=None):
        """从MongoDB中读取Tick数据"""
        if self.__mongoConnected:
            collection = self.__mongoTickDB[symbol]

            # 如果输入了读取TICK的最后日期
            if endDate:
                cx = collection.find({'date': {'$gte': startDate.strftime('%Y%m%d'),
                                               '$lte': endDate.strftime('%Y%m%d')}})
            else:
                cx = collection.find({'date': {'$gte': startDate.strftime('%Y%m%d')}})
            return cx
        else:
            return None

    #----------------------------------------------------------------------
    def registerEvent(self):
        """注册事件监听"""
        self.signal.connect(self.updateMarketData)
        #self.__eventEngine.register(EVENT_TICK, self.signal.emit)
        self.__eventEngine.register(EVENT_SPREAD_TICK, self.signal.emit)




