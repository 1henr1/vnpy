# encoding: UTF-8

import sys
from time import sleep
import time
from PyQt4 import QtGui

from vntaptd import *



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


########################################################################
class TestTdApi(TdApi):
    """测试用实例"""

    #----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(TestTdApi, self).__init__()
        self.isApiReady = False
        self.quoteTimes = 0

    #----------------------------------------------------------------------
    @simple_log
    def onConnect(self):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspLogin(self,  errorCode,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onAPIReady(self):
        self.isApiReady = True
        pass

   #----------------------------------------------------------------------
    @simple_log
    def onDisconnect(self,  reasonCode):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspChangePassword(self,   errorCode):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspSetReservedInfo(self,   errorCode,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryAccount(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryFund(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnFund(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryExchange(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryCommodity(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryContract(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnContract(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnOrder(self,  errorCode,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspOrderAction(self, errorCode,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryOrder(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryOrderProcess(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryFill(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnFill(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryPosition(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnPosition(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnClose(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnPositionProfit(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryDeepQuote(self,   errorCode,  isLast,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryExchangeStateInfo(self,  errorCode,  isLast, data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnExchangeStateInfo(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnReqQuoteNotice(self,  data):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspUpperChannelInfo(self,  errorCode,  isLast,  data):
        pass



def main():
    """主测试函数，出现堵塞时可以考虑使用sleep"""

    # 创建Qt应用对象，用于事件循环
    app = QtGui.QApplication(sys.argv)

    # 创建API对象
    api = TestTdApi()
    
    # 在C++环境中创建TdApi对象
    req = {}
    req["AuthCode"] = "67EA896065459BECDFDB924B29CB7DF1946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE41BCBAB68D525B0D111A0884D847D57163FF7F329FA574E7946CED32E26C1EAC946CED32E26C1EAC733827B0CE853869ABD9B8F170E14F8847D3EA0BF4E191F5D97B3DFE4CCB1F01842DD2B3EA2F4B20CAD19B8347719B7E20EA1FA7A3D1BFEFF22290F4B5C43E6C520ED5A40EC1D50ACDF342F46A92CCF87AEE6D73542C42EC17818349C7DEDAB0E4DB16977714F873D505029E27B3D57EB92D5BEDA0A710197EB67F94BB1892B30F58A3F211D9C3B3839BE2D73FD08DD776B9188654853DDA57675EBB7D6FBBFC"
    req["KeyOperationLogPath"] = ".\\"
    api.createTapTraderApi(req)
    
    # 注册前置机地址
    api.setHostAddress("123.15.58.21", 7171)

    # 登录服务器
    req = {}
    req["UserID"] =  "Q1355739190"
    req["Password"] =  "027096"
    api.reqUserLogin(req)

    while (not api.isApiReady):
        sleep(1)
        pass

    # api.exit();

    req = {}
    req["AccountNo"] =  "Q1355739190"
    api.qryAccount(req)

    req = {}
    req["AccountNo"] =  "Q1355739190"
    api.qryFund(req)

    api.qryExchange();

    api.qryCommodity()

    req = {}
    req["ExchangeNo"] = "COMEX"
    req["CommodityNo"] = "SI"
    req["CommodityType"] = "F"
    api.qryContract(req)

    req = {}
    req["AccountNo"] = ""
    req["ExchangeNo"] = ""
    req["CommodityType"] = ""
    req["CommodityNo"] = ""
    req["ContractNo"] = ""
    req["OrderType"] = ""
    req["OrderSource"] = ""
    req["TimeInForce"] = ""
    req["OrderSide"] = ""
    req["OrderPrice"] = ""
    req["OrderQty"] = ""
    api.reqInsertOrder( req)
	

    req = {}
    req["OrderNo"] = ""
    api.reqCancelOrder( req)

    req = {}
    req["QryOrderType"] = ""
    api.qryOrder( req)

    api.qryFill()

    api.qryPosition()


    # 连续运行，用于输出行情
    app.exec_()

if __name__ == '__main__':
    main()
