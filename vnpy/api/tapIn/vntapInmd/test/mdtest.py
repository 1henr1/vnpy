# encoding: UTF-8

import sys
from time import sleep
import time
from PyQt4 import QtGui

from vntapInmd import *



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
    #@simple_log
    def onRspQryCommodity(self,  errorCode,  isLast,  data):
        #print errorCode, isLast
        #print_dict(data)
        if data["Commodity.ExchangeNo"] == "SGE":
            print_dict(data)
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
        #print_dict(data)
        print data["CommodityNo"], data["ContractNo1"]
        print "Basic:"
        print data["QAskPrice1"], data["QAskQty1"]
        print data["QBidPrice1"], data["QBidQty1"]
        print "Implyid"
        print data["QImpliedAskPrice"], data["QImpliedAskQty"]
        print data["QImpliedBidPrice"], data["QImpliedBidQty"]
        print "Stamp:  "
        print data["DateTimeStamp"][:10].replace('-',''),
        print data["DateTimeStamp"][10:]
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
    req["AuthCode"] = "B112F916FE7D27BCE7B97EB620206457946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE9D6F622F72E25D5DEF7F47AA93A738EF5A51B81D8526AB6A9D19E65B41F59D6A946CED32E26C1EACCAF8D4C61E28E2B1ABD9B8F170E14F8847D3EA0BF4E191F5DCB1B791E63DC196D1576DEAF5EC563CA3E560313C0C3411B45076795F550EB050A62C4F74D5892D2D14892E812723FAC858DEBD8D4AF9410729FB849D5D8D6EA48A1B8DC67E037381A279CE9426070929D5DA085659772E24A6F5EA52CF92A4D403F9E46083F27B19A88AD99812DADA44100324759F9FD1964EBD4F2F0FB50B51CD31C0B02BB437"
    req["KeyOperationLogPath"] = ".\\"
    req["DllLocation"] = ".\\TapQuoteAPI.dll"
    api.createTapInMdApi(req)
    
    # 注册前置机地址
    api.setHostAddress("222.88.40.170", 6161 )

    # 登录服务器
    req = {}
    req["UserID"] =  "Q1355739190"
    req["Password"] =  "743295"
    api.reqUserLogin(req)

    while (not api.isApiReady):
        sleep(1)
        pass

    #api.qryCommodity()

    #req = {}
    #req["ExchangeNo"] = "SGE"
    #req["CommodityNo"] = "AG(T+D)"
    #req["CommodityType"] = "Y"
    #api.qryContract(req)

    #req["ExchangeNo"] = "SGE"
    #req["CommodityNo"] = "AG(T+D)"
    #req["CommodityType"] = "Y"
    #req["ContractNo1"] = ""
    #print api.unSubscribeMarketData(req)

    req = {}
    req["ExchangeNo"] = "SGE"
    req["CommodityNo"] = "AG(T+D)"
    req["CommodityType"] = 'Y'
    req["ContractNo1"] = "AG(T+D)"
    print api.subscribeMarketData(req)

    #req = {}
    ##req["ExchangeNo"] = "COMEX"
    ##req["CommodityNo"] = "SI"
    ##req["CommodityType"] = "F"
    ##req["ContractNo1"] = "1805"
    #req["ExchangeNo"] = "COMEX"
    #req["CommodityNo"] = "GC"
    #req["CommodityType"] = "F"
    #req["ContractNo1"] = "1802"
    #print api.subscribeMarketData(req)


    # 连续运行，用于输出行情
    app.exec_()

if __name__ == '__main__':
    main()
