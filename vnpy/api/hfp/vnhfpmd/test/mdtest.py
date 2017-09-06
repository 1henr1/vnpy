# encoding: UTF-8

import sys
from time import sleep

from PyQt4 import QtGui

from vnhfpmd import *


#----------------------------------------------------------------------
def print_dict(d):
    """按照键值打印一个字典"""
    for key,value in d.items():
        print key + ':' + str(value)
        
        
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
    
    #----------------------------------------------------------------------
    @simple_log    
    def onClientClosed(self, client, n):
        print("Client Closed")
        pass
    
    #----------------------------------------------------------------------
    @simple_log    
    def onClientConnected(self, client):
        print("Client Connected")
        pass
        
    #----------------------------------------------------------------------
    @simple_log    
    def onClientDisConnected(self, client, n):
        print("Client DisConnected")
        pass
    
    #----------------------------------------------------------------------
    @simple_log    
    def onClienthandshaked(self, client, IsSuccess, index, code):
        print("Client HandSharked")
        pass
    
    @simple_log 
    #----------------------------------------------------------------------
    def onQuotationInfo(self, client, data):
        print("Receiving Quotation")
        print_dict(data)


#----------------------------------------------------------------------
def main():
    """主测试函数，出现堵塞时可以考虑使用sleep"""
    reqid = 0
    
    # 创建Qt应用对象，用于事件循环
    app = QtGui.QApplication(sys.argv)

    # 创建API对象
    api = TestMdApi()
    
    # 在C++环境中创建MdApi对象，传入参数是希望用来保存.con文件的地址
    api.createHFPMdApi()
    
    # 注册前置机地址
    #api.registerFront("tcp://qqfz-md1.ctp.shcifco.com:32313")
    
    # 初始化api，连接前置机
    api.init()
    sleep(0.5)
    
    # 登陆
    #loginReq = {}                           # 创建一个空字典
    #loginReq['UserID'] = ''                 # 参数作为字典键值的方式传入
    #loginReq['Password'] = ''               # 键名和C++中的结构体成员名对应
    #loginReq['BrokerID'] = ''    
    #reqid = reqid + 1                       # 请求数必须保持唯一性
    #i = api.reqUserLogin(loginReq, 1)
    #sleep(0.5)
    
    ## 登出，测试出错（无此功能）
    #reqid = reqid + 1
    #i = api.reqUserLogout({}, 1)
    #sleep(0.5)
    
    ## 安全退出，测试通过
    #i = api.exit()
    
    ## 获取交易日，目前输出为空
    #day = api.getTradingDay()
    #print 'Trading Day is:' + str(day)
    #sleep(0.5)
    
    ## 订阅合约，测试通过
    #i = api.subscribeMarketData('IF1505')
    
    ## 退订合约，测试通过
    #i = api.unSubscribeMarketData('IF1505')
    
    # 订阅询价，测试通过
    #i = api.subscribeForQuoteRsp('IO1504-C-3900')
    
    # 退订询价，测试通过
    #i = api.unSubscribeForQuoteRsp('IO1504-C-3900')
    
    # 连续运行，用于输出行情
    app.exec_()
    
    
    
if __name__ == '__main__':
    main()
