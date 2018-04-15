# encoding: UTF-8

from collections import OrderedDict

from vnpy.event import Event
from vnpy.trader.uiQt import QtWidgets, QtCore
from vnpy.trader.uiBasicWidget import (BasicMonitor, BasicCell, PnlCell,
                                       AskCell, BidCell, BASIC_FONT)

from .stBase import (EVENT_SPREADTRADING_TICK, EVENT_SPREADTRADING_POS,
                     EVENT_SPREADTRADING_LOG, EVENT_SPREADTRADING_ALGO,
                     EVENT_SPREADTRADING_ALGOLOG)
from .stAlgo import StAlgoTemplate


STYLESHEET_START = 'background-color: rgb(111,255,244); color: black'
STYLESHEET_STOP = 'background-color: rgb(255,201,111); color: black'

# ----------------------------------------------------------------------
def log_row(func):
    """简单装饰器用于输出行号"""
    def wrapper(*args, **kw):
        print ""
        print str(func.__name__)
        return func(*args, **kw)
    return wrapper

########################################################################
class StTickMonitor(BasicMonitor):
    """价差行情监控"""

    #----------------------------------------------------------------------
    def __init__(self, mainEngine, eventEngine, parent=None):
        """Constructor"""
        super(StTickMonitor, self).__init__(mainEngine, eventEngine, parent)

        d = OrderedDict()
        d['name'] = {'chinese':u'价差名称', 'cellType':BasicCell}
        d['bidPrice'] = {'chinese':u'买价', 'cellType':BidCell}
        d['bidVolume'] = {'chinese':u'买量', 'cellType':BidCell}
        d['askPrice'] = {'chinese':u'卖价', 'cellType':AskCell}
        d['askVolume'] = {'chinese':u'卖量', 'cellType':AskCell}
        d['time'] = {'chinese':u'时间', 'cellType':BasicCell}
        d['symbol'] = {'chinese':u'价差公式', 'cellType':BasicCell}
        self.setHeaderDict(d)

        self.setDataKey('name')
        self.setEventType(EVENT_SPREADTRADING_TICK)
        self.setFont(BASIC_FONT)

        self.initTable()
        self.registerEvent()


########################################################################
class StPosMonitor(BasicMonitor):
    """价差持仓监控"""

    #----------------------------------------------------------------------
    def __init__(self, mainEngine, eventEngine, parent=None):
        """Constructor"""
        super(StPosMonitor, self).__init__(mainEngine, eventEngine, parent)

        d = OrderedDict()
        d['name'] = {'chinese':u'价差名称', 'cellType':BasicCell}
        d['netPos'] = {'chinese':u'净仓', 'cellType':PnlCell}
        d['longPos'] = {'chinese':u'多仓', 'cellType':BasicCell}
        d['shortPos'] = {'chinese':u'空仓', 'cellType':BasicCell}
        d['symbol'] = {'chinese':u'代码', 'cellType':BasicCell}
        self.setHeaderDict(d)

        self.setDataKey('name')
        self.setEventType(EVENT_SPREADTRADING_POS)
        self.setFont(BASIC_FONT)

        self.initTable()
        self.registerEvent()


########################################################################
class StLogMonitor(QtWidgets.QTextEdit):
    """价差日志监控"""
    signal = QtCore.Signal(type(Event()))

    #----------------------------------------------------------------------
    def __init__(self, mainEngine, eventEngine, parent=None):
        """Constructor"""
        super(StLogMonitor, self).__init__(parent)

        self.eventEngine = eventEngine

        self.registerEvent()

    #----------------------------------------------------------------------
    def processLogEvent(self, event):
        """处理日志事件"""
        log = event.dict_['data']
        content = '%s:%s' %(log.logTime, log.logContent)
        self.append(content)

    #----------------------------------------------------------------------
    def registerEvent(self):
        """注册事件监听"""
        self.signal.connect(self.processLogEvent)

        self.eventEngine.register(EVENT_SPREADTRADING_LOG, self.signal.emit)


########################################################################
class StAlgoLogMonitor(BasicMonitor):
    """价差日志监控"""

    #----------------------------------------------------------------------
    def __init__(self, mainEngine, eventEngine, parent=None):
        """Constructor"""
        super(StAlgoLogMonitor, self).__init__(mainEngine, eventEngine, parent)

        d = OrderedDict()
        d['logTime'] = {'chinese':u'时间', 'cellType':BasicCell}
        d['logContent'] = {'chinese':u'信息', 'cellType':BasicCell}
        self.setHeaderDict(d)

        self.setEventType(EVENT_SPREADTRADING_ALGOLOG)
        self.setFont(BASIC_FONT)

        self.initTable()
        self.registerEvent()


########################################################################
class StBuyPriceSpinBox(QtWidgets.QDoubleSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, price=0, parent=None):
        """Constructor"""
        super(StBuyPriceSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.price = price
        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.setValue(price)

        self.valueChanged.connect(self.setPrice)

    #----------------------------------------------------------------------
    def setPrice(self, value):
        """设置价格"""
        self.price = value

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)


########################################################################
class StSellPriceSpinBox(QtWidgets.QDoubleSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, price=0, parent=None):
        """Constructor"""
        super(StSellPriceSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.price = price
        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.setValue(price)

        self.valueChanged.connect(self.setPrice)

    #----------------------------------------------------------------------
    def setPrice(self, value):
        """设置价格"""
        self.price = value

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)


########################################################################
class StShortPriceSpinBox(QtWidgets.QDoubleSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self,  spreadName, price=0, parent=None):
        """Constructor"""
        super(StShortPriceSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.price = price
        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.setValue(price)

        self.valueChanged.connect(self.setPrice)

    #----------------------------------------------------------------------
    def setPrice(self, value):
        """设置价格"""
        self.price = value

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)


########################################################################
class StCoverPriceSpinBox(QtWidgets.QDoubleSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, price=0, parent=None):
        """Constructor"""
        super(StCoverPriceSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.price = price
        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.setValue(price)

        self.valueChanged.connect(self.setPrice)

    #----------------------------------------------------------------------
    def setPrice(self, value):
        """设置价格"""
        self.price = value

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)

########################################################################
class StTriggerVolumeSpinBox(QtWidgets.QDoubleSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, volume=0, parent=None):
        """Constructor"""
        super(StTriggerVolumeSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.volume = volume
        self.setDecimals(0)
        self.setRange(0, 10000)
        self.setValue(volume)

        self.valueChanged.connect(self.setVolume)

    #----------------------------------------------------------------------
    def setVolume(self, value):
        """设置价格"""
        self.volume = value

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)

########################################################################
class StMaxPosSizeSpinBox(QtWidgets.QSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, size=0, parent=None):
        """Constructor"""
        super(StMaxPosSizeSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.size = size
        self.setRange(-10000, 10000)
        self.setValue(size)

        self.valueChanged.connect(self.setSize)

    #----------------------------------------------------------------------
    def setSize(self, size):
        """设置价格"""
        self.size = size

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)


########################################################################
class StMaxOrderSizeSpinBox(QtWidgets.QSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, size=0, parent=None):
        """Constructor"""
        super(StMaxOrderSizeSpinBox, self).__init__(parent)

        self.spreadName = spreadName

        self.size = size
        self.setRange(-10000, 10000)
        self.setValue(size)

        self.valueChanged.connect(self.setSize)

    #----------------------------------------------------------------------
    def setSize(self, size):
        """设置价格"""
        self.size = size

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)


########################################################################
class StModeComboBox(QtWidgets.QComboBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, mode=StAlgoTemplate.MODE_LONGSHORT, parent=None):
        """Constructor"""
        super(StModeComboBox, self).__init__(parent)
        
        self.spreadName = spreadName

        l = [StAlgoTemplate.MODE_LONGSHORT, 
             StAlgoTemplate.MODE_LONGONLY,
             StAlgoTemplate.MODE_SHORTONLY]
        self.addItems(l)
        self.setCurrentIndex(l.index(mode))
        self.mode = unicode(self.currentText())

        self.currentIndexChanged.connect(self.setMode)
        
    #----------------------------------------------------------------------
    def setMode(self):
        """设置模式"""
        self.mode = unicode(self.currentText())

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时修改运行模式
        if active:
            self.setEnabled(False)
        else:
            self.setEnabled(True)


########################################################################
class StActiveButton(QtWidgets.QPushButton):
    """"""
    signalActive = QtCore.Signal(bool)

    #----------------------------------------------------------------------
    def __init__(self, alogManager, algoEngine, spreadName, parent=None):
        """Constructor"""
        super(StActiveButton, self).__init__(parent)

        self.algoManager = alogManager
        self.algoEngine = algoEngine
        self.spreadName = spreadName

        self.active = False
        self.setStopped()

        self.clicked.connect(self.buttonClicked)

    #----------------------------------------------------------------------
    def buttonClicked(self):
        """改变运行模式"""
        if self.active:
            self.stop()
        else:
            self.start()

    #----------------------------------------------------------------------
    def stop(self):
        """停止"""
        algoActive = self.algoEngine.stopAlgoGroup(self.spreadName)
        if not algoActive:
            self.setStopped()

    #----------------------------------------------------------------------
    def start(self):
        """启动"""
        self.algoManager.setAlgoGroupPara(self.spreadName)
        algoActive = self.algoEngine.startAlgoGroup(self.spreadName)
        if algoActive:
            self.setStarted()

    #----------------------------------------------------------------------
    def setStarted(self):
        """算法启动"""
        self.setText(u'运行中')
        self.setStyleSheet(STYLESHEET_START)

        self.active = True
        self.algoManager.AlgoGroupActive[self.spreadName] = True
        self.algoManager.setAlgoWidgetsEnabled(self.spreadName, False)
        self.signalActive.emit(self.active)

    #----------------------------------------------------------------------
    def setStopped(self):
        """算法停止"""
        self.setText(u'已停止')
        self.setStyleSheet(STYLESHEET_STOP)

        self.active = False
        self.algoManager.AlgoGroupActive[self.spreadName] = False
        self.algoManager.setAlgoWidgetsEnabled(self.spreadName, True)
        self.signalActive.emit(self.active)

########################################################################
class StAlgoManager(QtWidgets.QTableWidget):
    """价差算法管理组件"""

    #----------------------------------------------------------------------
    def __init__(self, stEngine, parent=None):
        """Constructor"""
        super(StAlgoManager, self).__init__(parent)
        
        self.algoEngine = stEngine.algoEngine

        self.tableManager = []    # list of list
        self.buttonActiveDict = {}       # spreadName: buttonActive
        self.AlgoGroupActive = {}       # spreadName : isActive

        self.initUi()
        
    #----------------------------------------------------------------------
    def initUi(self):
        """初始化表格"""
        self.headers = [u'价差',
                        u'算法',
                        u'多开价',
                        u'多平价',
                        u'空开价',
                        u'空平价',
                        u'触发数量',
                        u'委托上限',
                        u'持仓上限',
                        u'模式',
                        u'操作',
                        u'状态']
        # self.headers = [u'价差', u'算法', 'BuyPrice', 'SellPrice', 'ShortPrice', 'CoverPrice', u'触发数量', u'委托上限', u'持仓上限', u'模式', u'操作', u'状态']
        self.widgetHeaders = ["spreadName", "spinBuyPrice", "spinSellPrice", "spinShortPrice", "spinCoverPrice", "spinMaxOrderSize", "spinTriggerVolume", "spinMaxPosSize", "comboMode", "isFirstRow"]
        self.setColumnCount(len(self.headers))
        self.setHorizontalHeaderLabels(self.headers)
        self.horizontalHeader().setResizeMode(QtWidgets.QHeaderView.Stretch)
        
        self.verticalHeader().setVisible(False)
        self.setEditTriggers(self.NoEditTriggers)
        self.cellClicked.connect(self.onCellClicked)
        
    #----------------------------------------------------------------------
    def initCells(self):
        """初始化单元格"""
        paraGroupList = self.algoEngine.getAllAlgoParams()
        rowNum = 0
        for paraGroup in paraGroupList:
            rowNum += len(paraGroup)
        self.setRowCount(rowNum)

        row = 0
        for paraGroup in paraGroupList:
            for i, paraDict in enumerate(paraGroup):
                if i == 0:
                    cellSpreadName = QtWidgets.QTableWidgetItem(paraDict['spreadName'])
                    self.setItem(row, self.headers.index(u'价差'), cellSpreadName)
                    cellAlgoName = QtWidgets.QTableWidgetItem(paraDict['algoName'])
                    self.setItem(row, self.headers.index(u'算法'), cellAlgoName)
                    cellAdd = QtWidgets.QTableWidgetItem(u'增加行')
                    self.setItem(row, self.headers.index(u'操作'), cellAdd)
                    buttonActive = StActiveButton(self, self.algoEngine, paraDict['spreadName'])
                    self.setCellWidget(row, self.headers.index(u'状态'), buttonActive)
                    self.buttonActiveDict[paraDict['spreadName']] = buttonActive
                else:
                    cellDelete = QtWidgets.QTableWidgetItem(u'删除行')
                    self.setItem(row, self.headers.index(u'操作'), cellDelete)

                row_widgets = self.createRowWidgets(paraDict['spreadName'], paraDict, (i == 0))
                self.setRowWidges(row, row_widgets)
                self.tableManager.append(row_widgets)

                row += 1

    #----------------------------------------------------------------------
    def createRowWidgets(self, spreadName, dict=None, isFisrtRow=False):
        if dict:
            spinBuyPrice = StBuyPriceSpinBox(spreadName, dict['buyPrice'])
            spinSellPrice = StSellPriceSpinBox(spreadName, dict['sellPrice'])
            spinShortPrice = StShortPriceSpinBox(spreadName, dict['shortPrice'])
            spinCoverPrice = StCoverPriceSpinBox(spreadName, dict['coverPrice'])
            spinTriggerVolume = StTriggerVolumeSpinBox(spreadName, dict['triggerVolume'])
            spinMaxOrderSize = StMaxOrderSizeSpinBox(spreadName, dict['maxOrderSize'])
            spinMaxPosSize = StMaxPosSizeSpinBox(spreadName, dict['maxPosSize'])
            comboMode = StModeComboBox(spreadName, dict['mode'])
        else:
            spinBuyPrice = StBuyPriceSpinBox(spreadName)
            spinSellPrice = StSellPriceSpinBox(spreadName)
            spinShortPrice = StShortPriceSpinBox(spreadName)
            spinCoverPrice = StCoverPriceSpinBox(spreadName)
            spinTriggerVolume = StTriggerVolumeSpinBox(spreadName)
            spinMaxOrderSize = StMaxOrderSizeSpinBox(spreadName)
            spinMaxPosSize = StMaxPosSizeSpinBox(spreadName)
            comboMode = StModeComboBox(spreadName)
        row_widgets = [spreadName, spinBuyPrice, spinSellPrice, spinShortPrice, spinCoverPrice,
                       spinTriggerVolume,spinMaxOrderSize, spinMaxPosSize, comboMode, isFisrtRow]
        return row_widgets

    #----------------------------------------------------------------------
    def setRowWidges(self, row, row_widgets):
        self.setCellWidget(row, self.headers.index(u'多开价'), row_widgets[1])
        self.setCellWidget(row, self.headers.index(u'多平价'), row_widgets[2])
        self.setCellWidget(row, self.headers.index(u'空开价'), row_widgets[3])
        self.setCellWidget(row, self.headers.index(u'空平价'), row_widgets[4])
        self.setCellWidget(row, self.headers.index(u'触发数量'), row_widgets[5])
        self.setCellWidget(row, self.headers.index(u'委托上限'), row_widgets[6])
        self.setCellWidget(row, self.headers.index(u'持仓上限'), row_widgets[7])
        self.setCellWidget(row, self.headers.index(u'模式'), row_widgets[8])

    #----------------------------------------------------------------------
    def setAlgoGroupPara(self, spreadName):
        paraList = self.getParaList(spreadName)
        self.algoEngine.setAlgoGroupParams(spreadName, paraList)

    #----------------------------------------------------------------------
    def setAlgoWidgetsEnabled(self, spreadName, can_modify):
        for row,row_widgets in enumerate(self.tableManager):
            [rowSpreadName, spinBuyPrice, spinSellPrice, spinShortPrice, spinCoverPrice,
             spinTriggerVolume, spinMaxOrderSize, spinMaxPosSize, comboMode, isFisrtRow] = row_widgets
            if rowSpreadName == spreadName:
                spinBuyPrice.setEnabled(can_modify)
                spinSellPrice.setEnabled(can_modify)
                spinShortPrice.setEnabled(can_modify)
                spinCoverPrice.setEnabled(can_modify)
                spinTriggerVolume.setEnabled(can_modify)
                spinMaxOrderSize.setEnabled(can_modify)
                spinMaxPosSize.setEnabled(can_modify)
                comboMode.setEnabled(can_modify)

    # ----------------------------------------------------------------------
    def getParaList(self, spreadName):
        paraList = []
        for row,row_widgets in enumerate(self.tableManager):
            [rowSpreadName, spinBuyPrice, spinSellPrice, spinShortPrice, spinCoverPrice,
             spinTriggerVolume, spinMaxOrderSize, spinMaxPosSize, comboMode, isFisrtRow] = row_widgets
            if rowSpreadName == spreadName:
                para = {}
                para['buyPrice'] = spinBuyPrice.price
                para['sellPrice'] = spinSellPrice.price
                para['shortPrice'] = spinShortPrice.price
                para['coverPrice'] = spinCoverPrice.price
                para['triggerVolume'] = spinTriggerVolume.volume
                para['maxOrderSize'] = spinMaxOrderSize.size
                para['maxPosSize'] = spinMaxPosSize.size
                para['mode'] = comboMode.mode
                paraList.append(para)
        return paraList

    # ----------------------------------------------------------------------
    def onCellClicked(self, row, colume):
        operationColume = self.headers.index(u'操作')
        if operationColume == colume:
            ## 说明需要进行增加行或者删除行操作
            clickedRow = self.tableManager[row]
            spreadName = clickedRow[0]
            if self.AlgoGroupActive[spreadName]:
                ## 激活状态下不能修改
                self.algoEngine.writeLog("%s 在激活状态下不能增加或删除行" % spreadName)
                return

            if clickedRow[self.widgetHeaders.index("isFirstRow")]:
                ## 增加行
                self.insertAlgoRow(spreadName, self.findInsertLocation(spreadName))
            else:
                ## 删除行
                self.deleteAlgoRow(spreadName, row)
        pass

    #----------------------------------------------------------------------
    def findInsertLocation(self, spreadName):
        flag = False
        for i, rowInfo in enumerate(self.tableManager):
            rowSpreadName = rowInfo[0]
            if not flag:
                ## 还未找到第一个
                if rowSpreadName == spreadName:
                    flag = True
            else:
                ## 找下一个不是本Spread的行号
                if rowSpreadName != spreadName:
                    return i
        return len(self.tableManager)

    #----------------------------------------------------------------------
    def stopAll(self):
        """停止所有算法"""
        for button in self.buttonActiveDict.values():
            button.stop()

    #----------------------------------------------------------------------
    def insertAlgoRow(self, spreadName, row):
        """插入一个算法行"""
        self.insertRow(row)
        row_widgets = self.createRowWidgets(spreadName)
        self.setRowWidges(row, row_widgets)
        self.tableManager.insert(row, row_widgets)

        cellDelete = QtWidgets.QTableWidgetItem(u'删除行')
        operationCol = self.headers.index(u'操作')
        self.setItem(row, operationCol, cellDelete)

    def deleteAlgoRow(self, spreadName, row):
        self.removeRow(row)
        del self.tableManager[row]

########################################################################
class StGroup(QtWidgets.QGroupBox):
    """集合显示"""

    #----------------------------------------------------------------------
    def __init__(self, widget, title, parent=None):
        """Constructor"""
        super(StGroup, self).__init__(parent)
        
        self.setTitle(title)
        vbox = QtWidgets.QVBoxLayout()
        vbox.addWidget(widget)
        self.setLayout(vbox)
        

########################################################################
class StManager(QtWidgets.QWidget):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, stEngine, eventEngine, parent=None):
        """Constructor"""
        super(StManager, self).__init__(parent)
        
        self.stEngine = stEngine
        self.mainEngine = stEngine.mainEngine
        self.eventEngine = eventEngine
        
        self.initUi()
        
    #----------------------------------------------------------------------
    def initUi(self):
        """初始化界面"""
        self.setWindowTitle(u'价差交易')
        
        # 创建组件
        tickMonitor = StTickMonitor(self.mainEngine, self.eventEngine)
        posMonitor = StPosMonitor(self.mainEngine, self.eventEngine)
        logMonitor = StLogMonitor(self.mainEngine, self.eventEngine)
        self.algoManager = StAlgoManager(self.stEngine)
        algoLogMonitor = StAlgoLogMonitor(self.mainEngine, self.eventEngine)
        
        # 创建按钮
        buttonInit = QtWidgets.QPushButton(u'初始化')
        buttonInit.clicked.connect(self.init)       
        
        buttonStopAll = QtWidgets.QPushButton(u'全部停止')
        buttonStopAll.clicked.connect(self.algoManager.stopAll)
        
        # 创建集合
        groupTick = StGroup(tickMonitor, u'价差行情')
        groupPos = StGroup(posMonitor, u'价差持仓')
        groupLog = StGroup(logMonitor, u'日志信息')
        groupAlgo = StGroup(self.algoManager, u'价差算法')
        groupAlgoLog = StGroup(algoLogMonitor, u'算法信息')
        
        # 设置布局
        hbox = QtWidgets.QHBoxLayout()
        hbox.addWidget(buttonInit)
        hbox.addStretch()
        hbox.addWidget(buttonStopAll)
        
        grid = QtWidgets.QGridLayout()
        grid.addLayout(hbox, 0, 0, 1, 2)
        grid.addWidget(groupTick, 1, 0)
        grid.addWidget(groupPos, 1, 1)
        grid.addWidget(groupAlgo, 2, 0, 1, 2)
        grid.addWidget(groupLog, 3, 0)
        grid.addWidget(groupAlgoLog, 3, 1)

        self.setLayout(grid)
        
    #----------------------------------------------------------------------
    def show(self):
        """重载显示"""
        self.showMaximized()
        
    #----------------------------------------------------------------------
    def init(self):
        """初始化"""
        self.stEngine.init()
        self.algoManager.initCells()
    
    
    
    