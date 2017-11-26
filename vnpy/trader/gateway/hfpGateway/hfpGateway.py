# encoding: UTF-8

'''
vn.hfp的gateway接入

'''


import os
import json
from copy import copy
from datetime import datetime

from vnpy.api.hfp import MdApi, TdApi, defineDict
from vnpy.trader.vtGateway import *
from vnpy.trader.vtFunction import getJsonPath, getTempPath
from vnpy.trader.vtConstant import GATEWAYTYPE_FUTURES
from .language import text

def print_dict(d):
    """按照键值打印一个字典"""
    for key,value in d.items():
        print key + ':' + str(value),
    print

#----------------------------------------------------------------------
def simple_log(func):
    """简单装饰器用于输出函数名"""
    def wrapper(*args, **kw):
        print ""
        print str(func.__name__)
        return func(*args, **kw)
    return wrapper

# 以下为一些VT类型和HFP类型的映射字典
# 价格类型映射
#priceTypeMap = {}
#priceTypeMap[PRICETYPE_LIMITPRICE] = defineDict["THOST_FTDC_OPT_LimitPrice"]
#priceTypeMap[PRICETYPE_MARKETPRICE] = defineDict["THOST_FTDC_OPT_AnyPrice"]
#priceTypeMapReverse = {v: k for k, v in priceTypeMap.items()}

# 方向类型映射
directionMap = {}
directionMap[DIRECTION_LONG] = True
directionMap[DIRECTION_SHORT] = False
directionMapReverse = {v: k for k, v in directionMap.items()}

# 开平类型映射
offsetMap = {}
#offsetMap[OFFSET_OPEN] = defineDict['open']
#offsetMap[OFFSET_CLOSE] = defineDict['close']
offsetMap[OFFSET_OPEN] = 1
offsetMap[OFFSET_CLOSE] = 2
#offsetMap[OFFSET_CLOSETODAY] = defineDict['THOST_FTDC_OF_CloseToday']
#offsetMap[OFFSET_CLOSEYESTERDAY] = defineDict['THOST_FTDC_OF_CloseYesterday']
offsetMapReverse = {v:k for k,v in offsetMap.items()}

# 交易所类型映射
exchangeMap = {}
exchangeMap[EXCHANGE_CFFEX] = 'CFFEX'
exchangeMap[EXCHANGE_SHFE] = 'SHFE'
exchangeMap[EXCHANGE_CZCE] = 'CZCE'
exchangeMap[EXCHANGE_DCE] = 'DCE'
exchangeMap[EXCHANGE_SSE] = 'SSE'
exchangeMap[EXCHANGE_INE] = 'INE'
exchangeMap[EXCHANGE_UNKNOWN] = ''
exchangeMapReverse = {v:k for k,v in exchangeMap.items()}

# 持仓类型映射
#posiDirectionMap = {}
#posiDirectionMap[DIRECTION_NET] = defineDict["THOST_FTDC_PD_Net"]
#posiDirectionMap[DIRECTION_LONG] = defineDict["THOST_FTDC_PD_Long"]
#posiDirectionMap[DIRECTION_SHORT] = defineDict["THOST_FTDC_PD_Short"]
#posiDirectionMapReverse = {v:k for k,v in posiDirectionMap.items()}

# 产品类型映射
#productClassMap = {}
#productClassMap[PRODUCT_FUTURES] = defineDict["THOST_FTDC_PC_Futures"]
#productClassMap[PRODUCT_OPTION] = defineDict["THOST_FTDC_PC_Options"]
#productClassMap[PRODUCT_COMBINATION] = defineDict["THOST_FTDC_PC_Combination"]
#productClassMapReverse = {v:k for k,v in productClassMap.items()}

# 委托状态映射
statusMap = {}
statusMap[STATUS_ALLTRADED] = defineDict["alldeal"]
statusMap[STATUS_PARTTRADED] = defineDict["partdeal"]
statusMap[STATUS_NOTTRADED] = defineDict["nodeal"]
statusMap[STATUS_CANCELLED] = defineDict["canceled"]
statusMap[STATUS_UNKNOWN] = ""
statusMapReverse = {v:k for k,v in statusMap.items()}


########################################################################
class HfpGateway(VtGateway):
    """HFP接口"""

    #----------------------------------------------------------------------
    def __init__(self, eventEngine, gatewayName='HFP'):
        """Constructor"""
        super(HfpGateway, self).__init__(eventEngine, gatewayName)
        
        self.mdApi = HfpMdApi(self)     # 行情API
        self.tdApi = HfpTdApi(self)     # 交易API
        
        self.mdConnected = False        # 行情API连接状态，登录完成后为True
        self.tdConnected = False        # 交易API连接状态
        
        self.qryEnabled = True         # 循环查询
        
        self.fileName = self.gatewayName + '_connect.json'
        self.filePath = getJsonPath(self.fileName, __file__)        
        
    #----------------------------------------------------------------------
    def connect(self):
        """连接"""
        try:
            f = file(self.filePath)
        except IOError:
            log = VtLogData()
            log.gatewayName = self.gatewayName
            log.logContent = text.LOADING_ERROR
            self.onLog(log)
            return
        
        # 解析json文件
        setting = json.load(f)
        try:
            mdLicenseID = str(setting['mdLicenseID'])
            mdLicenseKey = str(setting['mdLicenseKey'])
            mdAddress = str(setting['mdAddress'])
            mdPort = str(setting['mdPort'])
            tdLicenseID = str(setting['tdLicenseID'])
            tdLicenseKey = str(setting['tdLicenseKey'])
            tdAddress = str(setting['tdAddress'])   
            tdPort = str(setting['tdPort'])
            userID = str(setting['userID'])
            password = str(setting['password'])

        except KeyError:
            log = VtLogData()
            log.gatewayName = self.gatewayName
            log.logContent = text.CONFIG_KEY_MISSING
            self.onLog(log)
            return            
        
        # 创建行情和交易接口对象
        self.mdApi.createHFPMdApi(mdLicenseID, mdLicenseKey)
        self.mdApi.connectMdFront(mdAddress, mdPort)
        
        self.tdApi.createHFPTdApi(tdLicenseID, tdLicenseKey)
        self.tdApi.connectTradeFront(tdAddress, int(tdPort))

        self.tdApi.setLoginInfo(userID, password)
        

    #----------------------------------------------------------------------
    def sendOrder(self, orderReq):
        """发单"""
        return self.tdApi.sendOrder(orderReq)
        
    #----------------------------------------------------------------------
    def cancelOrder(self, cancelOrderReq):
        """撤单"""
        self.tdApi.cancelOrder(cancelOrderReq)
        
    #----------------------------------------------------------------------
    def qryAccount(self):
        """查询账户资金"""
        self.tdApi.qryAccount()
        
    #----------------------------------------------------------------------
    def qryPosition(self):
        """查询持仓"""
        self.tdApi.qryPosition()

    #----------------------------------------------------------------------
    def qryAllOrder(self):
        """查询持仓"""
        self.tdApi.qryAllOrder()

    #----------------------------------------------------------------------
    def close(self):
        """关闭"""
        if self.mdConnected:
            self.mdApi.close()
        if self.tdConnected:
            self.tdApi.close()
        
    #----------------------------------------------------------------------
    def initQuery(self):
        """初始化连续查询"""
        if self.qryEnabled:
            # 需要循环的查询函数列表
            self.qryFunctionList = [self.qryAccount, self.qryPosition, self.qryAccount]
            
            self.qryCount = 0           # 查询触发倒计时
            self.qryTrigger = 2         # 查询触发点
            self.qryNextFunction = 0    # 上次运行的查询函数索引
            
            self.startQuery()
    
    #----------------------------------------------------------------------
    def query(self, event):
        """注册到事件处理引擎上的查询函数"""
        self.qryCount += 1
        
        if self.qryCount > self.qryTrigger:
            # 清空倒计时
            self.qryCount = 0
            
            # 执行查询函数
            function = self.qryFunctionList[self.qryNextFunction]
            function()
            
            # 计算下次查询函数的索引，如果超过了列表长度，则重新设为0
            self.qryNextFunction += 1
            if self.qryNextFunction == len(self.qryFunctionList):
                self.qryNextFunction = 0
    
    #----------------------------------------------------------------------
    def startQuery(self):
        """启动连续查询"""
        self.eventEngine.register(EVENT_TIMER, self.query)
    
    #----------------------------------------------------------------------
    def setQryEnabled(self, qryEnabled):
        """设置是否要启动循环查询"""
        self.qryEnabled = qryEnabled
    


########################################################################
class HfpMdApi(MdApi):
    """HFP行情API实现"""
    #----------------------------------------------------------------------
    def __init__(self, gateway):
        """Constructor"""
        super(HfpMdApi, self).__init__()
        
        self.gateway = gateway                  # gateway对象
        self.gatewayName = gateway.gatewayName  # gateway对象名称
        self.exchange = "WuXi"
        self.connectionStatus = False       # 连接状态
        self.subscribedSymbols = set()      # 已订阅合约代码

    #----------------------------------------------------------------------
    def onClientClosed(self, n):
        pass
    
    #----------------------------------------------------------------------
    def onClientConnected(self):
        pass
        
    #----------------------------------------------------------------------
    def onClientDisConnected(self, client, n):
        self.connectionStatus = False
        self.gateway.mdConnected = False
        self.writeLog(text.DATA_SERVER_DISCONNECTED)        
        pass
    
    #----------------------------------------------------------------------
    def onClienthandshaked(self, IsSuccess, index, code):
        self.connectionStatus = True
        self.writeLog(text.DATA_SERVER_CONNECTED)

    #----------------------------------------------------------------------
    def onQuotationInfo(self, data):
        """行情推送"""
        # 创建对象
        tick = VtTickData()
        tick.gatewayName = self.gatewayName
        tick.symbol = data['contract_id']
        tick.exchange = self.exchange
        tick.vtSymbol = '.'.join([tick.symbol, tick.exchange])
        tick.lastPrice = data['new']
        tick.volume = data['cur_volume']
        tick.openInterest = data['subs_volume']
        tick.time = time.strftime("%H:%M:%S", time.localtime(data["datetime"]))
        tick.date = datetime.now().strftime('%Y%m%d')
        tick.openPrice = data['open']
        tick.highPrice = data['high']
        tick.lowPrice = data['low']
        tick.preClosePrice = data['last_close']
        tick.upperLimit = data['up_limit']
        tick.lowerLimit = data['down_limit']
        tick.average = data['average']

        tick.bidPrice1 = data['buy_price_1']
        tick.bidVolume1 = data['buy_volume_1']
        tick.askPrice1 = data['sell_price_1']
        tick.askVolume1 = data['sell_volume_1']
        tick.bidPrice2 = data['buy_price_2']
        tick.bidVolume2 = data['buy_volume_2']
        tick.askPrice2 = data['sell_price_2']
        tick.askVolume2 = data['sell_volume_2']
        tick.bidPrice3 = data['buy_price_3']
        tick.bidVolume3 = data['buy_volume_3']
        tick.askPrice3 = data['sell_price_3']
        tick.askVolume3 = data['sell_volume_3']
        tick.bidPrice4 = data['buy_price_4']
        tick.bidVolume4 = data['buy_volume_4']
        tick.askPrice4 = data['sell_price_4']
        tick.askVolume4 = data['sell_volume_4']
        tick.bidPrice5 = data['buy_price_5']
        tick.bidVolume5 = data['buy_volume_5']
        tick.askPrice5 = data['sell_price_5']
        tick.askVolume5 = data['sell_volume_5']
        self.gateway.onTick(tick)

    #----------------------------------------------------------------------
    def close(self):
        """关闭"""
        self.exit()
        
    #----------------------------------------------------------------------
    def writeLog(self, content):
        """发出日志"""
        log = VtLogData()
        log.gatewayName = self.gatewayName
        log.logContent = content
        self.gateway.onLog(log)        


########################################################################
class HfpTdApi(TdApi):
    """HFP交易API实现"""
    
    #----------------------------------------------------------------------
    def __init__(self, gateway):
        """API对象的初始化函数"""
        super(HfpTdApi, self).__init__()
        
        self.gateway = gateway                  # gateway对象
        self.gatewayName = gateway.gatewayName  # gateway对象名称
        
        self.orderRef = EMPTY_INT           # 订单编号
        self.connectionStatus = False       # 连接状态
        self.loginStatus = False            # 登录状态
        self.clientID = EMPTY_STRING        # 客户号
        self.password = EMPTY_STRING        # 密码
        self.address = EMPTY_STRING         # 服务器地址
        self.frontID = EMPTY_INT            # 前置机编号
        self.marketID = "001"
        self.exchange = "WuXi"
        self.posDict = {}
        self.symbolExchangeDict = {}        # 保存合约代码和交易所的印射关系
        self.symbolSizeDict = {}            # 保存合约代码和合约大小的印射关系
        self.symbolStatusDict = {}          # 保存合约代码和合约状态的印射关系
        self.requireAuthentication = False;

    def setLoginInfo(self, clientID, passwd):
        self.clientID = clientID
        self.password = passwd

    #----------------------------------------------------------------------
    @simple_log    
    def onClientClosed(self, n):
        pass
    
    #----------------------------------------------------------------------
    def onClientConnected(self):
        pass
        
    #----------------------------------------------------------------------
    @simple_log    
    def onClientDisConnected(self, client, n):
        """服务器断开"""
        self.connectionStatus = False
        self.loginStatus = False
        self.gateway.tdConnected = False
        self.writeLog(text.TRADING_SERVER_DISCONNECTED)
        pass
    
    #----------------------------------------------------------------------
    @simple_log  
    def onClienthandshaked(self, IsSuccess, index, code):
        """握手连接"""
        self.connectionStatus = True
        self.writeLog(text.TRADING_SERVER_CONNECTED)

        if self.requireAuthentication:
            self.authenticate()
        else:
            self.login()
        pass

    #--------------------------------------------------------
    @simple_log
    def onLoginResponse(self, rsp):
        """登陆回报"""
        # 如果登录成功，推送日志信息
        if rsp['success'] == True:
            self.loginStatus = True
            self.gateway.tdConnected = True
            self.writeLog(text.TRADING_SERVER_LOGIN)
            self.qryOrder(self.marketID)
            self.qryAccount()
            self.qryPosition()
            self.reqContract()
        else:
            err = VtErrorData()
            err.gatewayName = self.gatewayName
            err.errorID = rsp['errcode']
#            err.errorMsg = rsp['errdesc'].decode('gbk')
            self.gateway.onError(err)

    #----------------------------------------------------------------------
    def onLogoutPush(self, outtype):
        """登出回报"""
        self.loginStatus = False
        self.gateway.tdConnected = False
        self.writeLog(text.TRADING_SERVER_LOGOUT)


    @simple_log
    def onAssociatorResponse(self, rsp, info):
        pass
    
    def onMarketStatePush(self, MarketState):
        self.symbolStatusDict[MarketState["instrument"]] = MarketState["status"]
        pass
    
    @simple_log
    def onMarketResponse(self, MarketResponse):
        self.marketID = MarketResponse["id"]
        pass
    
    @simple_log
    def onContractResponse(self, data):
        print_dict(data)
        contract = VtContractData()

        contract.symbol = data['contractid']  # 代码
        contract.exchange = self.exchange  # 交易所代码
        contract.vtSymbol = '.'.join([contract.symbol, contract.exchange])  # 合约在vt系统中的唯一代码，通常是 合约代码.交易所代码
        contract.name = data['contractname']  # 合约中文名
        contract.productClass = PRODUCT_FUTURES   # 合约类型
        contract.size = EMPTY_INT  # 合约大小
        contract.priceTick = data['mindiffprice']  # 合约最小价格TICK
        self.gateway.onContract(contract)

        pass
    
    @simple_log
    def onAccountResponse(self, data):
        account = VtAccountData()
        account.gatewayName = self.gatewayName
        account.accountID = data['memberid']  # 账户代码
        account.vtAccountID = '.'.join([account.gatewayName, account.accountID])  # 账户在vt中的唯一代码，通常是 Gateway名.账户代码

        # 数值相关
        account.preBalance = data['avlbfundout']  # 昨日账户结算净值
        account.balance = data['balance']  # 账户净值
        account.available = data['avlb']  # 可用资金
        account.commission = data['poundage']  # 今日手续费
        account.margin = data['occp']  # 保证金占用
        account.closeProfit = data['cnybalance']  # 平仓盈亏
        account.positionProfit = data['funddynamic']  # 持仓盈亏, 暂时用动态权益代替

        self.gateway.onAccount(account)
        pass

    @simple_log
    def onReceiptcollectResponse(self, ReceiptcollectResponse):
        pass


    #----------------------------------------------------------------------
    @simple_log
    def onOrderResponse(self, rsp, data):
        """发单错误（柜台）"""
        # 推送委托信息
        order = VtOrderData()
        order.gatewayName = self.gatewayName
        order.symbol = data['contractid']
        order.exchange = self.exchange
        order.vtSymbol = '.'.join([order.symbol, order.exchange])
        order.orderID = data['orderid']
        order.vtOrderID = '.'.join([self.gatewayName, order.orderID])
        order.direction = directionMapReverse.get(data['isbuy'])
        order.offset = offsetMapReverse.get(data['offsetflag'], OFFSET_UNKNOWN)
        order.status = STATUS_NOTTRADED
        order.price = data['price']
        order.totalVolume = data['qty']
        self.gateway.onOrder(order)

        # 推送错误信息
        err = VtErrorData()
        err.gatewayName = self.gatewayName
        err.errorID = rsp['errcode']
#        err.errorMsg = rsp['errdesc'].decode('gbk')
        self.gateway.onError(err)

        #每次收到rsp都要查询报单状态，因为API设计的很烂
        self.qryOrder(self.marketID)
        pass
    
    @simple_log
    def onQueryorderResponse(self, rsp, data):
        """报单回报"""
        print_dict(data)
        # 创建报单数据对象
        order = VtOrderData()

        order.gatewayName = self.gatewayName
        order.symbol = data['contractid']
        order.exchange = self.exchange
        order.vtSymbol = '.'.join([order.symbol, order.exchange])
        order.orderID = data['orderid']
        order.direction = directionMapReverse.get(data['isbuy'])
        order.offset = offsetMapReverse.get(data['offsetflag'])
        order.status = statusMapReverse.get(data['state'])

        # 价格、报单量等数值
        order.price = data['price']
        order.totalVolume = data['qty']
        order.tradedVolume = data['qty'] - data['leftqty']
        order.orderTime = time.strftime("%H:%M:%S", time.localtime(data["ordertime"]/1000))
        if order.status == STATUS_CANCELLED:
            order.cancelTime = time.strftime("%H:%M:%S", time.localtime(data["canceltime"]/1000))
        else:
            order.cancelTime = ""
        order.vtOrderID = '.'.join([self.gatewayName, order.orderID])

        # 推送
        self.gateway.onOrder(order)
        pass
    
    @simple_log
    def onCancelorderPush(self, data):
        print_dict(data)
        self.qryAllOrder()
        pass
    
    @simple_log
    def onCancelorderResponse(self, rsp, data):
        print_dict(data)
        # 创建报单数据对象
        err = VtErrorData()
        err.gatewayName = self.gatewayName
        err.errorID = rsp['errcode']
        #            err.errorMsg = rsp['errdesc'].decode('gbk')
        self.gateway.onError(err)

        pass
    
    @simple_log
    def onDealPush(self, data):
        """成交回报"""
        print_dict(data)
        # 创建报单数据对象
        trade = VtTradeData()
        trade.gatewayName = self.gatewayName

        # 保存代码和报单号
        trade.symbol = data['contractid']
        trade.exchange = self.exchange
        trade.vtSymbol = '.'.join([trade.symbol, trade.exchange])

        trade.tradeID = data['dealid']
        trade.vtTradeID = '.'.join([self.gatewayName, trade.tradeID])
        trade.orderID = data['orderid']
        trade.vtOrderID = '.'.join([self.gatewayName, trade.orderID])

        trade.direction = directionMapReverse.get(data['isbuy'], '')
        trade.offset = offsetMapReverse.get(data['offsetflag'], '')
        trade.price = data['price']
        trade.volume = data['qty']
        trade.tradeTime = time.strftime("%H:%M:%S", time.localtime(data["dealtime"]/1000))

        # 推送
        self.gateway.onTrade(trade)
        pass
    
    @simple_log
    def onQuerydealResponse(self, rsp, deal):
        pass
    
    @simple_log
    def onQueryPositioncollectResponse(self, rsp, data):

        position = VtPositionData()
        position.gatewayName = self.gatewayName
        # 代码编号相关
        position.symbol = data['contractid']  # 合约代码
        position.exchange = self.exchange
        position.vtSymbol = '.'.join([position.symbol, position.exchange])

        # 持仓相关
        position.direction = directionMapReverse.get(data['isbuy'])
        position.position = data['totalqty']  # 持仓量
        position.frozen = data['frzord']  # 冻结数量
        position.price = data['totalcost'] / data['totalqty'] # 持仓均价
        position.vtPositionName = '.'.join([position.vtSymbol, position.direction])  # 持仓在vt系统中的唯一代码，通常是vtSymbol.方向
        position.ydPosition = data['totalqty'] - data['totalqtytoday']  # 昨持仓
        position.positionProfit = data['balance']  # 持仓盈亏

        self.gateway.onPosition(position)

        pass
    
    @simple_log
    def onQueryPositiondetailResponse(self, rsp, position_detail):
        pass

    #----------------------------------------------------------------------
    def qryAccount(self):
        """查询账户"""
        self.reqAccount()
        pass

    #----------------------------------------------------------------------
    def qryPosition(self):
        """查询持仓"""
        self.qryPositioncollect(self.marketID)
        pass

    #----------------------------------------------------------------------
    def qryAllOrder(self):
        """查询所有报单"""
        self.qryOrder(self.marketID)
        pass
    #----------------------------------------------------------------------
    def login(self):
        """登陆"""
        loginReq = {}
        loginReq["userID"] = self.clientID
        loginReq["password"] = self.password
        self.reqUserLogin(loginReq)

    #----------------------------------------------------------------------
    @simple_log
    def sendOrder(self, orderReq):
        """发单"""
        seq = self.reqOrder(self.marketID, orderReq.symbol, self.clientID,
                directionMap[orderReq.direction],
                offsetMap[orderReq.offset],
                0, True, int(orderReq.price), orderReq.volume)

        # 返回订单号（字符串），便于某些算法进行动态管理
        vtOrderID = '.'.join([self.gatewayName, str(seq)])
        return vtOrderID
    
    #----------------------------------------------------------------------
    @simple_log
    def cancelOrder(self, cancelOrderReq):
        """撤单"""
        self.reqCancelOrder(self.marketID, cancelOrderReq.orderID)

    #----------------------------------------------------------------------
    @simple_log
    def close(self):
        """关闭"""
        ##self.exit()

    #----------------------------------------------------------------------
    def writeLog(self, content):
        """发出日志"""
        log = VtLogData()
        log.gatewayName = self.gatewayName
        log.logContent = content
        self.gateway.onLog(log)


