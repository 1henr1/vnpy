# encoding: UTF-8

import sys
from time import sleep

from PyQt4 import QtGui

from vnhfptd import *


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
    
    #----------------------------------------------------------------------
    @simple_log    
    def onClientClosed(self, n):
        pass
    
    #----------------------------------------------------------------------
    @simple_log 
    def onClientConnected(self):
        pass
        
    #----------------------------------------------------------------------
    @simple_log    
    def onClientDisConnected(self, client, n):
        pass
    
    #----------------------------------------------------------------------
    @simple_log  
    def onClienthandshaked(self, IsSuccess, index, code):
        pass
    
    @simple_log 
    def onLoginResponse(self, rsp):
        global loginSuccess
        if rsp['success'] == True:
            loginSuccess = 1
        else:
            loginSuccess = 0
            print(rsp['errdesc'])
    
    @simple_log
    def onLogoutPush(self, outtype):
        print outtype
        pass
    
    @simple_log
    def onMarketStatePush(self, MarketState):
        #print_dict(MarketState)
        pass    
    
    @simple_log
    def onMarketResponse(self, MarketResponse):
        #print_dict(MarketResponse)
        pass   
    
    @simple_log
    def onContractResponse(self, ContractResponse):
        #print_dict(ContractResponse)
        pass    
    
    @simple_log
    def onAccountResponse(self, AccountResponse):
        #print_dict(AccountResponse)
        pass   
    
    @simple_log
    def onReceiptcollectResponse(self, ReceiptcollectResponse):
        print_dict(ReceiptcollectResponse)
        pass     
    
    @simple_log
    def onOrderResponse(self, rsp, order):
        print_dict(rsp)
        print_dict(order)
        pass 
    
    
    
#global variables
loginSuccess = 0

#----------------------------------------------------------------------
def main():
    """主测试函数，出现堵塞时可以考虑使用sleep"""
    reqid = 0
    
    # 创建Qt应用对象，用于事件循环
    app = QtGui.QApplication(sys.argv)

    # 创建API对象
    api = TestTdApi()
    
    # 在C++环境中创建TdApi对象
    api.createHFPTdApi('3AA2A71E1A6C69C2822CAB76D87388C0','41GQ225qOZHv8pjBhxYzlh7s++VhvVUw+Fcuu30cqwydEx+nzlldw+hGklZvkE2lNorGH/fvfhjE6HGQi1ta6WeUK5yjK4T28Z3Xjb8L6WW=')
    
    # 注册前置机地址
    api.connectTradeFront("58.215.39.218", 6666)
    sleep(0.5)
    
    # 登陆
    loginReq = {}                           # 创建一个空字典
    loginReq['userID'] = '800261'                 # 参数作为字典键值的方式传入
    loginReq['password'] = '800261'               # 键名和C++中的结构体成员名对应
    api.reqUserLogin(loginReq)
    
    
    while (loginSuccess == 0):
        pass
    
    ## 登出, 会出错， 原因未知
    #i = api.reqUserLogout()
    #sleep(0.5)
    
    """
    ## 获取服务器时间
    time = api.reqServertime()
    print(time)
    print 'Server Time is:' + str(time)
    sleep(0.5)
    """
    
    ## 请求交易所信息  pass
    #seq = api.reqMarket()
    #print seq
    
    ## 请求合约信息 pass
    #seq = api.reqContract()
    #print seq
    
    ## 请求账户信息 pass
    #seq = api.reqAccount()
    #print seq
    
    ## 请求仓单汇总信息 pass
    #seq = api.reqReceiptcollect()
    #print seq
       
    ## 下单
    ##seq = api.Order("001", "Ni1709", "800261", True, 1, 0, True, 8000, 100)
    #print seq
    
    ## 查询订单
    ## api.qryOrder(sequence);
    
    ## 撤单
    
   
    
    # 连续运行，用于输出行情
    app.exec_()
    
    
    
if __name__ == '__main__':
    main()
