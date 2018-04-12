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
    def __init__(self, spreadName, price, parent=None):
        """Constructor"""
        super(StBuyPriceSpinBox, self).__init__(parent)
        
        self.spreadName = spreadName

        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.price = 0
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
    def __init__(self, spreadName, price, parent=None):
        """Constructor"""
        super(StSellPriceSpinBox, self).__init__(parent)
        
        self.spreadName = spreadName

        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.price = 0
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
    def __init__(self,  spreadName, price, parent=None):
        """Constructor"""
        super(StShortPriceSpinBox, self).__init__(parent)
        
        self.spreadName = spreadName

        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.price = 0
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
    def __init__(self, spreadName, price, parent=None):
        """Constructor"""
        super(StCoverPriceSpinBox, self).__init__(parent)
        
        self.spreadName = spreadName

        self.setDecimals(4)
        self.setRange(-10000, 10000)
        self.price = 0
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
class StMaxPosSizeSpinBox(QtWidgets.QSpinBox):
    """"""

    #----------------------------------------------------------------------
    def __init__(self, spreadName, size, parent=None):
        """Constructor"""
        super(StMaxPosSizeSpinBox, self).__init__(parent)
        
        self.spreadName = spreadName

        self.size = 0
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
    def __init__(self, spreadName, size, parent=None):
        """Constructor"""
        super(StMaxOrderSizeSpinBox, self).__init__(parent)
        
        self.spreadName = spreadName

        self.size = 0
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
    def __init__(self, spreadName, mode, parent=None):
        """Constructor"""
        super(StModeComboBox, self).__init__(parent)
        
        self.spreadName = spreadName

        l = [StAlgoTemplate.MODE_LONGSHORT, 
             StAlgoTemplate.MODE_LONGONLY,
             StAlgoTemplate.MODE_SHORTONLY]
        self.addItems(l)
        self.setCurrentIndex(l.index(mode))
        
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
        algoActive = self.algoEngine.startAlgoGroup(self.spreadName)
        if algoActive:
            self.setStarted()

    #----------------------------------------------------------------------
    def setStarted(self):
        """算法启动"""
        self.setText(u'运行中')
        self.setStyleSheet(STYLESHEET_START)

        self.active = True
        self.algoManager.startAlgoGroup(self.spreadName)
        self.signalActive.emit(self.active)

    #----------------------------------------------------------------------
    def setStopped(self):
        """算法停止"""
        self.setText(u'已停止')
        self.setStyleSheet(STYLESHEET_STOP)

        self.active = False
        self.algoManager.stopAlgoGroup(self.spreadName)
        self.signalActive.emit(self.active)


########################################################################
class StAddDeleteButton(QtWidgets.QPushButton):
    """"""
    signalActive = QtCore.Signal(bool)
    BUTTON_MOD_ADD = 0
    BUTTON_MOD_DELETE = 1

    #----------------------------------------------------------------------
    def __init__(self, algoEngine, spreadName, seqNum, stAlgoManager, row, parent=None):
        """Constructor"""
        super(StAddDeleteButton, self).__init__(parent)
        
        self.algoEngine = algoEngine
        self.spreadName = spreadName
        self.seqNum = seqNum
        self.algoManager = stAlgoManager
        self.row = row
        
        self.active = False
        self._setMode(seqNum)
        
        self.clicked.connect(self.buttonClicked)
        
    #----------------------------------------------------------------------
    def buttonClicked(self):
        """"""
        if self.mode == StAddDeleteButton.BUTTON_MOD_ADD:
            self.addAlgo()
        else:
            self.deleteAlgo(self.seqNum)

    #----------------------------------------------------------------------
    def addAlgo(self):
        """增加算法"""
        if self.active:
            return
        para = self.algoEngine.addAlgo(self.spreadName)
        self.algoManager.insertAlgoRow(self.spreadName, para)

    #----------------------------------------------------------------------
    def deleteAlgo(self, seqNum):
        """删除算法"""
        if self.active:
            return
        self.algoEngine.deleteAlgo(self.spreadName, seqNum)
        self.algoManager.removeRow(self.row)

    #----------------------------------------------------------------------
    def algoActiveChanged(self, active):
        """算法运行状态改变"""
        # 只允许算法停止时进行操作
        if active:
            self.active = True
        else:
            self.active = False

    #----------------------------------------------------------------------
    def _setMode(self, seqNum):
        """设置工作模式"""
        if seqNum == 0:
            self.mode = StAddDeleteButton.BUTTON_MOD_ADD
            self.setText(u'增加')
        else:
            self.mode = StAddDeleteButton.BUTTON_MOD_DELETE
            self.setText(u'删除')


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
        self.AlgoNumDict = {}   # spreadName: algoNum
        self.rowInfo = []

        self.initUi()
        
    #----------------------------------------------------------------------
    def initUi(self):
        """初始化表格"""
        self.headers = [u'价差',
                   u'算法',
                   'BuyPrice',
                   'SellPrice',
                   'CoverPrice',
                   'ShortPrice',
                   u'委托上限',
                   u'持仓上限',
                   u'模式',
                   u'操作',
                   u'状态']
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
        for paraGroup in paraGroupList:
            for i, paraDict in enumerate(paraGroup):
                if i == 0:
                    self.AlgoNumDict[paraDict['spreadName']] = len(paraGroup)
                    self.rowInfo.append([paraDict['spreadName'], 0])
                else:
                    self.rowInfo.append([paraDict['spreadName'], 1])
        rowNum = len(self.rowInfo)
        self.setRowCount(rowNum)

        row = 0
        for paraGroup in paraGroupList:
            for i, paraDict in enumerate(paraGroup):
                cellSpreadName = QtWidgets.QTableWidgetItem(paraDict['spreadName'])
                cellAlgoName = QtWidgets.QTableWidgetItem(paraDict['algoName'])
                spinBuyPrice = StBuyPriceSpinBox(paraDict['spreadName'], paraDict['buyPrice'])
                spinSellPrice = StSellPriceSpinBox(paraDict['spreadName'], paraDict['sellPrice'])
                spinShortPrice = StShortPriceSpinBox(paraDict['spreadName'], paraDict['shortPrice'])
                spinCoverPrice = StCoverPriceSpinBox(paraDict['spreadName'], paraDict['coverPrice'])
                spinMaxOrderSize = StMaxOrderSizeSpinBox(paraDict['spreadName'], paraDict['maxOrderSize'])
                spinMaxPosSize = StMaxPosSizeSpinBox(paraDict['spreadName'], paraDict['maxPosSize'])
                comboMode = StModeComboBox(paraDict['spreadName'], paraDict['mode'])

                self.setItem(row, 0, cellSpreadName)
                self.setItem(row, 1, cellAlgoName)
                self.setCellWidget(row, 2, spinBuyPrice)
                self.setCellWidget(row, 3, spinSellPrice)
                self.setCellWidget(row, 4, spinCoverPrice)
                self.setCellWidget(row, 5, spinShortPrice)
                self.setCellWidget(row, 6, spinMaxOrderSize)
                self.setCellWidget(row, 7, spinMaxPosSize)
                self.setCellWidget(row, 8, comboMode)

                row_manager = [spinBuyPrice, spinSellPrice, spinShortPrice, spinCoverPrice, spinMaxOrderSize, spinMaxPosSize]
                self.tableManager.append(row_manager)

                if i == 0:
                    cellAdd = QtWidgets.QTableWidgetItem(u'增加行')
                    buttonActive = StActiveButton(self, self.algoEngine, paraDict['spreadName'])
                    self.setItem(row, 9, cellAdd)
                    self.setCellWidget(row, 10, buttonActive)
                    self.buttonActiveDict[paraDict['spreadName']] = buttonActive
                else:
                    cellDelete = QtWidgets.QTableWidgetItem(u'删除行')
                    self.setItem(row, 9, cellDelete)

                row += 1

    #----------------------------------------------------------------------
    def startAlgoGroup(self, spreadName):
        pass

    #----------------------------------------------------------------------
    def stopAlgoGroup(self, spreadName):
        pass

    #----------------------------------------------------------------------
    def onCellClicked(self, row, colume):
        print row, colume
        operationCol = self.headers.index(u'操作')
        print operationCol
        if operationCol == colume:
            ## 说明需要进行增加行或者删除行操作
            [spreadName, seqNum] = self.rowInfo[row]
            if seqNum == 0:
                ## 要增加行
                new_row = row + self.AlgoNumDict[spreadName]
                self.insertAlgoRow(spreadName, new_row)
                self.rowInfo.insert(new_row, [spreadName, 1])
                self.AlgoNumDict[spreadName] += 1
            else:
                ## 要 删除行
                self.deleteAlgoRow(spreadName, row)
                del self.rowInfo[row]
                self.AlgoNumDict[spreadName] -= 1
        pass

    #----------------------------------------------------------------------
    def stopAll(self):
        """停止所有算法"""
        for button in self.buttonActiveDict.values():
            button.stop()

    #----------------------------------------------------------------------
    def insertAlgoRow(self, spreadName, row):
        """插入一个算法行"""
        print spreadName, row
        self.insertRow(row)
        new_row_num = self.AlgoNumDict[spreadName]
        algoEngine = self.algoEngine
        ##seqNum = paraDict['seqNum']
        ##row = seqNum
        ##cellSpreadName = QtWidgets.QTableWidgetItem(paraDict['spreadName'])
        ##cellSeqNum = QtWidgets.QTableWidgetItem(str(paraDict['seqNum']))
        ##cellAlgoName = QtWidgets.QTableWidgetItem(paraDict['algoName'])
        ##spinBuyPrice = StBuyPriceSpinBox(algoEngine, paraDict['spreadName'], paraDict['buyPrice'], seqNum)
        ##spinSellPrice = StSellPriceSpinBox(algoEngine, paraDict['spreadName'], paraDict['sellPrice'],seqNum)
        ##spinShortPrice = StShortPriceSpinBox(algoEngine, paraDict['spreadName'], paraDict['shortPrice'], seqNum)
        ##spinCoverPrice = StCoverPriceSpinBox(algoEngine, paraDict['spreadName'], paraDict['coverPrice'], seqNum)
        ##spinMaxOrderSize = StMaxOrderSizeSpinBox(algoEngine, paraDict['spreadName'], paraDict['maxOrderSize'], seqNum)
        ##spinMaxPosSize = StMaxPosSizeSpinBox(algoEngine, paraDict['spreadName'], paraDict['maxPosSize'], seqNum)
        ##comboMode = StModeComboBox(algoEngine, paraDict['spreadName'], paraDict['mode'], seqNum)
        ##buttonActive = StActiveButton(algoEngine, paraDict['spreadName'], seqNum)
        ##buttonAddDelete = StAddDeleteButton(algoEngine, paraDict['spreadName'], seqNum, self, row)

        ##self.insertRow(row)

        ##self.setItem(row, 0, cellSpreadName)
        ##self.setItem(row, 1, cellSeqNum)
        ##self.setItem(row, 2, cellAlgoName)
        ##self.setCellWidget(row, 3, spinBuyPrice)
        ##self.setCellWidget(row, 4, spinSellPrice)
        ##self.setCellWidget(row, 5, spinCoverPrice)
        ##self.setCellWidget(row, 6, spinShortPrice)
        ##self.setCellWidget(row, 7, spinMaxOrderSize)
        ##self.setCellWidget(row, 8, spinMaxPosSize)
        ##self.setCellWidget(row, 9, comboMode)
        ##self.setCellWidget(row, 10, buttonActive)
        ##self.setCellWidget(row, 11, buttonAddDelete)

        ##buttonActive.signalActive.connect(spinBuyPrice.algoActiveChanged)
        ##buttonActive.signalActive.connect(spinSellPrice.algoActiveChanged)
        ##buttonActive.signalActive.connect(spinShortPrice.algoActiveChanged)
        ##buttonActive.signalActive.connect(spinCoverPrice.algoActiveChanged)
        ##buttonActive.signalActive.connect(spinMaxOrderSize.algoActiveChanged)
        ##buttonActive.signalActive.connect(spinMaxPosSize.algoActiveChanged)
        ##buttonActive.signalActive.connect(comboMode.algoActiveChanged)
        ##buttonActive.signalActive.connect(buttonAddDelete.algoActiveChanged)

    def deleteAlgoRow(self, spreadName, row):
        print spreadName, row
        self.removeRow(row)



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
    
    
    
    