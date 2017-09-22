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
        print IsSuccess, index, code
        pass
    
    @simple_log 
    def onLoginResponse(self, rsp):
        print_dict(rsp)
        pass
    
    @simple_log
    def onLogoutPush(self, outtype):
        print outtype
        pass


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
    i = api.reqUserLogin(loginReq)
    print(i)
    sleep(0.5)
    
    ## 登出
    i = api.reqUserLogout()
    sleep(0.5)
    
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
