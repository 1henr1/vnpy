# encoding: UTF-8

import sys
from time import sleep
import time
from PyQt4 import QtGui

from vntapmd import *



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
class TestMdApi(MdApi):
    """测试用实例"""

    #----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(TestMdApi, self).__init__()
        self.isApiReady = False
        self.quoteTimes = 0

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
    def onDisconnect(self, reasonCode):
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryCommodity(self,  errorCode,  isLast,  data):
        #print errorCode, isLast
        #print_dict(data)
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspQryContract(self, errorCode,  isLast,  data):
        print_dict(data)
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspSubscribeQuote(self,  errorCode, isLast,  data):
        print_dict(data)
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRspUnSubscribeQuote(self, errorCode,  isLast,  data):
        print_dict(data)
        req = {}
        req["ExchangeNo"] = "COMEX"
        req["CommodityNo"] = "SI"
        req["CommodityType"] = "F"
        req["ContractNo1"] = "1807"
        print self.subscribeMarketData(req)
        pass

    #----------------------------------------------------------------------
    @simple_log
    def onRtnQuote(self,  data):
        ##self.quoteTimes = self.quoteTimes + 1
        ##print self.quoteTimes
        if (self.quoteTimes == 5):
            self.quoteTimes = 0
            req = {}
            req["ExchangeNo"] = "COMEX"
            req["CommodityNo"] = "SI"
            req["CommodityType"] = "F"
            req["ContractNo1"] = "1805"
            print self.unSubscribeMarketData(req)

        #print_dict(data)
        print data["CommodityNo"], data["ContractNo1"]
        print "Basic:"
        print data["QAskPrice1"], data["QAskQty1"]
        print data["QBidPrice1"], data["QBidQty1"]
        print "Implyid"
        print data["QImpliedAskPrice"], data["QImpliedAskQty"]
        print data["QImpliedBidPrice"], data["QImpliedBidQty"]
        pass

#----------------------------------------------------------------------


def main():
    """主测试函数，出现堵塞时可以考虑使用sleep"""

    # 创建Qt应用对象，用于事件循环
    app = QtGui.QApplication(sys.argv)

    # 创建API对象
    api = TestMdApi()
    
    # 在C++环境中创建MdApi对象
    req = {}
    req["AuthCode"] = "67EA896065459BECDFDB924B29CB7DF1946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE41BCBAB68D525B0D111A0884D847D57163FF7F329FA574E7946CED32E26C1EAC946CED32E26C1EAC733827B0CE853869ABD9B8F170E14F8847D3EA0BF4E191F5D97B3DFE4CCB1F01842DD2B3EA2F4B20CAD19B8347719B7E20EA1FA7A3D1BFEFF22290F4B5C43E6C520ED5A40EC1D50ACDF342F46A92CCF87AEE6D73542C42EC17818349C7DEDAB0E4DB16977714F873D505029E27B3D57EB92D5BEDA0A710197EB67F94BB1892B30F58A3F211D9C3B3839BE2D73FD08DD776B9188654853DDA57675EBB7D6FBBFC"
    req["KeyOperationLogPath"] = ".\\"
    api.createTapMdApi(req)
    
    # 注册前置机地址
    api.setHostAddress("123.15.58.21", 7171)

    # 登录服务器
    req = {}
    req["UserID"] =  "ES"
    req["Password"] =  "123456"
    api.reqUserLogin(req)

    while (not api.isApiReady):
        sleep(1)
        pass

    #api.qryCommodity()

    #req = {}
    #req["ExchangeNo"] = "COMEX"
    #req["CommodityNo"] = "SI"
    #req["CommodityType"] = "F"
    #api.qryContract(req)

    req = {}
    #req["ExchangeNo"] = "COMEX"
    #req["CommodityNo"] = "SI"
    #req["CommodityType"] = "F"
    #req["ContractNo1"] = "1805"
    req["ExchangeNo"] = "COMEX"
    req["CommodityNo"] = "GC"
    req["CommodityType"] = "F"
    req["ContractNo1"] = "1801"
    print api.subscribeMarketData(req)


    # 连续运行，用于输出行情
    app.exec_()

if __name__ == '__main__':
    main()
