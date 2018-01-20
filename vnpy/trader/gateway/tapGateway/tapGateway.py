# encoding: UTF-8

'''
vn.tap的gateway接入

'''


import os
import json
from copy import copy
from datetime import datetime, timedelta

from vnpy.api.tap import MdApi, TdApi, defineDict
from vnpy.trader.vtGateway import *
from vnpy.trader.vtFunction import getJsonPath, getTempPath
from vnpy.trader.vtConstant import GATEWAYTYPE_FUTURES
from .language import text

#----------------------------------------------------------------------
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

# 以下为一些VT类型和TAP类型的映射字典
# 价格类型映射
priceTypeMap = {}
priceTypeMap[PRICETYPE_LIMITPRICE] = defineDict["TAPI_ORDER_TYPE_LIMIT"]
priceTypeMap[PRICETYPE_MARKETPRICE] = defineDict["TAPI_ORDER_TYPE_MARKET"]
priceTypeMapReverse = {v: k for k, v in priceTypeMap.items()} 

# 方向类型映射
directionMap = {}
directionMap[DIRECTION_LONG] = defineDict['TAPI_SIDE_BUY']
directionMap[DIRECTION_SHORT] = defineDict['TAPI_SIDE_SELL']
directionMapReverse = {v: k for k, v in directionMap.items()}

# 开平类型映射
#offsetMap = {}
#offsetMap[OFFSET_OPEN] = defineDict['']
#offsetMap[OFFSET_CLOSE] = defineDict['']
#offsetMap[OFFSET_CLOSETODAY] = defineDict['']
#offsetMap[OFFSET_CLOSEYESTERDAY] = defineDict['']
#offsetMapReverse = {v:k for k,v in offsetMap.items()}

# 交易所类型映射
exchangeMap = {}
exchangeMap[EXCHANGE_CFFEX] = 'CFFEX'
exchangeMap[EXCHANGE_SHFE] = 'SHFE'
exchangeMap[EXCHANGE_CZCE] = 'CZCE'
exchangeMap[EXCHANGE_DCE] = 'DCE'
exchangeMap[EXCHANGE_SSE] = 'SSE'
exchangeMap[EXCHANGE_INE] = 'INE'
exchangeMap[EXCHANGE_ASX] = 'ASX'
exchangeMap[EXCHANGE_BMD] = 'BMD'
exchangeMap[EXCHANGE_CBOE] = 'CBOE'
exchangeMap[EXCHANGE_CBOT] = 'CBOT'
exchangeMap[EXCHANGE_CME] = 'CME'
exchangeMap[EXCHANGE_COMEX] = 'COMEX'
exchangeMap[EXCHANGE_EUREX] = 'EUREX'
exchangeMap[EXCHANGE_HKEX] = 'HKEX'
exchangeMap[EXCHANGE_HKMEX] = 'HKMEX'
exchangeMap[EXCHANGE_ICCA] = 'ICCA'
exchangeMap[EXCHANGE_ICEU] = 'ICEU'
exchangeMap[EXCHANGE_ICUS] = 'ICUS'
exchangeMap[EXCHANGE_INE] = 'INE'
exchangeMap[EXCHANGE_LIFFE] = 'LIFFE'
exchangeMap[EXCHANGE_LME] = 'LME'
exchangeMap[EXCHANGE_NYMEX] = 'NYMEX'
exchangeMap[EXCHANGE_SGX] = 'SGX'
exchangeMap[EXCHANGE_TOCOM] = 'TOCOM'
exchangeMap[EXCHANGE_TSE] = 'TSE'
exchangeMap[EXCHANGE_UNKNOWN] = ''
exchangeMapReverse = {v:k for k,v in exchangeMap.items()}

# 持仓类型映射
posiDirectionMap = {}
posiDirectionMap[DIRECTION_LONG] = defineDict["TAPI_SIDE_BUY"]
posiDirectionMap[DIRECTION_SHORT] = defineDict["TAPI_SIDE_SELL"]
posiDirectionMapReverse = {v:k for k,v in posiDirectionMap.items()}

# 产品类型映射
productClassMap = {}
productClassMap[PRODUCT_FUTURES] = defineDict["TAPI_COMMODITY_TYPE_FUTURES"]
productClassMap[PRODUCT_OPTION] = defineDict["TAPI_COMMODITY_TYPE_OPTION"]
#productClassMap[PRODUCT_COMBINATION] = defineDict["THOST_FTDC_PC_Combination"]
#productClassMap[PRODUCT_SPOT] = defineDict["TAPI_COMMODITY_TYPE_SPOT"]
#productClassMap[PRODUCT_FUTURES] = defineDict["TAPI_COMMODITY_TYPE_FUTURES"]
#productClassMap[PRODUCT_OPTION] = defineDict["TAPI_COMMODITY_TYPE_OPTION"]
#productClassMap[PRODUCT_SPREAD_MONTH] = defineDict["TAPI_COMMODITY_TYPE_SPREAD_MONTH"]
#productClassMap[PRODUCT_SPREAD_COMMODITY] = defineDict["TAPI_COMMODITY_TYPE_SPREAD_COMMODITY"]
#productClassMap[PRODUCT_BUL] = defineDict["TAPI_COMMODITY_TYPE_BUL"]
#productClassMap[PRODUCT_BER] = defineDict["TAPI_COMMODITY_TYPE_BER"]
#productClassMap[PRODUCT_STD] = defineDict["TAPI_COMMODITY_TYPE_STD"]
#productClassMap[PRODUCT_STG] = defineDict["TAPI_COMMODITY_TYPE_STG"]
#productClassMap[PRODUCT_PRT] = defineDict["TAPI_COMMODITY_TYPE_PRT"]
#productClassMap[PRODUCT_DIRECTFOREX] = defineDict["TAPI_COMMODITY_TYPE_DIRECTFOREX"]
#productClassMap[PRODUCT_INDIRECTFOREX] = defineDict["TAPI_COMMODITY_TYPE_INDIRECTFOREX"]
#productClassMap[PRODUCT_CROSSFOREX] = defineDict["TAPI_COMMODITY_TYPE_CROSSFOREX"]
#productClassMap[PRODUCT_INDEX] = defineDict["TAPI_COMMODITY_TYPE_INDEX"]
#productClassMap[PRODUCT_STOCK] = defineDict["TAPI_COMMODITY_TYPE_STOCK"]
#productClassMap[PRODUCT_SPOT_TRADINGDEFER] = defineDict["TAPI_COMMODITY_TYPE_SPOT_TRADINGDEFER"]
productClassMapReverse = {v:k for k,v in productClassMap.items()}

# 委托状态映射
statusMap = {}
statusMap[STATUS_ALLTRADED] = defineDict["TAPI_ORDER_STATE_FINISHED"]
statusMap[STATUS_PARTTRADED] = defineDict["TAPI_ORDER_STATE_PARTFINISHED"]
statusMap[STATUS_NOTTRADED] = defineDict["TAPI_ORDER_STATE_QUEUED"]
statusMap[STATUS_CANCELLED] = defineDict["TAPI_ORDER_STATE_CANCELED"]
statusMapReverse = {v:k for k,v in statusMap.items()}

# 全局字典, key:symbol, value:exchange
symbolExchangeDict = {}

# 夜盘交易时间段分隔判断
NIGHT_TRADING = datetime(1900, 1, 1, 20).time()


########################################################################
class TapGateway(VtGateway):
    """CTP接口"""

    #----------------------------------------------------------------------
    def __init__(self, eventEngine, gatewayName='CTP'):
        """Constructor"""
        super(TapGateway, self).__init__(eventEngine, gatewayName)
        
        self.mdApi = TapMdApi(self)     # 行情API
        self.tdApi = TapTdApi(self)     # 交易API
        
        self.mdConnected = False        # 行情API连接状态，登录完成后为True
        self.tdConnected = False        # 交易API连接状态
        
        self.qryEnabled = False         # 循环查询
        
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
            authCode = str(setting['authCode'])
            mdUserID = str(setting['mdUserID'])
            mdPassword = str(setting['mdPassword'])
            mdAddress = str(setting['mdAddress'])
            mdPort = setting['mdPort']
            tdUserID = str(setting['tdUserID'])
            tdPassword = str(setting['tdPassword'])
            tdAddress = str(setting['tdAddress'])
            tdPort = setting['tdPort']

        except KeyError:
            log = VtLogData()
            log.gatewayName = self.gatewayName
            log.logContent = text.CONFIG_KEY_MISSING
            self.onLog(log)
            return            
        
        # 创建行情和交易接口对象
        self.mdApi.connect(authCode, mdUserID, mdPassword, mdAddress, mdPort)
        self.tdApi.connect(authCode, tdUserID, tdPassword, tdAddress, tdPort)

        # 初始化并启动查询
        self.initQuery()
    
    #----------------------------------------------------------------------
    def subscribe(self, subscribeReq):
        """订阅行情"""
        self.mdApi.subscribe(subscribeReq)
        
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
        self.tdApi.qryAccountGateway()
        
    #----------------------------------------------------------------------
    def qryPosition(self):
        """查询持仓"""
        self.tdApi.qryPositionGateway()
        
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
            self.qryFunctionList = [self.qryAccount, self.qryPosition]
            
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
class TapMdApi(MdApi):
    """CTP行情API实现"""

    #----------------------------------------------------------------------
    def __init__(self, gateway):
        """Constructor"""
        super(TapMdApi, self).__init__()
        
        self.gateway = gateway                  # gateway对象
        self.gatewayName = gateway.gatewayName  # gateway对象名称

        self.connectionStatus = False       # 连接状态
        self.loginStatus = False            # 登录状态
        
        self.subscribedSymbols = set()      # 已订阅合约代码        
        
        self.userID = EMPTY_STRING          # 账号
        self.password = EMPTY_STRING        # 密码
        self.brokerID = EMPTY_STRING        # 经纪商代码
        self.address = EMPTY_STRING         # 服务器地址
        self.port = EMPTY_INT
        
        self.tradingDt = None               # 交易日datetime对象
        self.tradingDate = EMPTY_STRING     # 交易日期字符串
        self.tickTime = None                # 最新行情time对象

    #----------------------------------------------------------------------
    def onRspLogin(self,  errorCode,  data):
        """登陆回报"""
        # 如果登录成功，推送日志信息
        if errorCode == 0:
            self.loginStatus = True
            self.gateway.mdConnected = True

            self.writeLog(text.DATA_SERVER_LOGIN)

            # 重新订阅之前订阅的合约
            for subscribeReq in self.subscribedSymbols:
                self.subscribe(subscribeReq)

            # 获取交易日
            # self.tradingDate = data['TradingDay']
            # self.tradingDt = datetime.strptime(self.tradingDate, '%Y%m%d')

        # 否则，推送错误信息
        else:
            err = VtErrorData()
            err.gatewayName = self.gatewayName
            err.errorID = errorCode
            err.errorMsg = ""
            self.gateway.onError(err)
        pass

    #----------------------------------------------------------------------
    def onAPIReady(self):
        self.isApiReady = True
        self.connectionStatus = True
        self.writeLog(text.DATA_SERVER_CONNECTED)

        req = VtSubscribeReq()
        req.symbol = "1803"
        req.exchange = "COMEX"
        req.productClass = "GC"
        self.subscribe(req)
        pass

    #----------------------------------------------------------------------
    def onDisconnect(self, reasonCode):
        self.connectionStatus = False
        self.loginStatus = False
        self.gateway.mdConnected = False
        self.writeLog(text.DATA_SERVER_DISCONNECTED)
        pass

    #----------------------------------------------------------------------
    def onRspQryCommodity(self,  errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    def onRspQryContract(self, errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    def onRspSubscribeQuote(self,  errorCode, isLast,  data):
        #print_dict(data)
        pass

    #----------------------------------------------------------------------
    def onRspUnSubscribeQuote(self, errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    def onRtnQuote(self,  data):
        """行情推送"""
        # 创建对象
        tick = VtTickData()
        tick.gatewayName = self.gatewayName

        tick.symbol = data['CommodityNo'] + " " + data["ContractNo1"]
        tick.exchange =  exchangeMapReverse.get(data['ExchangeNo'], '')
        tick.vtSymbol = '.'.join([tick.symbol, tick.exchange])

        tick.lastPrice = data['QLastPrice']            # 最新成交价
        tick.lastVolume = data['QLastQty']             # 最新成交量
        tick.volume = data['QTotalQty']                 # 今天总成交量
        tick.openInterest = data['QPrePositionQty']  # 持仓量
        tick.time = data['DateTimeStamp'][10:].replace('-','')                # 时间 11:20:56.5
        tick.date = data['DateTimeStamp'][:10]                # 日期 20151009
        tick.datetime = datetime.strptime(data['DateTimeStamp'], '%Y-%m-%d %H:%M:%S.%f')   # python的datetime时间对象

        tick.openPrice = data['QOpeningPrice']
        tick.highPrice = data['QHighPrice']
        tick.lowPrice = data['QLowPrice']
        tick.preClosePrice = data['QPreClosingPrice']
        tick.upperLimit = data['QLimitUpPrice']
        tick.lowerLimit = data['QLimitDownPrice']

        if data['QBidPrice1'] < data['QImpliedBidPrice']:
            tick.bidPrice1 = data['QImpliedBidPrice']
            tick.bidVolume1 = data['QImpliedBidQty']
        elif data['QBidPrice1'] == data['QImpliedBidPrice']:
            tick.bidPrice1 = data['QBidPrice1']
            tick.bidVolume1 = data['QBidQty1'] + data['QImpliedBidQty']
        else:
            tick.bidPrice1 = data['QBidPrice1']
            tick.bidVolume1 = data['QBidQty1']

        if data['QAskPrice1'] > data['QImpliedAskPrice']:
            tick.askPrice1 = data['QImpliedAskPrice']
            tick.askVolume1 = data['QImpliedAskQty']
        elif data['QAskPrice1'] == data['QImpliedAskPrice']:
            tick.askPrice1 = data['QAskPrice1']
            tick.askVolume1 = data['QAskQty1'] + data['QImpliedAskQty']
        else:
            tick.askPrice1 = data['QAskPrice1']
            tick.askVolume1 = data['QAskQty1']


        #tick.bidPrice2 = data['QBidPrice2']
        #tick.bidVolume2 = data['QBidQty2']
        #tick.askPrice2 = data['QAskPrice2']
        #tick.askVolume2 = data['QAskQty2']
        #tick.bidPrice3 = data['QBidPrice3']
        #tick.bidVolume3 = data['QBidQty3']
        #tick.askPrice3 = data['QAskPrice3']
        #tick.askVolume3 = data['QAskQty3']
        #tick.bidPrice4 = data['QBidPrice4']
        #tick.bidVolume4 = data['QBidQty4']
        #tick.askPrice4 = data['QAskPrice4']
        #tick.askVolume4 = data['QAskQty4']
        #tick.bidPrice5 = data['QBidPrice5']
        #tick.bidVolume5 = data['QBidQty5']
        #tick.askPrice5 = data['QAskPrice5']
        #tick.askVolume5 = data['QAskQty5']
        #tick.bidPrice6 = data['QBidPrice6']
        #tick.bidVolume6 = data['QBidQty6']
        #tick.askPrice6 = data['QAskPrice6']
        #tick.askVolume6 = data['QAskQty6']
        #tick.bidPrice7 = data['QBidPrice7']
        #tick.bidVolume7 = data['QBidQty7']
        #tick.askPrice7 = data['QAskPrice7']
        #tick.askVolume7 = data['QAskQty7']
        #tick.bidPrice8 = data['QBidPrice8']
        #tick.bidVolume8 = data['QBidQty8']
        #tick.askPrice8 = data['QAskPrice8']
        #tick.askVolume8 = data['QAskQty8']
        #tick.bidPrice9 = data['QBidPrice9']
        #tick.bidVolume9 = data['QBidQty9']
        #tick.askPrice9 = data['QAskPrice9']
        #tick.askVolume9 = data['QAskQty9']
        #tick.bidPrice10 = data['QBidPrice10']
        #tick.bidVolume10 = data['QBidQty10']
        #tick.askPrice10 = data['QAskPrice10']
        #tick.askVolume10 = data['QAskQty10']


        self.gateway.onTick(tick)
        pass

    #----------------------------------------------------------------------
    def connect(self, authCode, userID, password, address, port):
        """初始化连接"""
        self.userID = userID                # 账号
        self.password = password            # 密码
        self.address = address              # 服务器地址
        self.port = port
        
        # 如果尚未建立服务器连接，则进行连接
        if not self.connectionStatus:
            path = getTempPath(self.gatewayName + '_')
            req = {}
            req["AuthCode"] = authCode
            req["KeyOperationLogPath"] = path
            self.createTapMdApi(req)

            # 注册服务器地址
            self.setHostAddress(self.address, self.port)

            # 初始化连接，成功会调用onFrontConnected
            self.login()
            
        # 若已经连接但尚未登录，则进行登录
        else:
            if not self.loginStatus:
                self.login()
        
    #----------------------------------------------------------------------
    def subscribe(self, subscribeReq):
        """订阅合约"""
        # 这里的设计是，如果尚未登录就调用了订阅方法
        # 则先保存订阅请求，登录完成后会自动订阅
        if self.loginStatus:
            req = {}
            req["ExchangeNo"] = subscribeReq.exchange
            req["CommodityNo"] = subscribeReq.productClass
            req["CommodityType"] = "F"
            req["ContractNo1"] = subscribeReq.symbol
            self.subscribeMarketData(req)
        self.subscribedSymbols.add(subscribeReq)

    #----------------------------------------------------------------------
    def login(self):
        """登录"""
        # 如果填入了用户名密码等，则登录
        if self.userID and self.password:
            req = {}
            req["UserID"] = self.userID
            req["Password"] = self.password
            self.reqUserLogin(req)

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
class TapTdApi(TdApi):
    """CTP交易API实现"""
    
    #----------------------------------------------------------------------
    def __init__(self, gateway):
        """API对象的初始化函数"""
        super(TapTdApi, self).__init__()
        
        self.gateway = gateway                  # gateway对象
        self.gatewayName = gateway.gatewayName  # gateway对象名称
        
        self.orderRef = EMPTY_INT           # 订单编号
        
        self.connectionStatus = False       # 连接状态
        self.loginStatus = False            # 登录状态
        self.authStatus = False             # 验证状态
        self.loginFailed = False            # 登录失败（账号密码错误）
        
        self.userID = EMPTY_STRING          # 账号
        self.password = EMPTY_STRING        # 密码
        self.brokerID = EMPTY_STRING        # 经纪商代码
        self.address = EMPTY_STRING         # 服务器地址
        self.port = EMPTY_INT
        self.accountID = EMPTY_STRING
        

        self.posDict = {}
        self.symbolExchangeDict = {}        # 保存合约代码和交易所的印射关系
        self.symbolSizeDict = {}            # 保存合约代码和合约大小的印射关系

        self.requireAuthentication = False

    #----------------------------------------------------------------------
    def onRspLogin(self, errorCode,  data):
        """登陆回报"""
        # 如果登录成功，推送日志信息
        if errorCode == 0:
            self.loginStatus = True
            self.gateway.tdConnected = True
            self.writeLog(text.TRADING_SERVER_LOGIN)

            # 否则，推送错误信息
        else:
            err = VtErrorData()
            err.gatewayName = self.gatewayName
            err.errorID = errorCode
            err.errorMsg = ""
            self.gateway.onError(err)

            # 标识登录失败，防止用错误信息连续重复登录
            self.loginFailed =  True

    #----------------------------------------------------------------------
    @simple_log
    def onAPIReady(self,):
        self.isApiReady = True
        self.connectionStatus = True
        self.writeLog(text.TRADING_SERVER_CONNECTED)

        ##API准备好后，查询基础数据
        self.qryAccount()
        self.qryCommodity()
        self.qryAccountGateway()
        self.qryPositionGateway()
        pass

    #----------------------------------------------------------------------
    def onDisconnect(self, reasonCode):
        """服务器断开"""
        self.connectionStatus = False
        self.loginStatus = False
        self.gateway.tdConnected = False
        self.writeLog(text.TRADING_SERVER_DISCONNECTED)

    #----------------------------------------------------------------------
    def onRspQryAccount(self, errorCode,  isLast,  data):
        if errorCode == 0:
            self.accountID = data["AccountNo"]
            #self.writeLog(text.ACCOUNT_ID_RECEIVED)
        pass

    #----------------------------------------------------------------------
    def onRspQryCommodity(self, errorCode,  isLast,  data):
        req = {}
        req["ExchangeNo"] = data['ExchangeNo']
        req["CommodityNo"] = data['CommodityNo']
        req["CommodityType"] = data['CommodityType']
        self.qryContract(req)
        pass

    #----------------------------------------------------------------------
    def onRspQryContract(self, errorCode,  isLast,  data):
        contract = VtContractData()
        contract.gatewayName = self.gatewayName

        contract.symbol = data['CommodityNo'] + " " + data['ContractNo1']
        contract.exchange = exchangeMapReverse[data['ExchangeNo']]
        contract.vtSymbol = '.'.join([contract.symbol, contract.exchange])
        #contract.name = data['InstrumentName'].decode('GBK')

        # 合约数值
        #contract.size = data['VolumeMultiple']
        #contract.priceTick = data['PriceTick']
        #contract.strikePrice = data['StrikePrice']
        #contract.underlyingSymbol = data['UnderlyingInstrID']

        contract.productClass = productClassMapReverse.get(data['CommodityType'], PRODUCT_UNKNOWN)

        # 缓存代码和交易所的印射关系
        self.symbolExchangeDict[contract.symbol] = contract.exchange
        #self.symbolSizeDict[contract.symbol] = contract.size

        # 推送
        self.gateway.onContract(contract)

        # 缓存合约代码和交易所映射
        symbolExchangeDict[contract.symbol] = contract.exchange

        #if isLast:
        #    self.writeLog(text.CONTRACT_DATA_RECEIVED)

    #----------------------------------------------------------------------
    def onRspQryFund(self, errorCode,  isLast,  data):
        if errorCode == 0:
            account = VtAccountData()
            account.gatewayName = self.gatewayName

            # 账户代码
            account.accountID = data['AccountNo']
            account.vtAccountID = '.'.join([self.gatewayName, account.accountID])

            # 数值相关
            account.preBalance = data['PreBalance']
            account.available = data['Available']
            account.commission = data['AccountFee']
            account.margin = data['FrozenDeposit']
            account.closeProfit = data['CloseProfit']
        #   account.positionProfit = data['PositionProfit']
            account.balance =  data['Balance']

            # 推送
            self.gateway.onAccount(account)

    #----------------------------------------------------------------------
    def onRtnFund(self, data):
        account = VtAccountData()
        account.gatewayName = self.gatewayName


        # 账户代码
        account.accountID = data['AccountNo']
        account.vtAccountID = '.'.join([self.gatewayName, account.accountID])

        # 数值相关
        account.preBalance = data['PreBalance']
        account.available = data['Available']
        account.commission = data['AccountFee']
        account.margin = data['FrozenDeposit']
        account.closeProfit = data['CloseProfit']
        #account.positionProfit = data['PositionProfit']
        account.balance =  data['Balance']

        # 推送
        self.gateway.onAccount(account)
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryPosition(self, errorCode,  isLast,  data):
        # 获取持仓缓存对象
        print_dict(data)
        posName = '.'.join([data['CommodityNo'], data['ContractNo']])
        if posName in self.posDict:
            pos = self.posDict[posName]
        else:
            pos = VtPositionData()
            self.posDict[posName] = pos

            pos.gatewayName = self.gatewayName
            pos.symbol = data['CommodityNo'] + " " + data['ContractNo']
            pos.vtSymbol = '.'.join([pos.symbol , pos.gatewayName])
            pos.direction = posiDirectionMapReverse.get(data['MatchSide'], '')
            pos.vtPositionName = '.'.join([pos.vtSymbol, pos.direction])

        # 计算成本
        lastPosition = pos.position
        if data["MatchSide"] == directionMap.get(DIRECTION_LONG):
            pos.position = pos.position + data["PositionQty"]               # 持仓量
        else:
            pos.position = pos.position - data["PositionQty"]               # 持仓量
        if pos.position == 0:
            pos.price = 0
        else:
            pos.price = (pos.price * lastPosition + data["PositionPrice"] * data["PositionQty"] ) / (pos.position)                # 持仓均价
        pos.ydPosition = pos.position            # 昨持仓
        pos.positionProfit = pos.positionProfit + data["PositionProfit"]       # 持仓盈亏
        pos.frozen = 0                 # 冻结数量

        # 查询回报结束
        if isLast:
            # 遍历推送
            for pos in self.posDict.values():
                self.gateway.onPosition(pos)

            # 清空缓存
            #self.posDict.clear()

    #----------------------------------------------------------------------
    @simple_log
    def onRtnPosition(self, data):
        # 代码编号相关
        print_dict(data)
        posName = '.'.join([data['CommodityNo'], data['ContractNo']])
        if posName in self.posDict:
            pos = self.posDict[posName]
        else:
            pos = VtPositionData()
            self.posDict[posName] = pos

            pos.gatewayName = self.gatewayName
            pos.symbol = data['CommodityNo'] + " " + data['ContractNo']
            pos.vtSymbol = '.'.join([pos.symbol , pos.gatewayName])
            pos.direction = posiDirectionMapReverse.get(data['MatchSide'], '')
            pos.vtPositionName = '.'.join([pos.vtSymbol, pos.direction])

        # 计算成本
        lastPosition = pos.position
        if data["MatchSide"] == directionMap.get(DIRECTION_LONG):
            pos.position = pos.position + data["PositionQty"]               # 持仓量
        else:
            pos.position = pos.position - data["PositionQty"]               # 持仓量
        if pos.position == 0:
            pos.price = 0
        else:
            pos.price = (pos.price * lastPosition + data["PositionPrice"] * data["PositionQty"] ) / (pos.position)                # 持仓均价
        #pos.positionProfit = 0       # 持仓盈亏
        #pos.frozen = 0                 # 冻结数量

        self.gateway.onPosition(pos)

    #----------------------------------------------------------------------
    @simple_log
    def onRspOrderAction(self, errorCode,  data):
        err = VtErrorData()
        err.gatewayName = self.gatewayName
        err.errorID = errorCode
        err.errorMsg = ""
        self.gateway.onError(err)

    #----------------------------------------------------------------------
    @simple_log
    def onRtnOrder(self, data):
        """报单回报"""
        # 创建报单数据对象
        order = VtOrderData()
        order.gatewayName = self.gatewayName

        # 保存代码和报单号
        order.symbol = data['CommodityNo'] + " " + data['ContractNo']
        order.exchange = exchangeMapReverse[data['ExchangeNo']]
        order.vtSymbol = '.'.join([order.symbol, order.exchange])

        order.orderID = data['OrderNo']
        order.vtOrderID = '.'.join([self.gatewayName, order.orderID])

        order.direction = directionMapReverse.get(data['OrderSide'], DIRECTION_UNKNOWN)
        order.status = statusMapReverse.get(data['OrderState'], STATUS_UNKNOWN)

        # 价格、报单量等数值
        order.price = data['OrderPrice']
        order.totalVolume = data['OrderQty']
        order.tradedVolume = data['OrderMatchQty']
        order.orderTime = data['OrderInsertTime']
        if order.status == STATUS_CANCELLED:
            order.cancelTime = data['OrderUpdateTime']

        # 推送
        self.gateway.onOrder(order)
        pass

    #----------------------------------------------------------------------
    def onRspQryInstrument(self, data, error, n, last):
        """合约查询回报"""
        contract = VtContractData()
        contract.gatewayName = self.gatewayName

        contract.symbol = data['InstrumentID']
        contract.exchange = exchangeMapReverse[data['ExchangeID']]
        contract.vtSymbol = contract.symbol #'.'.join([contract.symbol, contract.exchange])
        contract.name = data['InstrumentName'].decode('GBK')

        # 合约数值
        contract.size = data['VolumeMultiple']
        contract.priceTick = data['PriceTick']
        contract.strikePrice = data['StrikePrice']
        contract.underlyingSymbol = data['UnderlyingInstrID']

        contract.productClass = productClassMapReverse.get(data['ProductClass'], PRODUCT_UNKNOWN)
        
        # 期权类型
        if contract.productClass is PRODUCT_OPTION:
            if data['OptionsType'] == '1':
                contract.optionType = OPTION_CALL
            elif data['OptionsType'] == '2':
                contract.optionType = OPTION_PUT

        # 缓存代码和交易所的印射关系
        self.symbolExchangeDict[contract.symbol] = contract.exchange
        self.symbolSizeDict[contract.symbol] = contract.size

        # 推送
        self.gateway.onContract(contract)
        
        # 缓存合约代码和交易所映射
        symbolExchangeDict[contract.symbol] = contract.exchange

        if last:
            self.writeLog(text.CONTRACT_DATA_RECEIVED)


    #----------------------------------------------------------------------
    @simple_log
    def onRtnFill(self, data):
        """成交回报"""
        # 创建报单数据对象
        trade = VtTradeData()
        trade.gatewayName = self.gatewayName
        
        # 保存代码和报单号
        trade.symbol = data['CommodityNo'] + " " + data['ContractNo']
        trade.exchange = exchangeMapReverse[data['ExchangeNo']]
        trade.vtSymbol = '.'.join([trade.symbol, trade.exchange])
        
        trade.tradeID = data['MatchNo']
        trade.vtTradeID = '.'.join([self.gatewayName, trade.tradeID])
        
        trade.orderID = data['OrderNo']
        trade.vtOrderID = '.'.join([self.gatewayName, trade.orderID])
        
        # 方向
        trade.direction = directionMapReverse.get(data['MatchSide'], '')
            
        # 开平
        #trade.offset = offsetMapReverse.get(data['OffsetFlag'], '')
            
        # 价格、报单量等数值
        trade.price = data['MatchPrice']
        trade.volume = data['MatchQty']
        trade.tradeTime = data['MatchDateTime']
        
        # 推送
        self.gateway.onTrade(trade)

    #----------------------------------------------------------------------
    def connect(self, authCode, userID, password, address, port ):
        """初始化连接"""
        self.userID = userID                # 账号
        self.password = password            # 密码
        self.address = address              # 服务器地址
        self.port = port                    # 服务器端口
        self.authCode = authCode            # 验证码

        # 如果尚未建立服务器连接，则进行连接
        if not self.connectionStatus:
            path = getTempPath(self.gatewayName + '_')
            req = {}
            req["AuthCode"] = authCode
            req["KeyOperationLogPath"] = path
            self.createTapTraderApi(req)

            # 注册服务器地址
            self.setHostAddress(self.address, self.port)

            self.login()


    #----------------------------------------------------------------------
    def login(self):
        """连接服务器"""
        # 如果之前有过登录失败，则不再进行尝试
        if self.loginFailed:
            return

        # 如果填入了用户名密码等，则登录
        if self.userID and self.password:
            req = {}
            req["UserID"] = self.userID
            req["Password"] = self.password
            self.reqUserLogin(req)

    #----------------------------------------------------------------------
    def qryAccountGateway(self):
        """查询账户"""
        req = {}
        req["AccountNo"] = self.accountID
        self.qryFund(req)

    #----------------------------------------------------------------------
    def qryPositionGateway(self):
        """查询持仓"""
        req = {}
        req["AccountNo"] =  self.accountID
        self.qryPosition(req)

    #----------------------------------------------------------------------
    def sendOrder(self, orderReq):
        """发单"""
        req = {}
        req["AccountNo"] = self.accountID					##//< 客户资金帐号，必填
        req["ExchangeNo"] = orderReq.exchange						##//< 交易所编号，必填
        req["CommodityType"] = productClassMap.get(orderReq.productClass,'')	 ##//< 品种类型，必填
        index = orderReq.symbol.index(' ')    ## 以空格为分界线 拆分出产品号与合约号
        req["CommodityNo"] = orderReq.symbol[:index]					##//< 品种编码类型，必填
        req["ContractNo"] = orderReq.symbol[(index+1):]						##//< 合约1，必填
        req["OrderType"] = priceTypeMap.get(orderReq.priceType, '')##//< 委托类型 必填
        req["OrderSide"] = directionMap.get(orderReq.direction, '')##//< 买入卖出
        req["OrderPrice"] = orderReq.price					##//< 委托价格1
        req["OrderQty"] = orderReq.volume						##//< 委托数量，必填
        req["OrderSource"] = "A"
        print_dict(req)
        self.orderRef = self.reqInsertOrder(req)

        # 返回订单号（字符串），便于某些算法进行动态管理
        vtOrderID = '.'.join([self.gatewayName, str(self.orderRef)])
        return vtOrderID
    
    #----------------------------------------------------------------------
    def cancelOrder(self, cancelOrderReq):
        """撤单"""
        req = {}
        req["ServerFlag"] = "A"
        req["OrderNo"] = cancelOrderReq.orderID
        self.reqCancelOrder(req)

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





    
