# encoding: UTF-8

import sys
from time import sleep
import time
from PyQt4 import QtGui

from vnhfpmd import *


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
    
    #----------------------------------------------------------------------
    def onQuotationInfo(self, data):
        if data["contract_id"] == "Ni1710":
            print_dict(data)
            time_struct = time.localtime(data["datetime"])
            print(time_struct)
            print (time.strftime("%H:%M:%S", time_struct))
            print("localtime")
            print(time.time())
            print (time.strftime("%H:%M:%S", time.localtime(time.time())))
        pass


#----------------------------------------------------------------------
def main():
    """主测试函数，出现堵塞时可以考虑使用sleep"""
    reqid = 0
    
    # 创建Qt应用对象，用于事件循环
    app = QtGui.QApplication(sys.argv)

    # 创建API对象
    api = TestMdApi()
    
    # 在C++环境中创建MdApi对象
    api.createHFPMdApi('3A0A64012D1084AF793F1BB1FDE2B4CB','71GQ215YTJFWhw3IKaT2GM0Z0HWK6Wb51mP77r1VRH98Ga6kQ+PQ5He8HNkZYrHINorKHq91VJitAiq+VtnC1qSV')
    
    # 注册前置机地址
    api.connectMdFront("58.215.39.218", "5566")

    # 请求行情档位
    #api.reqQuotationLevel()

    # 连续运行，用于输出行情
    app.exec_()
    
    
    
if __name__ == '__main__':
    main()
