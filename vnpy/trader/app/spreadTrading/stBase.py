# encoding: UTF-8

from __future__ import division

from math import floor
from datetime import datetime
from vnpy.trader.vtObject import *

from vnpy.trader.vtConstant import (EMPTY_INT, EMPTY_FLOAT, 
                                    EMPTY_STRING, EMPTY_UNICODE)



EVENT_SPREADTRADING_TICK = 'eSpreadTradingTick.'
EVENT_SPREAD_TICK = 'eSpreadTick.'
EVENT_SPREADTRADING_POS = 'eSpreadTradingPos.'
EVENT_SPREADTRADING_LOG = 'eSpreadTradingLog'
EVENT_SPREADTRADING_ALGO = 'eSpreadTradingAlgo.'
EVENT_SPREADTRADING_ALGOLOG = 'eSpreadTradingAlgoLog'



########################################################################
class StLeg(object):
    """"""

    #----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        self.vtSymbol = EMPTY_STRING      # 代码
        self.ratio = EMPTY_INT          # 实际交易时的比例
        self.multiplier = EMPTY_FLOAT   # 计算价差时的乘数
        self.payup = EMPTY_INT          # 对冲时的超价tick
        
        self.bidPrice = EMPTY_FLOAT
        self.askPrice = EMPTY_FLOAT
        self.bidVolume = EMPTY_FLOAT
        self.askVolume = EMPTY_FLOAT
        self.lastPrice = EMPTY_FLOAT
        self.lastVolume = EMPTY_FLOAT

        self.longPos = EMPTY_FLOAT
        self.shortPos = EMPTY_FLOAT
        self.netPos = EMPTY_FLOAT


########################################################################
class StSpread(object):
    """"""

    #----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        self.name = EMPTY_UNICODE       # 名称
        self.symbol = EMPTY_STRING      # 代码（基于组成腿计算）
        self.vtSymbol = EMPTY_STRING    # name.spread

        self.activeLeg = None           # 主动腿
        self.passiveLegs = []           # 被动腿（支持多条）
        self.allLegs = []               # 所有腿
        self.floorDigits = 1000
        
        self.bidPrice = EMPTY_FLOAT
        self.askPrice = EMPTY_FLOAT
        self.bidVolume = EMPTY_INT
        self.askVolume = EMPTY_INT
        self.lastPrice = EMPTY_FLOAT
        self.lastVolume = EMPTY_FLOAT
        self.time = EMPTY_STRING
        self.date = EMPTY_STRING                # 日期 20151009
        self.datetime = None                    # python的datetime时间对象

        self.longPos = EMPTY_INT
        self.shortPos = EMPTY_INT
        self.netPos = EMPTY_INT

        self.tradingPeriod = []         # 可交易时间段(支持多段)

    #----------------------------------------------------------------------
    def initSpread(self):
        """初始化价差"""
        # 价差最少要有一条主动腿
        if not self.activeLeg:
            return
        
        # 生成所有腿列表
        self.allLegs.append(self.activeLeg)
        self.allLegs.extend(self.passiveLegs)
        
        # 生成价差代码
        legSymbolList = []
        
        for leg in self.allLegs:
            if leg.multiplier >= 0:
                legSymbol = '+%s*%s' %(leg.multiplier, leg.vtSymbol)
            else:
                legSymbol = '%s*%s' %(leg.multiplier, leg.vtSymbol)
            legSymbolList.append(legSymbol)
        
        #self.symbol = ''.join(legSymbolList)
        self.symbol = self.name
        self.vtSymbol = '.'.join([self.name, 'spread'])    # 合约在vt系统中的唯一代码，通常是 合约代码.交易所代码

    #----------------------------------------------------------------------
    def calculateTime(self, tick):
        """计算时间"""
        self.date = datetime.now().strftime('%Y-%m-%d')
        self.time = datetime.now().strftime('%H:%M:%S.%f')
        self.datetime = datetime.now()

    #----------------------------------------------------------------------
    def calculatePrice(self):
        """计算价格"""
        # 清空价格和委托量数据
        self.bidPrice = EMPTY_FLOAT
        self.askPrice = EMPTY_FLOAT
        self.askVolume = EMPTY_FLOAT
        self.bidVolume = EMPTY_FLOAT
        self.lastPrice = EMPTY_FLOAT
        self.lastVolume = EMPTY_FLOAT

        # 遍历价差腿列表
        for n, leg in enumerate(self.allLegs):
            # 过滤有某条腿尚未初始化的情况（无挂单量）
            if not leg.bidVolume or not leg.askVolume:
                self.bidPrice = EMPTY_FLOAT
                self.askPrice = EMPTY_FLOAT
                self.askVolume = EMPTY_INT
                self.bidVolume = EMPTY_INT
                self.lastPrice = EMPTY_FLOAT
                self.lastVolume = EMPTY_FLOAT
                return
                
            # 计算价格
            if leg.multiplier > 0:
                self.bidPrice += leg.bidPrice * leg.multiplier
                self.askPrice += leg.askPrice * leg.multiplier
            else:
                self.bidPrice += leg.askPrice * leg.multiplier
                self.askPrice += leg.bidPrice * leg.multiplier

            self.lastPrice += leg.lastPrice * leg.multiplier

            # 计算报单量
            if leg.ratio > 0:
                legAdjustedBidVolume = floor(self.floorDigits * leg.bidVolume / leg.ratio) / self.floorDigits
                legAdjustedAskVolume = floor(self.floorDigits * leg.askVolume / leg.ratio) / self.floorDigits
            else:
                legAdjustedBidVolume = floor(self.floorDigits * leg.askVolume / abs(leg.ratio)) / self.floorDigits
                legAdjustedAskVolume = floor(self.floorDigits * leg.bidVolume / abs(leg.ratio)) / self.floorDigits

            legAdjustedlastVolume = floor(self.floorDigits * leg.lastVolume / abs(leg.ratio)) / self.floorDigits

            if n == 0:
                self.bidVolume = legAdjustedBidVolume                           # 对于第一条腿，直接初始化
                self.askVolume = legAdjustedAskVolume
                self.lastVolume = legAdjustedlastVolume
            else:
                self.bidVolume = min(self.bidVolume, legAdjustedBidVolume)      # 对于后续的腿，价差可交易报单量取较小值
                self.askVolume = min(self.askVolume, legAdjustedAskVolume)
                self.lastVolume = min(self.lastVolume, legAdjustedlastVolume)

        # 更新时间
        # self.time = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        
    #----------------------------------------------------------------------
    def calculatePos(self):
        """计算持仓"""
        # 清空持仓数据
        self.longPos = EMPTY_INT
        self.shortPos = EMPTY_INT
        self.netPos = EMPTY_INT
        
        # 遍历价差腿列表
        for n, leg in enumerate(self.allLegs):
            if leg.ratio > 0:
                legAdjustedLongPos = floor(leg.longPos / leg.ratio)
                legAdjustedShortPos = floor(leg.shortPos / leg.ratio)
            else:
                legAdjustedLongPos = floor(leg.shortPos / abs(leg.ratio))
                legAdjustedShortPos = floor(leg.longPos / abs(leg.ratio))
            
            if n == 0:
                self.longPos = legAdjustedLongPos
                self.shortPos = legAdjustedShortPos
            else:
                self.longPos = min(self.longPos, legAdjustedLongPos)
                self.shortPos = min(self.shortPos, legAdjustedShortPos)
        
        # 计算净仓位
        self.longPos = int(self.longPos)
        self.shortPos = int(self.shortPos)
        self.netPos = self.longPos - self.shortPos
    
    #----------------------------------------------------------------------
    def addActiveLeg(self, leg):
        """添加主动腿"""
        self.activeLeg = leg
    
    #----------------------------------------------------------------------
    def addPassiveLeg(self, leg):
        """添加被动腿"""
        self.passiveLegs.append(leg)

    #----------------------------------------------------------------------
    def addTradingPeriod(self, tradingPeriod):
        """添加被动腿"""
        self.tradingPeriod = tradingPeriod

    #----------------------------------------------------------------------
    def isTradingPeriod(self):
        """判断是否在交易时间内"""
        for period in self.tradingPeriod:
            startTime = period[0]
            endTime = period[1]
            currTime = self.time[:8]
            if currTime >= startTime and currTime <= endTime:
                return True
        return False

    #----------------------------------------------------------------------
    def convert2tick(self):
        tick = VtTickData()
        # 代码相关
        tick.symbol = self.symbol # 合约代码
        tick.vtSymbol = self.vtSymbol # name.spread

        # 成交数据
        tick.lastPrice = self.lastPrice # 最新成交价
        tick.lastVolume = self.lastVolume # 最新成交量

        tick.time = self.time # 时间 11:20:56.5
        tick.date = self.date # 日期 20151009
        tick.datetime = self.datetime                    # python的datetime时间对象

        # 五档行情
        tick.bidPrice1 = self.bidPrice
        #tick.bidPrice2 = self.bidPrice2
        #tick.bidPrice3 = self.bidPrice3
        #tick.bidPrice4 = self.bidPrice4
        #tick.bidPrice5 = self.bidPrice5

        tick.askPrice1 = self.askPrice
        #tick.askPrice2 = self.askPrice2
        #tick.askPrice3 = self.askPrice3
        #tick.askPrice4 = self.askPrice4
        #tick.askPrice5 = self.askPrice5

        tick.bidVolume1 = self.bidVolume
        #tick.bidVolume2 = self.bidVolume2
        #tick.bidVolume3 = self.bidVolume3
        #tick.bidVolume4 = self.bidVolume4
        #tick.bidVolume5 = self.bidVolume5

        tick.askVolume1 = self.askVolume
        #tick.askVolume2 = self.askVolume2
        #tick.askVolume3 = self.askVolume3
        #tick.askVolume4 = self.askVolume4
        #tick.askVolume5 = self.askVolume5

        return tick

        
    
    