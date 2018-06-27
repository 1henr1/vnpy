// vntapInmd.cpp : 定义 DLL 应用程序的导出函数。
//

#include "TapQuoteAPI.h"
#include "TapAPIError.h"
#include "vntapInmd.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

#ifdef _DEBUG
FILE *fp = NULL;
char filename[] = "tapIn_md_debug.txt";
#endif


#define TOSTRING(pChar) (pChar == NULL) ? "" : string(pChar)


///-------------------------------------------------------------------------------------
///从Python对象到C++类型转换用的函数
///-------------------------------------------------------------------------------------

void getInt(dict d, string key, int *value)
{
	if (d.has_key(key))		//检查字典中是否存在该键值
	{
		object o = d[key];	//获取该键值
		extract<int> x(o);	//创建提取器
		if (x.check())		//如果可以提取
		{
			*value = x();	//对目标整数指针赋值
		}
	}
};

void getDouble(dict d, string key, double *value)
{
	if (d.has_key(key))
	{
		object o = d[key];
		extract<double> x(o);
		if (x.check())
		{
			*value = x();
		}
	}
};

void getStr(dict d, string key, char *value)
{
	if (d.has_key(key))
	{
		object o = d[key];
		extract<string> x(o);
		if (x.check())
		{
			string s = x();
			const char *buffer = s.c_str();
			//对字符串指针赋值必须使用strcpy_s, vs2013使用strcpy编译通不过
			//+1应该是因为C++字符串的结尾符号？不是特别确定，不加这个1会出错
#ifdef _MSC_VER //WIN32
			strcpy_s(value, strlen(buffer) + 1, buffer);
#elif __GNUC__
			strncpy(value, buffer, strlen(buffer) + 1);
#endif
		}
	}
};

void getChar(dict d, string key, char *value)
{
	if (d.has_key(key))
	{
		object o = d[key];
		extract<string> x(o);
		if (x.check())
		{
			string s = x();
			const char *buffer = s.c_str();
			*value = *buffer;
		}
	}
};


///-------------------------------------------------------------------------------------
///C++的回调函数将数据保存到队列中
///-------------------------------------------------------------------------------------
void TAP_CDECL MdApi::OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info)
{
	Task task = Task();
	task.task_name = ONRSPUSERLOGIN;
	task.task_error = errorCode;
	if (info) {
		task.task_data = *info;
	}
	else {
		TapAPIQuotLoginRspInfo emptyData;
		task.task_data = emptyData;
	}
	this->task_queue.push(task);
}

void TAP_CDECL MdApi::OnAPIReady()
{
	Task task = Task();
	task.task_name = ONAPIREADY;
	this->task_queue.push(task);
}

void TAP_CDECL MdApi::OnDisconnect(TAPIINT32 reasonCode)
{
	Task task = Task();
	task.task_name = ONDISCONNECTED;
	task.task_error = reasonCode;
	this->task_queue.push(task);
}

void TAP_CDECL MdApi::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info)
{
	Task task = Task();
	task.task_name = ONRSPQRYCOMMODITY;
	task.task_error = errorCode;
	task.task_last = isLast;

	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIQuoteCommodityInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}

void TAP_CDECL MdApi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info)
{
	Task task = Task();
	task.task_name = ONRSPQRYCONTRACT;
	task.task_error = errorCode;
	task.task_last = isLast;

	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIQuoteContractInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);

}

void TAP_CDECL MdApi::OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info)
{
	Task task = Task();
	task.task_name = ONRSPSUBQUOTE;
	task.task_error = errorCode;
	task.task_last = isLast;

	if (info) {
		task.task_data = *info;
	}
	else {
		TapAPIQuoteWhole emptyData;
		task.task_data = emptyData;
	}
	task.task_last = isLast;
	this->task_queue.push(task);
}

void TAP_CDECL MdApi::OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info)
{
	Task task = Task();
	task.task_name = ONRSPUNSUBQUOTE;
	task.task_error = errorCode;
	task.task_last = isLast;

	if (info) {
		task.task_data = *info;
	}
	else {
		TapAPIContract emptyData;
		task.task_data = emptyData;
	}
	task.task_last = isLast;
	this->task_queue.push(task);
}

void TAP_CDECL MdApi::OnRtnQuote(const TapAPIQuoteWhole *info)
{
	Task task = Task();
	task.task_name = ONRTNQUOTE;

	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIQuoteWhole empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}




///-------------------------------------------------------------------------------------
///工作线程从队列中取出数据，转化为python对象后，进行推送
///-------------------------------------------------------------------------------------

void MdApi::processTask()
{
	while (1)
	{
		Task task = this->task_queue.wait_and_pop();

		switch (task.task_name)
		{
		case ONRSPUSERLOGIN:
		{
			this->processRspLogin(task);
			break;
		}
		case ONAPIREADY:
		{
			this->processAPIReady(task);
			break;
		}
		case ONDISCONNECTED:
		{
			this->processDisconnect(task);
			break;
		}
		case ONRSPQRYCOMMODITY:
		{
			this->processRspQryCommodity(task);
			break;
		}
		case ONRSPQRYCONTRACT:
		{
			this->processRspQryContract(task);
			break;
		}
		case ONRSPSUBQUOTE:
		{
			this->processRspSubscribeQuote(task);
			break;
		}
		case ONRSPUNSUBQUOTE:
		{
			this->processRspUnSubscribeQuote(task);
			break;
		}
		case ONRTNQUOTE:
		{
			this->processRtnQuote(task);
			break;
		}
		};
	}
};

void MdApi::processRspLogin(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	int errorCode = any_cast<int>(task.task_error);

	TapAPIQuotLoginRspInfo task_data = any_cast<TapAPIQuotLoginRspInfo>(task.task_data);
	dict data;
	data["UserNo"] = TOSTRING(task_data.UserNo);							///< 用户名
	data["UserType"] = task_data.UserType;						///< 用户类型
	data["UserName"] = TOSTRING(task_data.UserName);						///< 昵称，GBK编码格式
	data["QuoteTempPassword"] = TOSTRING(task_data.QuoteTempPassword);				///< 行情临时密码
	data["ReservedInfo"] = TOSTRING(task_data.ReservedInfo);					///< 用户自己设置的预留信息
	data["LastLoginIP"] = TOSTRING(task_data.LastLoginIP);					///< 上次登录的地址
	data["LastLoginProt"] = task_data.LastLoginProt;					///< 上次登录使用的端口
	data["LastLoginTime"] = TOSTRING(task_data.LastLoginTime);					///< 上次登录的时间
	data["LastLogoutTime"] = TOSTRING(task_data.LastLogoutTime);					///< 上次退出的时间
	data["TradeDate"] = TOSTRING(task_data.TradeDate);						///< 当前交易日期
	data["LastSettleTime"] = TOSTRING(task_data.LastSettleTime);					///< 上次结算时间
	data["StartTime"] = TOSTRING(task_data.StartTime);						///< 系统启动时间
	data["InitTime"] = TOSTRING(task_data.InitTime);						///< 系统初始化时间
	
	this->onRspLogin(errorCode, data);
}
void MdApi::processAPIReady(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onAPIReady();
}
void MdApi::processDisconnect(Task task)
{
	PyLock lock;
	int reasonCode = any_cast<int>(task.task_error);
	this->onDisconnect(reasonCode);
}
void MdApi::processRspQryCommodity(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	int errorCode = any_cast<int>(task.task_error);
	bool isLast = task.task_last;
	TapAPIQuoteCommodityInfo task_data = any_cast<TapAPIQuoteCommodityInfo>(task.task_data);
	dict data;
	data["Commodity.ExchangeNo"] = task_data.Commodity.ExchangeNo;                             ///< 交易所编码
	data["Commodity.CommodityType"] = task_data.Commodity.CommodityType;                          ///< 品种类型
	data["Commodity.CommodityNo"] = task_data.Commodity.CommodityNo;
	data["CommodityName"] = task_data.CommodityName;						///< 品种名称,GBK编码格式
	data["CommodityEngName"] = task_data.CommodityEngName;					///< 品种英文名称
	data["ContractSize"] = task_data.ContractSize;						///< 每手乘数
	data["CommodityTickSize"] = task_data.CommodityTickSize;					///< 最小变动价位
	data["CommodityDenominator"] = task_data.CommodityDenominator;				///< 报价分母
	data["CmbDirect"] = task_data.CmbDirect;							///< 组合方向
	data["CommodityContractLen"] = task_data.CommodityContractLen;				///< 品种合约年限
	data["IsDST"] = task_data.IsDST;								///< 是否夏令时,'Y'为是,'N'为否
	//data["RelateCommodity1"] = task_data.RelateCommodity1;					///< 关联品种1
	//data["RelateCommodity2"] = task_data.RelateCommodity2;					///< 关联品种2

	this->onRspQryCommodity(errorCode, isLast, data);
}
void MdApi::processRspQryContract(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	int errorCode = any_cast<int>(task.task_error);
	bool isLast = task.task_last;
	TapAPIQuoteContractInfo task_data = any_cast<TapAPIQuoteContractInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.Contract.Commodity.ExchangeNo;                             ///< 交易所编码
	data["CommodityNo"] = task_data.Contract.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.Contract.ContractNo1;                            ///< 合约代码1
	data["StrikePrice1"] = task_data.Contract.StrikePrice1;                           ///< 执行价1
	data["CallOrPutFlag1"] = task_data.Contract.CallOrPutFlag1;                         ///< 看涨看跌标示1
	data["ContractNo2"] = task_data.Contract.ContractNo2;                            ///< 合约代码2
	data["StrikePrice2"] = task_data.Contract.StrikePrice2;                           ///< 执行价2
	data["CallOrPutFlag2"] = task_data.Contract.CallOrPutFlag2;
	data["ContractType"] = task_data.ContractType;                       ///< 合约类型,'1'表示交易行情合约,'2'表示行情合约
	data["QuoteUnderlyingContract"] = task_data.QuoteUnderlyingContract;			///< 行情真实合约
	data["ContractName"] = task_data.ContractName;                       ///< 合约名称
	data["ContractExpDate"] = task_data.ContractExpDate;                    ///< 合约到期日	
	data["LastTradeDate"] = task_data.LastTradeDate;                      ///< 最后交易日
	data["FirstNoticeDate"] = task_data.FirstNoticeDate;                    ///< 首次通知日
	this->onRspQryContract(errorCode, isLast, data);
}
void MdApi::processRspSubscribeQuote(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	int errorCode = any_cast<int>(task.task_error);
	bool isLast = task.task_last;
	TapAPIQuoteWhole task_data = any_cast<TapAPIQuoteWhole>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.Contract.Commodity.ExchangeNo;                             ///< 交易所编码
	data["CommodityNo"] = task_data.Contract.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.Contract.ContractNo1;                            ///< 合约代码1
	data["StrikePrice1"] = task_data.Contract.StrikePrice1;                           ///< 执行价1
	data["CallOrPutFlag1"] = task_data.Contract.CallOrPutFlag1;                         ///< 看涨看跌标示1
	data["ContractNo2"] = task_data.Contract.ContractNo2;                            ///< 合约代码2
	data["StrikePrice2"] = task_data.Contract.StrikePrice2;                           ///< 执行价2
	data["CallOrPutFlag2"] = task_data.Contract.CallOrPutFlag2;
	data["CurrencyNo"] = task_data.CurrencyNo;						///< 币种编号
	data["TradingState"] = task_data.TradingState;					///< 交易状态。1,集合竞价;2,集合竞价撮合;3,连续交易;4,交易暂停;5,闭市
	data["DateTimeStamp"] = task_data.DateTimeStamp;					///< 时间戳
	data["QPreClosingPrice"] = task_data.QPreClosingPrice;				///< 昨收盘价
	data["QPreSettlePrice"] = task_data.QPreSettlePrice;				///< 昨结算价
	data["QPrePositionQty"] = task_data.QPrePositionQty;				///< 昨持仓量
	data["QOpeningPrice"] = task_data.QOpeningPrice;					///< 开盘价
	data["QLastPrice"] = task_data.QLastPrice;						///< 最新价
	data["QHighPrice"] = task_data.QHighPrice;						///< 最高价
	data["QLowPrice"] = task_data.QLowPrice;						///< 最低价
	data["QHisHighPrice"] = task_data.QHisHighPrice;					///< 历史最高价
	data["QHisLowPrice"] = task_data.QHisLowPrice;					///< 历史最低价
	data["QLimitUpPrice"] = task_data.QLimitUpPrice;					///< 涨停价
	data["QLimitDownPrice"] = task_data.QLimitDownPrice;				///< 跌停价
	data["QTotalQty"] = task_data.QTotalQty;						///< 当日总成交量
	data["QTotalTurnover"] = task_data.QTotalTurnover;					///< 当日成交金额
	data["QPositionQty"] = task_data.QPositionQty;					///< 持仓量
	data["QAveragePrice"] = task_data.QAveragePrice;					///< 均价
	data["QClosingPrice"] = task_data.QClosingPrice;					///< 收盘价
	data["QSettlePrice"] = task_data.QSettlePrice;					///< 结算价
	data["QLastQty"] = task_data.QLastQty;						///< 最新成交量
	data["QBidPrice1"] = task_data.QBidPrice[0];					///< 买价1-20档
	data["QBidPrice2"] = task_data.QBidPrice[1];					///< 买价1-20档
	data["QBidPrice3"] = task_data.QBidPrice[2];					///< 买价1-20档
	data["QBidPrice4"] = task_data.QBidPrice[3];					///< 买价1-20档
	data["QBidPrice5"] = task_data.QBidPrice[4];					///< 买价1-20档
	data["QBidPrice6"] = task_data.QBidPrice[5];					///< 买价1-20档
	data["QBidPrice7"] = task_data.QBidPrice[6];					///< 买价1-20档
	data["QBidPrice8"] = task_data.QBidPrice[7];					///< 买价1-20档
	data["QBidPrice9"] = task_data.QBidPrice[8];					///< 买价1-20档
	data["QBidPrice10"] = task_data.QBidPrice[9];					///< 买价1-20档
	data["QBidQty1"] = task_data.QBidQty[0];					///< 买价1-20档
	data["QBidQty2"] = task_data.QBidQty[1];					///< 买价1-20档
	data["QBidQty3"] = task_data.QBidQty[2];					///< 买价1-20档
	data["QBidQty4"] = task_data.QBidQty[3];					///< 买价1-20档
	data["QBidQty5"] = task_data.QBidQty[4];					///< 买价1-20档
	data["QBidQty6"] = task_data.QBidQty[5];					///< 买价1-20档
	data["QBidQty7"] = task_data.QBidQty[6];					///< 买价1-20档
	data["QBidQty8"] = task_data.QBidQty[7];					///< 买价1-20档
	data["QBidQty9"] = task_data.QBidQty[8];					///< 买价1-20档
	data["QBidQty10"] = task_data.QBidQty[9];					///< 买价1-20档
	data["QAskPrice1"] = task_data.QAskPrice[0];					///< 买价1-20档
	data["QAskPrice2"] = task_data.QAskPrice[1];					///< 买价1-20档
	data["QAskPrice3"] = task_data.QAskPrice[2];					///< 买价1-20档
	data["QAskPrice4"] = task_data.QAskPrice[3];					///< 买价1-20档
	data["QAskPrice5"] = task_data.QAskPrice[4];					///< 买价1-20档
	data["QAskPrice6"] = task_data.QAskPrice[5];					///< 买价1-20档
	data["QAskPrice7"] = task_data.QAskPrice[6];					///< 买价1-20档
	data["QAskPrice8"] = task_data.QAskPrice[7];					///< 买价1-20档
	data["QAskPrice9"] = task_data.QAskPrice[8];					///< 买价1-20档
	data["QAskPrice10"] = task_data.QAskPrice[9];					///< 买价1-20档
	data["QAskQty1"] = task_data.QAskQty[0];					///< 买价1-20档
	data["QAskQty2"] = task_data.QAskQty[1];					///< 买价1-20档
	data["QAskQty3"] = task_data.QAskQty[2];					///< 买价1-20档
	data["QAskQty4"] = task_data.QAskQty[3];					///< 买价1-20档
	data["QAskQty5"] = task_data.QAskQty[4];					///< 买价1-20档
	data["QAskQty6"] = task_data.QAskQty[5];					///< 买价1-20档
	data["QAskQty7"] = task_data.QAskQty[6];					///< 买价1-20档
	data["QAskQty8"] = task_data.QAskQty[7];					///< 买价1-20档
	data["QAskQty9"] = task_data.QAskQty[8];					///< 买价1-20档
	data["QAskQty10"] = task_data.QAskQty[9];					///< 买价1-20档
	data["QImpliedBidPrice"] = task_data.QImpliedBidPrice;				///< 隐含买价
	data["QImpliedBidQty"] = task_data.QImpliedBidQty;					///< 隐含买量
	data["QImpliedAskPrice"] = task_data.QImpliedAskPrice;				///< 隐含卖价
	data["QImpliedAskQty"] = task_data.QImpliedAskQty;					///< 隐含卖量
	data["QPreDelta"] = task_data.QPreDelta;						///< 昨虚实度
	data["QCurrDelta"] = task_data.QCurrDelta;						///< 今虚实度
	data["QInsideQty"] = task_data.QInsideQty;						///< 内盘量
	data["QOutsideQty"] = task_data.QOutsideQty;					///< 外盘量
	data["QTurnoverRate"] = task_data.QTurnoverRate;					///< 换手率
	data["Q5DAvgQty"] = task_data.Q5DAvgQty;						///< 五日均量
	data["QPERatio"] = task_data.QPERatio;						///< 市盈率
	data["QTotalValue"] = task_data.QTotalValue;					///< 总市值
	data["QNegotiableValue"] = task_data.QNegotiableValue;				///< 流通市值
	data["QPositionTrend"] = task_data.QPositionTrend;					///< 持仓走势
	data["QChangeSpeed"] = task_data.QChangeSpeed;					///< 涨速
	data["QChangeRate"] = task_data.QChangeRate;					///< 涨幅
	data["QChangeValue"] = task_data.QChangeValue;					///< 涨跌值
	data["QSwing"] = task_data.QSwing;							///< 振幅
	data["QTotalBidQty"] = task_data.QTotalBidQty;					///< 委买总量
	data["QTotalAskQty"] = task_data.QTotalAskQty;					///< 委卖总量

	this->onRspSubscribeQuote(errorCode, isLast, data);
}
void MdApi::processRspUnSubscribeQuote(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	int errorCode = any_cast<int>(task.task_error);
	bool isLast = task.task_last;
	TapAPIContract task_data = any_cast<TapAPIContract>(task.task_data);
	dict data;
	data["Commodity.ExchangeNo"] = task_data.Commodity.ExchangeNo;                             ///< 交易所编码
	data["Commodity.CommodityType"] = task_data.Commodity.CommodityType;                          ///< 品种类型
	data["Commodity.CommodityNo"] = task_data.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.ContractNo1;                            ///< 合约代码1
	data["StrikePrice1"] = task_data.StrikePrice1;                           ///< 执行价1
	data["CallOrPutFlag1"] = task_data.CallOrPutFlag1;                         ///< 看涨看跌标示1
	data["ContractNo2"] = task_data.ContractNo2;                            ///< 合约代码2
	data["StrikePrice2"] = task_data.StrikePrice2;                           ///< 执行价2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;                         ///< 看涨看跌标示2
	this->onRspUnSubscribeQuote(errorCode, isLast, data);
}
void MdApi::processRtnQuote(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIQuoteWhole task_data = any_cast<TapAPIQuoteWhole>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.Contract.Commodity.ExchangeNo;                             ///< 交易所编码
	data["CommodityNo"] = task_data.Contract.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.Contract.ContractNo1;                            ///< 合约代码1
	data["StrikePrice1"] = task_data.Contract.StrikePrice1;                           ///< 执行价1
	data["CallOrPutFlag1"] = task_data.Contract.CallOrPutFlag1;                         ///< 看涨看跌标示1
	data["ContractNo2"] = task_data.Contract.ContractNo2;                            ///< 合约代码2
	data["StrikePrice2"] = task_data.Contract.StrikePrice2;                           ///< 执行价2
	data["CallOrPutFlag2"] = task_data.Contract.CallOrPutFlag2;
	data["CurrencyNo"] = task_data.CurrencyNo;						///< 币种编号
	data["TradingState"] = task_data.TradingState;					///< 交易状态。1,集合竞价;2,集合竞价撮合;3,连续交易;4,交易暂停;5,闭市
	data["DateTimeStamp"] = task_data.DateTimeStamp;					///< 时间戳
	data["QPreClosingPrice"] = task_data.QPreClosingPrice;				///< 昨收盘价
	data["QPreSettlePrice"] = task_data.QPreSettlePrice;				///< 昨结算价
	data["QPrePositionQty"] = task_data.QPrePositionQty;				///< 昨持仓量
	data["QOpeningPrice"] = task_data.QOpeningPrice;					///< 开盘价
	data["QLastPrice"] = task_data.QLastPrice;						///< 最新价
	data["QHighPrice"] = task_data.QHighPrice;						///< 最高价
	data["QLowPrice"] = task_data.QLowPrice;						///< 最低价
	data["QHisHighPrice"] = task_data.QHisHighPrice;					///< 历史最高价
	data["QHisLowPrice"] = task_data.QHisLowPrice;					///< 历史最低价
	data["QLimitUpPrice"] = task_data.QLimitUpPrice;					///< 涨停价
	data["QLimitDownPrice"] = task_data.QLimitDownPrice;				///< 跌停价
	data["QTotalQty"] = task_data.QTotalQty;						///< 当日总成交量
	data["QTotalTurnover"] = task_data.QTotalTurnover;					///< 当日成交金额
	data["QPositionQty"] = task_data.QPositionQty;					///< 持仓量
	data["QAveragePrice"] = task_data.QAveragePrice;					///< 均价
	data["QClosingPrice"] = task_data.QClosingPrice;					///< 收盘价
	data["QSettlePrice"] = task_data.QSettlePrice;					///< 结算价
	data["QLastQty"] = task_data.QLastQty;						///< 最新成交量
	data["QBidPrice1"] = task_data.QBidPrice[0];					///< 买价1-20档
	data["QBidPrice2"] = task_data.QBidPrice[1];					///< 买价1-20档
	data["QBidPrice3"] = task_data.QBidPrice[2];					///< 买价1-20档
	data["QBidPrice4"] = task_data.QBidPrice[3];					///< 买价1-20档
	data["QBidPrice5"] = task_data.QBidPrice[4];					///< 买价1-20档
	data["QBidPrice6"] = task_data.QBidPrice[5];					///< 买价1-20档
	data["QBidPrice7"] = task_data.QBidPrice[6];					///< 买价1-20档
	data["QBidPrice8"] = task_data.QBidPrice[7];					///< 买价1-20档
	data["QBidPrice9"] = task_data.QBidPrice[8];					///< 买价1-20档
	data["QBidPrice10"] = task_data.QBidPrice[9];					///< 买价1-20档
	data["QBidQty1"] = task_data.QBidQty[0];					///< 买价1-20档
	data["QBidQty2"] = task_data.QBidQty[1];					///< 买价1-20档
	data["QBidQty3"] = task_data.QBidQty[2];					///< 买价1-20档
	data["QBidQty4"] = task_data.QBidQty[3];					///< 买价1-20档
	data["QBidQty5"] = task_data.QBidQty[4];					///< 买价1-20档
	data["QBidQty6"] = task_data.QBidQty[5];					///< 买价1-20档
	data["QBidQty7"] = task_data.QBidQty[6];					///< 买价1-20档
	data["QBidQty8"] = task_data.QBidQty[7];					///< 买价1-20档
	data["QBidQty9"] = task_data.QBidQty[8];					///< 买价1-20档
	data["QBidQty10"] = task_data.QBidQty[9];					///< 买价1-20档
	data["QAskPrice1"] = task_data.QAskPrice[0];					///< 买价1-20档
	data["QAskPrice2"] = task_data.QAskPrice[1];					///< 买价1-20档
	data["QAskPrice3"] = task_data.QAskPrice[2];					///< 买价1-20档
	data["QAskPrice4"] = task_data.QAskPrice[3];					///< 买价1-20档
	data["QAskPrice5"] = task_data.QAskPrice[4];					///< 买价1-20档
	data["QAskPrice6"] = task_data.QAskPrice[5];					///< 买价1-20档
	data["QAskPrice7"] = task_data.QAskPrice[6];					///< 买价1-20档
	data["QAskPrice8"] = task_data.QAskPrice[7];					///< 买价1-20档
	data["QAskPrice9"] = task_data.QAskPrice[8];					///< 买价1-20档
	data["QAskPrice10"] = task_data.QAskPrice[9];					///< 买价1-20档
	data["QAskQty1"] = task_data.QAskQty[0];					///< 买价1-20档
	data["QAskQty2"] = task_data.QAskQty[1];					///< 买价1-20档
	data["QAskQty3"] = task_data.QAskQty[2];					///< 买价1-20档
	data["QAskQty4"] = task_data.QAskQty[3];					///< 买价1-20档
	data["QAskQty5"] = task_data.QAskQty[4];					///< 买价1-20档
	data["QAskQty6"] = task_data.QAskQty[5];					///< 买价1-20档
	data["QAskQty7"] = task_data.QAskQty[6];					///< 买价1-20档
	data["QAskQty8"] = task_data.QAskQty[7];					///< 买价1-20档
	data["QAskQty9"] = task_data.QAskQty[8];					///< 买价1-20档
	data["QAskQty10"] = task_data.QAskQty[9];					///< 买价1-20档
	data["QImpliedBidPrice"] = task_data.QImpliedBidPrice;				///< 隐含买价
	data["QImpliedBidQty"] = task_data.QImpliedBidQty;					///< 隐含买量
	data["QImpliedAskPrice"] = task_data.QImpliedAskPrice;				///< 隐含卖价
	data["QImpliedAskQty"] = task_data.QImpliedAskQty;					///< 隐含卖量
	data["QPreDelta"] = task_data.QPreDelta;						///< 昨虚实度
	data["QCurrDelta"] = task_data.QCurrDelta;						///< 今虚实度
	data["QInsideQty"] = task_data.QInsideQty;						///< 内盘量
	data["QOutsideQty"] = task_data.QOutsideQty;					///< 外盘量
	data["QTurnoverRate"] = task_data.QTurnoverRate;					///< 换手率
	data["Q5DAvgQty"] = task_data.Q5DAvgQty;						///< 五日均量
	data["QPERatio"] = task_data.QPERatio;						///< 市盈率
	data["QTotalValue"] = task_data.QTotalValue;					///< 总市值
	data["QNegotiableValue"] = task_data.QNegotiableValue;				///< 流通市值
	data["QPositionTrend"] = task_data.QPositionTrend;					///< 持仓走势
	data["QChangeSpeed"] = task_data.QChangeSpeed;					///< 涨速
	data["QChangeRate"] = task_data.QChangeRate;					///< 涨幅
	data["QChangeValue"] = task_data.QChangeValue;					///< 涨跌值
	data["QSwing"] = task_data.QSwing;							///< 振幅
	data["QTotalBidQty"] = task_data.QTotalBidQty;					///< 委买总量
	data["QTotalAskQty"] = task_data.QTotalAskQty;					///< 委卖总量
	this->onRtnQuote(data);
}


///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------

int MdApi::createTapMdApi(dict req)
{
#ifdef _DEBUG
	fopen_s(&fp, filename, "w");
	if (NULL == fp) {
		cout << "create debug file failed!" << endl;
		return -1;
	}
#endif // DEBUG
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif

	char pDllName[MAX_PATH];
	getStr(req, "DllLocation", pDllName);
	printf("Use DLL Location = %s \n", pDllName);

	HMODULE m_dll = LoadLibrary(pDllName);
	if (!m_dll)
	{
		cout << GetLastError() << endl;
		cout << "Fail to LoadLibrary " << endl;
		getchar();
		return -2;
	}
	
	//取得API的版本信息
	typedef const TAPICHAR* (*GET_VERSION_FUNC)();
	GET_VERSION_FUNC pGetVersion = (GET_VERSION_FUNC)GetProcAddress(m_dll, "GetTapQuoteAPIVersion");
	cout << pGetVersion() << endl;
	//cout << GetTapQuoteAPIVersion() << endl;

	typedef ITapQuoteAPI* (*CREATE_QUOTEAPI_FUNC) (const TapAPIApplicationInfo *appInfo, TAPIINT32 &iResult);
	CREATE_QUOTEAPI_FUNC pCreateQuoteAPI = (CREATE_QUOTEAPI_FUNC)(GetProcAddress(m_dll, "CreateTapQuoteAPI"));


	//创建API实例
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	memset(&stAppInfo, 0, sizeof(stAppInfo));
	getStr(req, "AuthCode", stAppInfo.AuthCode);
	getStr(req, "KeyOperationLogPath", stAppInfo.KeyOperationLogPath);
	//api = CreateTapQuoteAPI(&stAppInfo, iResult);
	api = pCreateQuoteAPI(&stAppInfo, iResult);
	if (NULL == api){
		cout << "创建API实例失败，错误码：" << iResult << endl;
		return -1;
	}

	//设定ITapQuoteAPINotify的实现类，用于异步消息的接收
	return api->SetAPINotify(this);
};


int MdApi::setHostAddress(string ip, int port)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->SetHostAddress(ip.c_str(), port);
}

int MdApi::reqUserLogin(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	//登录服务器
	TapAPIQuoteLoginAuth stLoginAuth;
	memset(&stLoginAuth, 0, sizeof(stLoginAuth));
	getStr(req, "UserID", stLoginAuth.UserNo);
	getStr(req, "Password", stLoginAuth.Password);
	stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
	stLoginAuth.ISDDA = APIYNFLAG_NO;
	return api->Login(&stLoginAuth);
}

int MdApi::exit()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->Disconnect();
}

int MdApi::qryCommodity()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->QryCommodity(&m_sessionID);
}

int MdApi::qryContract(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPICommodity stAPICommodity;
	memset(&stAPICommodity, 0, sizeof(stAPICommodity));
	getStr(req, "ExchangeNo", stAPICommodity.ExchangeNo);
	getStr(req, "CommodityNo", stAPICommodity.CommodityNo);
	getChar(req, "CommodityType", &(stAPICommodity.CommodityType));
	return api->QryContract(&m_sessionID,&stAPICommodity);
}

int MdApi::subscribeMarketData(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIContract stContract;
	memset(&stContract, 0, sizeof(stContract));
	getStr(req, "ExchangeNo", stContract.Commodity.ExchangeNo);
	getChar(req, "CommodityType", &(stContract.Commodity.CommodityType));
	getStr(req, "CommodityNo", stContract.Commodity.CommodityNo);
	getStr(req, "ContractNo1", stContract.ContractNo1);
	stContract.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
	stContract.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
#ifdef _DEBUG
	fprintf(fp, "CommodityType = %c \n", stContract.Commodity.CommodityType); fflush(fp);
#endif
	return api->SubscribeQuote(&m_sessionID, &stContract);

}

int MdApi::unSubscribeMarketData(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIContract stContract;
	memset(&stContract, 0, sizeof(stContract));
	getStr(req, "ExchangeNo", stContract.Commodity.ExchangeNo);
	getChar(req, "CommodityType", &(stContract.Commodity.CommodityType));
	getStr(req, "CommodityNo", stContract.Commodity.CommodityNo);
	getStr(req, "ContractNo1", stContract.ContractNo1);
	stContract.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
	stContract.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
	return api->UnSubscribeQuote(&m_sessionID, &stContract);
}


///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct MdApiWrap : MdApi, wrapper < MdApi >
{

	virtual void onRspLogin(int errorCode, dict data)
	{
		//以下的try...catch...可以实现捕捉python环境中错误的功能，防止C++直接出现原因未知的崩溃
		try
		{
			this->get_override("onRspLogin")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onAPIReady() 
	{
		try
		{
			this->get_override("onAPIReady")();
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onDisconnect(int reasonCode) 
	{
		try
		{
			this->get_override("onDisconnect")(reasonCode);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onRspQryCommodity(int errorCode, bool isLast, dict data)
	{
		try
		{
			this->get_override("onRspQryCommodity")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onRspQryContract(int errorCode, bool isLast, dict data)
	{
		try
		{
			this->get_override("onRspQryContract")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onRspSubscribeQuote(int errorCode, bool isLast, dict data) 
	{
		try
		{
			this->get_override("onRspSubscribeQuote")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};


	virtual void onRspUnSubscribeQuote(int errorCode, bool isLast, dict data) 
	{
		try
		{
			this->get_override("onRspUnSubscribeQuote")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onRtnQuote(dict data) 
	{
		try
		{
			this->get_override("onRtnQuote")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

};


BOOST_PYTHON_MODULE(vntapInmd)
{
	PyEval_InitThreads();	//导入时运行，保证先创建GIL

	class_<MdApiWrap, boost::noncopyable>("MdApi")
		.def("createTapInMdApi", &MdApiWrap::createTapMdApi)
		.def("setHostAddress", &MdApiWrap::setHostAddress)
		.def("reqUserLogin", &MdApiWrap::reqUserLogin)
		.def("exit", &MdApiWrap::exit)
		.def("qryCommodity", &MdApiWrap::qryCommodity)
		.def("qryContract", &MdApiWrap::qryContract)
		.def("subscribeMarketData", &MdApiWrap::subscribeMarketData)
		.def("unSubscribeMarketData", &MdApiWrap::unSubscribeMarketData)

		.def("onRspLogin", pure_virtual(&MdApiWrap::onRspLogin))
		.def("onAPIReady", pure_virtual(&MdApiWrap::onAPIReady))
		.def("onDisconnect", pure_virtual(&MdApiWrap::onDisconnect))
		.def("onRspQryCommodity", pure_virtual(&MdApiWrap::onRspQryCommodity))
		.def("onRspQryContract", pure_virtual(&MdApiWrap::onRspQryContract))
		.def("onRspSubscribeQuote", pure_virtual(&MdApiWrap::onRspSubscribeQuote))
		.def("onRspUnSubscribeQuote", pure_virtual(&MdApiWrap::onRspUnSubscribeQuote))
		.def("onRtnQuote", pure_virtual(&MdApiWrap::onRtnQuote))
		;
};
