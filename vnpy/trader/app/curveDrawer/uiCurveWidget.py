# encoding: UTF-8

'''
行情曲线绘制模块相关的GUI控制组件
'''

from vnpy.trader.uiQt import QtWidgets, QtCore
from vnpy.trader.vtEvent import *
from vnpy.trader.vtObject import *
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
class CurveWidget(QtWidgets.QWidget):
    """用于显示价格走势图"""
    signal = QtCore.pyqtSignal(type(Event()))

    # tick图的相关参数、变量
    listlastPrice = np.empty(1000)

    fastMA = 0
    midMA = 0
    slowMA = 0
    listfastMA = np.empty(1000)
    listmidMA = np.empty(1000)
    listslowMA = np.empty(1000)
    tickFastAlpha = 0.0333    # 快速均线的参数,30
    tickMidAlpha = 0.0167     # 中速均线的参数,60
    tickSlowAlpha = 0.0083    # 慢速均线的参数,120

    ptr = 0
    ticktime = None  # tick数据时间

    # K线图EMA均线的参数、变量
    EMAFastAlpha = 0.0167    # 快速EMA的参数,60
    EMASlowAlpha = 0.0083  # 慢速EMA的参数,120
    fastEMA = 0        # 快速EMA的数值
    slowEMA = 0        # 慢速EMA的数值
    #listfastEMA = []
    #listslowEMA = []

    # K线缓存对象
    barOpen = 0
    barHigh = 0
    barLow = 0
    barClose = 0
    barTime = None
    barOpenInterest = 0
    num = 0

    # 保存K线数据的列表对象
    listBar = []
    listClose = []
    listHigh = []
    listLow = []
    listOpen = []
    # listOpenInterest = []

    # 是否完成了历史数据的读取
    initCompleted = False
    # 初始化时读取的历史数据的起始日期(可以选择外部设置)
    startDate = "2018-07-05"
    symbol = 'SI 1809.COMEX'


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

        # 调用函数
        self.__connectMongo()
        self.initUi()
        self.registerEvent()

    #----------------------------------------------------------------------
    def initUi(self):
        """初始化界面"""
        self.setWindowTitle(u'Price')

        self.vbl_1 = QtGui.QVBoxLayout()
        self.initplotTick()  # plotTick初始化

        self.vbl_2 = QtGui.QVBoxLayout()
        self.initplotKline()  # plotKline初始化
        #self.initplotTendency()  # plot分时图的初始化

        # 整体布局
        self.hbl = QtGui.QHBoxLayout()
        self.hbl.addLayout(self.vbl_1)
        self.hbl.addLayout(self.vbl_2)
        self.setLayout(self.hbl)

        self.initHistoricalData()  # 下载历史数据

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
        self.pw2 = pg.PlotWidget(name='Plot2')  # K线图
        self.vbl_2.addWidget(self.pw2)
        self.pw2.setDownsampling(mode='peak')
        self.pw2.setClipToView(True)
        self.pw2.showGrid(x=True, y=True)


        #self.curve5 = self.pw2.plot()
        #self.curve6 = self.pw2.plot()

        self.candle = CandlestickItem(self.listBar)
        self.pw2.addItem(self.candle)
        ## Draw an arrowhead next to the text box
        # self.arrow = pg.ArrowItem()
        # self.pw2.addItem(self.arrow)

    #----------------------------------------------------------------------
    '''
    def initplotTendency(self):
        """"""
        self.pw3 = pg.PlotWidget(name='Plot3')
        self.vbl_2.addWidget(self.pw3)
        self.pw3.setDownsampling(mode='peak')
        self.pw3.setClipToView(True)
        self.pw3.setMaximumHeight(200)
        self.pw3.setXLink('Plot2')   # X linked with Plot2

        self.curve7 = self.pw3.plot()
    '''

    #----------------------------------------------------------------------
    def initHistoricalData(self,startDate=None):
        """初始历史数据"""

        startDate = "2018-07-05"
        td = timedelta(days=1)     # 读取3天的历史TICK数据

        if startDate:
            cx = self.loadTick(self.symbol, startDate)
        else:
            today = datetime.today().replace(hour=0, minute=0, second=0, microsecond=0)
            cx = self.loadTick(self.symbol, today-td)

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

        self.initCompleted = True    # 读取历史数据完成

    #----------------------------------------------------------------------
    def plotTick(self):
        """画tick图"""
        self.initCompleted = True
        if self.initCompleted:
            self.curve1.setData(self.listlastPrice[:self.ptr])
            self.curve2.setData(self.listfastMA[:self.ptr], pen=(255, 0, 0), name="Red curve")
            self.curve3.setData(self.listmidMA[:self.ptr], pen=(0, 255, 0), name="Green curve")
            self.curve4.setData(self.listslowMA[:self.ptr], pen=(0, 0, 255), name="Blue curve")
            self.curve1.setPos(-self.ptr, 0)
            self.curve2.setPos(-self.ptr, 0)
            self.curve3.setPos(-self.ptr, 0)
            self.curve4.setPos(-self.ptr, 0)

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
            self.pw2.setYRange(min(self.listLow), max(self.listHigh))

    #----------------------------------------------------------------------
    def updateMarketData(self, event):
        """更新行情"""
        data = event.dict_['data']
        if data.vtSymbol != self.symbol:
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

        # # 将数据插入MongoDB数据库，实盘建议另开程序记录TICK数据
        # self.__recordTick(data)

    #----------------------------------------------------------------------
    def onTick(self, tick):
        """tick数据更新"""

        # 首先生成datetime.time格式的时间（便于比较）,从字符串时间转化为time格式的时间
        self.ticktime = tick.datetime   # python的datetime时间对象

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
        self.plotTick()      # tick图

        # K线数据
        # 假设是收到的第一个TICK
        if self.barOpen == 0:
            # 初始化新的K线数据
            self.barOpen = tick.lastPrice
            self.barHigh = tick.lastPrice
            self.barLow = tick.lastPrice
            self.barClose = tick.lastPrice
            self.barTime = self.ticktime
            self.barOpenInterest = tick.openInterest
            self.onBar(self.num, self.barOpen, self.barClose, self.barLow, self.barHigh, self.barOpenInterest)
        else:
            # 如果是当前一分钟内的数据
            if self.ticktime.minute == self.barTime.minute:
                #if self.ticktime.second >= 30 and self.barTime.second < 30: # 判断30秒周期K线
                #    # 先保存K线收盘价
                #    self.num += 1
                #    self.onBar(self.num, self.barOpen, self.barClose, self.barLow, self.barHigh, self.barOpenInterest)
                #    # 初始化新的K线数据
                #    self.barOpen = tick.lastPrice
                #    self.barHigh = tick.lastPrice
                #    self.barLow = tick.lastPrice
                #    self.barClose = tick.lastPrice
                #    self.barTime = self.ticktime
                #    self.barOpenInterest = tick.openInterest
                # 汇总TICK生成K线
                self.barHigh = max(self.barHigh, tick.lastPrice)
                self.barLow = min(self.barLow, tick.lastPrice)
                self.barClose = tick.lastPrice
                self.barTime = self.ticktime
                self.listBar.pop()  # 这里pop是为了使得最后一个数据在一分钟内也是实时的
                # self.listfastEMA.pop()
                # self.listslowEMA.pop()
                self.listOpen.pop()
                self.listClose.pop()
                self.listHigh.pop()
                self.listLow.pop()
                #self.listOpenInterest.pop()
                self.onBar(self.num, self.barOpen, self.barClose, self.barLow, self.barHigh, self.barOpenInterest)
            # 如果是新一分钟的数据
            else:
                # 先保存K线收盘价
                self.num += 1
                self.onBar(self.num, self.barOpen, self.barClose, self.barLow, self.barHigh, self.barOpenInterest)
                # 初始化新的K线数据
                self.barOpen = tick.lastPrice
                self.barHigh = tick.lastPrice
                self.barLow = tick.lastPrice
                self.barClose = tick.lastPrice
                self.barTime = self.ticktime
                self.barOpenInterest = tick.openInterest

    #----------------------------------------------------------------------
    def onBar(self, n, o, c, l, h, oi):
        self.listBar.append((n, o, c, l, h))
        self.listOpen.append(o)
        self.listClose.append(c)
        self.listHigh.append(h)
        self.listLow.append(l)
        #self.listOpenInterest.append(oi)

        ##计算K线图EMA均线
        #if self.fastEMA:
        #    self.fastEMA = c*self.EMAFastAlpha + self.fastEMA*(1-self.EMAFastAlpha)
        #    self.slowEMA = c*self.EMASlowAlpha + self.slowEMA*(1-self.EMASlowAlpha)
        #else:
        #    self.fastEMA = c
        #    self.slowEMA = c
        # self.listfastEMA.append(self.fastEMA)
        # self.listslowEMA.append(self.slowEMA)

        # 调用画图函数
        self.plotKline()     # K线图
        #self.plotTendency()  # K线副图，持仓量

    #----------------------------------------------------------------------
    def __connectMongo(self):
        """连接MongoDB数据库"""
        try:
            self.__mongoConnection = MongoClient()
            self.__mongoConnected = True
            self.__mongoTickDB = self.__mongoConnection['VnTrader_Tick_Db']
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
                cx = collection.find({'date': {'$gte': startDate, '$lte': endDate}})
            else:
                cx = collection.find({'date': {'$gte': startDate}})
            return cx
        else:
            return None

    #----------------------------------------------------------------------
    def registerEvent(self):
        """注册事件监听"""
        self.signal.connect(self.updateMarketData)
        self.__eventEngine.register(EVENT_TICK, self.signal.emit)




