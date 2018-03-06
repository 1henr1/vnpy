// vntapIntd.cpp : 定义 DLL 应用程序的导出函数。
//

#include "vntapIntd.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
FILE *fp = NULL;
char filename[] = "tapIn_td_debug.txt";
#endif

using namespace std;


#define  transIsLast(isLast) ((isLast == APIYNFLAG_YES) ? true : false)

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
}

void getUInt(dict d, string key, unsigned int *value)
{
	if (d.has_key(key))		//检查字典中是否存在该键值
	{
		object o = d[key];	//获取该键值
		extract<unsigned int> x(o);	//创建提取器
		if (x.check())		//如果可以提取
		{
			*value = x();	//对目标整数指针赋值
		}
	}
}

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
}

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
}

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
	} }

///-------------------------------------------------------------------------------------
///C++的回调函数将数据保存到队列中
///-------------------------------------------------------------------------------------

void TdApi::OnConnect() {
	Task task = Task();
	task.task_name = DEFINE_OnConnect;
	this->task_queue.push(task);
}
void TdApi::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo) {
	Task task = Task();
	task.task_name = DEFINE_OnRspLogin;
	task.task_id = errorCode;
	if (loginRspInfo)
	{
		task.task_data = *loginRspInfo;
	}
	else
	{
		TapAPITradeLoginRspInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnExpriationDate(TAPIDATE date, int days) {
	Task task = Task();
	task.task_name = DEFINE_OnExpriationDate;
	this->task_queue.push(task);
}
void TdApi::OnAPIReady() {
	Task task = Task();
	task.task_name = DEFINE_OnAPIReady;
	this->task_queue.push(task);
}
void TdApi::OnDisconnect(TAPIINT32 reasonCode) {
	Task task = Task();
	task.task_name = DEFINE_OnDisconnect;
	task.task_id = reasonCode;
	this->task_queue.push(task);
}
void TdApi::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode) {
	Task task = Task();
	task.task_name = DEFINE_OnRspChangePassword;
	task.task_id = errorCode;
	this->task_queue.push(task);
}
void TdApi::OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspSetReservedInfo;
	task.task_id = errorCode;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TAPISTR_50 empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryAccount;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIAccountInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryFund;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFundData empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRtnFund(const TapAPIFundData *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnFund;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFundData empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryExchange;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIExchangeInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryCommodity;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPICommodityInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryContract;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPITradeContractInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRtnContract(const TapAPITradeContractInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnContract;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPITradeContractInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspOrderAction;
	task.task_id = errorCode;
	if (info)
	{
		task.task_error = info->ActionType;
		task.task_data = *(info->OrderInfo);
	}
	else
	{
		TAPIORDERACT actionType;	
		task.task_error = actionType;
		TapAPIOrderInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRtnOrder(const TapAPIOrderInfoNotice *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnOrder;
	if (info)
	{
		task.task_id = info->ErrorCode;
		task.task_data = *(info->OrderInfo);
	}
	else
	{
		TapAPIOrderInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_id = -1;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryOrder;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIOrderInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryOrderProcess;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIOrderInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryFill;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFillInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRtnFill(const TapAPIFillInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnFill;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFillInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryPosition;
	task.task_id = errorCode;
	task.task_last = transIsLast(isLast);
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIPositionInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRtnPosition(const TapAPIPositionInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnPosition;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIPositionInfo empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}
void TdApi::OnRtnPositionProfit(const TapAPIPositionProfitNotice *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnPositionProfit;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIPositionProfitNotice empty_data;
		memset(&empty_data, 0, sizeof(empty_data));
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
}

///-------------------------------------------------------------------------------------
///工作线程从队列中取出数据，转化为python对象后，进行推送
///-------------------------------------------------------------------------------------

void TdApi::processTask()
{
	while (1)
	{
		Task task = this->task_queue.wait_and_pop();

		switch (task.task_name)
		{
		case DEFINE_OnConnect:
		{
			this->processConnect(task);
			break;
		}
		case DEFINE_OnRspLogin:
		{
			this->processRspLogin(task);
			break;
		}
		case DEFINE_OnExpriationDate:
		{
			this->processExpriationDate(task);
			break;
		}
		case DEFINE_OnAPIReady:
		{
			this->processAPIReady(task);
			break;
		}
		case DEFINE_OnDisconnect:
		{
			this->processDisconnect(task);
			break;
		}
		case DEFINE_OnRspChangePassword:
		{
			this->processRspChangePassword(task);
			break;
		}
		case DEFINE_OnRspSetReservedInfo:
		{
			this->processRspSetReservedInfo(task);
			break;
		}
		case DEFINE_OnRspQryAccount:
		{
			this->processRspQryAccount(task);
			break;
		}
		case DEFINE_OnRspQryFund:
		{
			this->processRspQryFund(task);
			break;
		}
		case DEFINE_OnRtnFund:
		{
			this->processRtnFund(task);
			break;
		}
		case DEFINE_OnRspQryExchange:
		{
			this->processRspQryExchange(task);
			break;
		}
		case DEFINE_OnRspQryCommodity:
		{
			this->processRspQryCommodity(task);
			break;
		}
		case DEFINE_OnRspQryContract:
		{
			this->processRspQryContract(task);
			break;
		}
		case DEFINE_OnRtnContract:
		{
			this->processRtnContract(task);
			break;
		}
		case DEFINE_OnRspOrderAction:
		{
			this->processRspOrderAction(task);
			break;
		}
		case DEFINE_OnRtnOrder:
		{
			this->processRtnOrder(task);
			break;
		}
		case DEFINE_OnRspQryOrder:
		{
			this->processRspQryOrder(task);
			break;
		}
		case DEFINE_OnRspQryOrderProcess:
		{
			this->processRspQryOrderProcess(task);
			break;
		}
		case DEFINE_OnRspQryFill:
		{
			this->processRspQryFill(task);
			break;
		}
		case DEFINE_OnRtnFill:
		{
			this->processRtnFill(task);
			break;
		}
		case DEFINE_OnRspQryPosition:
		{
			this->processRspQryPosition(task);
			break;
		}
		case DEFINE_OnRtnPosition:
		{
			this->processRtnPosition(task);
			break;
		}
		case DEFINE_OnRtnPositionProfit:
		{
			this->processRtnPositionProfit(task);
			break;
		}
		}
	}
}

/*  process List */

void TdApi::processConnect(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onConnect();
};

void TdApi::processRspLogin(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPITradeLoginRspInfo task_data = any_cast<TapAPITradeLoginRspInfo>(task.task_data);
	dict data;
	data["UserNo"] = task_data.UserNo;							///< 用户编号
	data["UserType"] = task_data.UserType;						///< 用户类型
	data["UserName"] = task_data.UserName;						///< 用户名
	data["ReservedInfo"] = task_data.ReservedInfo;					///< 预留信息
	data["LastLoginIP"] = task_data.LastLoginIP;					///< 上次登录IP
	data["LastLoginProt"] = task_data.LastLoginProt;					///< 上次登录端口
	data["LastLoginTime"] = task_data.LastLoginTime;					///< 上次登录时间
	data["LastLogoutTime"] = task_data.LastLogoutTime;					///< 上次退出时间
	data["TradeDate"] = task_data.TradeDate;						///< 当前交易日期
	data["LastSettleTime"] = task_data.LastSettleTime;					///< 上次结算时间
	data["StartTime"] = task_data.StartTime;						///< 系统启动时间
	data["InitTime"] = task_data.InitTime;						///< 系统初始化时间
	this->onRspLogin(task.task_id, data);
};

void TdApi::processExpriationDate(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onExpriationDate(data);
};

void TdApi::processAPIReady(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onAPIReady();
};

void TdApi::processDisconnect(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onDisconnect(task.task_id);
};

void TdApi::processRspChangePassword(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onRspChangePassword(task.task_id);
};

void TdApi::processRspSetReservedInfo(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspSetReservedInfo(task.task_id, data);
};

void TdApi::processRspQryAccount(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIAccountInfo task_data = any_cast<TapAPIAccountInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;                              ///< 资金账号
	data["AccountType"] = task_data.AccountType;                            ///< 账号类型
	data["AccountState"] = task_data.AccountState;                           ///< 账号状态
	data["AccountShortName"] = task_data.AccountShortName;                       ///< 账号简称
	this->onRspQryAccount(task.task_id, task.task_last, data);
};

void TdApi::processRspQryFund(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFundData task_data = any_cast<TapAPIFundData>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金账号
	data["CurrencyGroupNo"] = task_data.CurrencyGroupNo;				///< 币种组号
	data["CurrencyNo"] = task_data.CurrencyNo;						///< 币种号(为空表示币种组基币资金)
	data["TradeRate"] = task_data.TradeRate;						///< 交易汇率
	data["FutureAlg"] = task_data.FutureAlg;                      ///< 期货算法
	data["OptionAlg"] = task_data.OptionAlg;                      ///< 期权算法
	data["PreBalance"] = task_data.PreBalance;						///< 上日结存
	data["PreUnExpProfit"] = task_data.PreUnExpProfit;					///< 上日未到期平盈
	data["PreLMEPositionProfit"] = task_data.PreLMEPositionProfit;			///< 上日LME持仓平盈
	data["PreEquity"] = task_data.PreEquity;						///< 上日权益
	data["PreAvailable1"] = task_data.PreAvailable1;					///< 上日可用
	data["PreMarketEquity"] = task_data.PreMarketEquity;				///< 上日市值权益
	data["CashInValue"] = task_data.CashInValue;					///< 入金
	data["CashOutValue"] = task_data.CashOutValue;					///< 出金
	data["CashAdjustValue"] = task_data.CashAdjustValue;				///< 资金调整
	data["CashPledged"] = task_data.CashPledged;					///< 质押资金
	data["FrozenFee"] = task_data.FrozenFee;						///< 冻结手续费
	data["FrozenDeposit"] = task_data.FrozenDeposit;					///< 冻结保证金
	data["AccountFee"] = task_data.AccountFee;						///< 客户手续费包含交割手续费
	data["ExchangeFee"] = task_data.ExchangeFee;					///< 汇兑手续费
	data["AccountDeliveryFee"] = task_data.AccountDeliveryFee;				///< 客户交割手续费
	data["PremiumIncome"] = task_data.PremiumIncome;					///< 权利金收取
	data["PremiumPay"] = task_data.PremiumPay;						///< 权利金支付
	data["CloseProfit"] = task_data.CloseProfit;					///< 平仓盈亏
	data["DeliveryProfit"] = task_data.DeliveryProfit;					///< 交割盈亏
	data["UnExpProfit"] = task_data.UnExpProfit;					///< 未到期平盈
	data["ExpProfit"] = task_data.ExpProfit;						///< 到期平仓盈亏
	data["PositionProfit"] = task_data.PositionProfit;					///< 不含LME持仓盈亏
	data["LmePositionProfit"] = task_data.LmePositionProfit;				///< LME持仓盈亏
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< 期权市值
	data["AccountIntialMargin"] = task_data.AccountIntialMargin;			///< 客户初始保证金
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< 客户维持保证金
	data["UpperInitalMargin"] = task_data.UpperInitalMargin;				///< 上手初始保证金
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< 上手维持保证金
	data["Discount"] = task_data.Discount;						///< LME贴现
	data["Balance"] = task_data.Balance;						///< 当日结存
	data["Equity"] = task_data.Equity;							///< 当日权益
	data["Available"] = task_data.Available;						///< 当日可用
	data["CanDraw"] = task_data.CanDraw;						///< 可提取
	data["MarketEquity"] = task_data.MarketEquity;					///< 账户市值
	this->onRspQryFund(task.task_id, task.task_last, data);
};

void TdApi::processRtnFund(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFundData task_data = any_cast<TapAPIFundData>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金账号
	data["CurrencyGroupNo"] = task_data.CurrencyGroupNo;				///< 币种组号
	data["CurrencyNo"] = task_data.CurrencyNo;						///< 币种号(为空表示币种组基币资金)
	data["TradeRate"] = task_data.TradeRate;						///< 交易汇率
	data["FutureAlg"] = task_data.FutureAlg;                      ///< 期货算法
	data["OptionAlg"] = task_data.OptionAlg;                      ///< 期权算法
	data["PreBalance"] = task_data.PreBalance;						///< 上日结存
	data["PreUnExpProfit"] = task_data.PreUnExpProfit;					///< 上日未到期平盈
	data["PreLMEPositionProfit"] = task_data.PreLMEPositionProfit;			///< 上日LME持仓平盈
	data["PreEquity"] = task_data.PreEquity;						///< 上日权益
	data["PreAvailable1"] = task_data.PreAvailable1;					///< 上日可用
	data["PreMarketEquity"] = task_data.PreMarketEquity;				///< 上日市值权益
	data["CashInValue"] = task_data.CashInValue;					///< 入金
	data["CashOutValue"] = task_data.CashOutValue;					///< 出金
	data["CashAdjustValue"] = task_data.CashAdjustValue;				///< 资金调整
	data["CashPledged"] = task_data.CashPledged;					///< 质押资金
	data["FrozenFee"] = task_data.FrozenFee;						///< 冻结手续费
	data["FrozenDeposit"] = task_data.FrozenDeposit;					///< 冻结保证金
	data["AccountFee"] = task_data.AccountFee;						///< 客户手续费包含交割手续费
	data["ExchangeFee"] = task_data.ExchangeFee;					///< 汇兑手续费
	data["AccountDeliveryFee"] = task_data.AccountDeliveryFee;				///< 客户交割手续费
	data["PremiumIncome"] = task_data.PremiumIncome;					///< 权利金收取
	data["PremiumPay"] = task_data.PremiumPay;						///< 权利金支付
	data["CloseProfit"] = task_data.CloseProfit;					///< 平仓盈亏
	data["DeliveryProfit"] = task_data.DeliveryProfit;					///< 交割盈亏
	data["UnExpProfit"] = task_data.UnExpProfit;					///< 未到期平盈
	data["ExpProfit"] = task_data.ExpProfit;						///< 到期平仓盈亏
	data["PositionProfit"] = task_data.PositionProfit;					///< 不含LME持仓盈亏
	data["LmePositionProfit"] = task_data.LmePositionProfit;				///< LME持仓盈亏
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< 期权市值
	data["AccountIntialMargin"] = task_data.AccountIntialMargin;			///< 客户初始保证金
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< 客户维持保证金
	data["UpperInitalMargin"] = task_data.UpperInitalMargin;				///< 上手初始保证金
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< 上手维持保证金
	data["Discount"] = task_data.Discount;						///< LME贴现
	data["Balance"] = task_data.Balance;						///< 当日结存
	data["Equity"] = task_data.Equity;							///< 当日权益
	data["Available"] = task_data.Available;						///< 当日可用
	data["CanDraw"] = task_data.CanDraw;						///< 可提取
	data["MarketEquity"] = task_data.MarketEquity;					///< 账户市值
	this->onRtnFund(data);
};

void TdApi::processRspQryExchange(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIExchangeInfo task_data = any_cast<TapAPIExchangeInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;								///< 交易所编码
	data["ExchangeName"] = task_data.ExchangeName;							///< 交易所名称
	this->onRspQryExchange(task.task_id, task.task_last, data);
};

void TdApi::processRspQryCommodity(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPICommodityInfo task_data = any_cast<TapAPICommodityInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;						//交易所编码
	data["CommodityType"] = task_data.CommodityType;					//品种类型
	data["CommodityNo"] = task_data.CommodityNo;					//品种编号
	data["CommodityName"] = task_data.CommodityName;					//品种名称
	data["CommodityEngName"] = task_data.CommodityEngName;				//品种英文名称
	data["TradeCurrency"] = task_data.TradeCurrency;					//交易币种
	data["ContractSize"] = task_data.ContractSize;					//每手乘数
	data["OpenCloseMode"] = task_data.OpenCloseMode;					//开平方式
	data["StrikePriceTimes"] = task_data.StrikePriceTimes;				//执行价格倍数
	data["CommodityTickSize"] = task_data.CommodityTickSize;				//最小变动价位
	data["CommodityDenominator"] = task_data.CommodityDenominator;			//报价分母
	data["CmbDirect"] = task_data.CmbDirect;						//组合方向
	data["DeliveryMode"] = task_data.DeliveryMode;					//交割行权方式
	data["DeliveryDays"] = task_data.DeliveryDays;					//交割日偏移
	data["AddOneTime"] = task_data.AddOneTime;						//T+1分割时间
	data["CommodityTimeZone"] = task_data.CommodityTimeZone;				//品种时区

	this->onRspQryCommodity(task.task_id, task.task_last, data);
};

void TdApi::processRspQryContract(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPITradeContractInfo task_data = any_cast<TapAPITradeContractInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;                             ///< 交易所编码
	data["CommodityType"] = task_data.CommodityType;                          ///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;                            ///< 品种编号
	data["ContractNo1"] = task_data.ContractNo1;                            ///< 合约代码1
	data["StrikePrice1"] = task_data.StrikePrice1;                           ///< 执行价1
	data["CallOrPutFlag1"] = task_data.CallOrPutFlag1;                         ///< 看涨看跌标示1
	data["ContractNo2"] = task_data.ContractNo2;                            ///< 合约代码2
	data["StrikePrice2"] = task_data.StrikePrice2;                           ///< 执行价2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;                         ///< 看涨看跌标示2
	data["ContractType"] = task_data.ContractType;                           ///< 合约类型
	data["QuoteUnderlyingContract"] = task_data.QuoteUnderlyingContract;				///< 行情真实合约
	data["ContractName"] = task_data.ContractName;                           ///< 合约名称
	data["ContractExpDate"] = task_data.ContractExpDate;                        ///< 合约到期日	
	data["LastTradeDate"] = task_data.LastTradeDate;                          ///< 最后交易日
	data["FirstNoticeDate"] = task_data.FirstNoticeDate;                        ///< 首次通知日
	this->onRspQryContract(task.task_id, task.task_last, data);
};

void TdApi::processRtnContract(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPITradeContractInfo task_data = any_cast<TapAPITradeContractInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;                             ///< 交易所编码
	data["CommodityType"] = task_data.CommodityType;                          ///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;                            ///< 品种编号
	data["ContractNo1"] = task_data.ContractNo1;                            ///< 合约代码1
	data["StrikePrice1"] = task_data.StrikePrice1;                           ///< 执行价1
	data["CallOrPutFlag1"] = task_data.CallOrPutFlag1;                         ///< 看涨看跌标示1
	data["ContractNo2"] = task_data.ContractNo2;                            ///< 合约代码2
	data["StrikePrice2"] = task_data.StrikePrice2;                           ///< 执行价2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;                         ///< 看涨看跌标示2
	data["ContractType"] = task_data.ContractType;                           ///< 合约类型
	data["QuoteUnderlyingContract"] = task_data.QuoteUnderlyingContract;				///< 行情真实合约
	data["ContractName"] = task_data.ContractName;                           ///< 合约名称
	data["ContractExpDate"] = task_data.ContractExpDate;                        ///< 合约到期日	
	data["LastTradeDate"] = task_data.LastTradeDate;                          ///< 最后交易日
	data["FirstNoticeDate"] = task_data.FirstNoticeDate;                        ///< 首次通知日
	this->onRtnContract(data);
};

void TdApi::processRspOrderAction(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	TAPIORDERACT ActionType = any_cast<TAPIORDERACT>(task.task_error);
	data["ActionType"] = ActionType;
	TapAPIOrderInfo task_data = any_cast<TapAPIOrderInfo>(task.task_data);
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金帐号
	data["ExchangeNo"] = task_data.ExchangeNo;						///< 交易所编号
	data["CommodityType"] = task_data.CommodityType;					///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;					///< 品种编码类型
	data["ContractNo"] = task_data.ContractNo;						///< 合约1
	data["StrikePrice"] = task_data.StrikePrice;					///< 执行价格1
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< 看张看跌1
	data["ContractNo2"] = task_data.ContractNo2;					///< 合约2
	data["StrikePrice2"] = task_data.StrikePrice2;					///< 执行价格2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;					///< 看张看跌2
	data["OrderType"] = task_data.OrderType;						///< 委托类型
	data["OrderSource"] = task_data.OrderSource;					///< 委托来源
	data["TimeInForce"] = task_data.TimeInForce;					///< 委托有效类型
	data["ExpireTime"] = task_data.ExpireTime;						///< 有效日期(GTD情况下使用)
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< 是否风险报单
	data["OrderSide"] = task_data.OrderSide;						///< 买入卖出
	data["PositionEffect"] = task_data.PositionEffect;					///< 开平标志1
	data["PositionEffect2"] = task_data.PositionEffect2;				///< 开平标志2
	data["InquiryNo"] = task_data.InquiryNo;						///< 询价号
	data["HedgeFlag"] = task_data.HedgeFlag;						///< 投机保值
	data["OrderPrice"] = task_data.OrderPrice;						///< 委托价格1
	data["OrderPrice2"] = task_data.OrderPrice2;					///< 委托价格2，做市商应价使用
	data["StopPrice"] = task_data.StopPrice;						///< 触发价格
	data["OrderQty"] = task_data.OrderQty;						///< 委托数量
	data["OrderMinQty"] = task_data.OrderMinQty;					///< 最小成交量
	data["RefInt"] = task_data.RefInt;							///< 整型参考值
	data["RefString"] = task_data.RefString;                                      ///< 字符串参考值
	data["MinClipSize"] = task_data.MinClipSize;					///< 冰山单最小随机量
	data["MaxClipSize"] = task_data.MaxClipSize;					///< 冰山单最大随机量
	data["LicenseNo"] = task_data.LicenseNo;						///< 软件授权号
	data["ServerFlag"] = task_data.ServerFlag;						///< 服务器标识
	data["OrderNo"] = task_data.OrderNo;						///< 委托编码
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< 客户端本地委托编号
	data["TacticsType"] = task_data.TacticsType;					///< 策略单类型
	data["TriggerCondition"] = task_data.TriggerCondition;				///< 触发条件
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< 触发价格类型
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< 是否T+1有效
	data["OrderStreamID"] = task_data.OrderStreamID;					///< 委托流水号
	data["UpperNo"] = task_data.UpperNo;						///< 上手号
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< 上手通道号
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< 本地号
	data["UpperStreamID"] = task_data.UpperStreamID;					///< 上手流号
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< 系统号
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< 交易所系统号 
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< 父单系统号
	data["OrderInsertUserNo"] = task_data.OrderInsertUserNo;				///< 下单人
	data["OrderInsertTime"] = task_data.OrderInsertTime;				///< 下单时间
	data["OrderCommandUserNo"] = task_data.OrderCommandUserNo;				///< 录单操作人
	data["OrderUpdateUserNo"] = task_data.OrderUpdateUserNo;				///< 委托更新人
	data["OrderUpdateTime"] = task_data.OrderUpdateTime;				///< 委托更新时间
	data["OrderState"] = task_data.OrderState;						///< 委托状态
	data["OrderMatchPrice"] = task_data.OrderMatchPrice;				///< 成交价1
	data["OrderMatchPrice2"] = task_data.OrderMatchPrice2;				///< 成交价2
	data["OrderMatchQty"] = task_data.OrderMatchQty;					///< 成交量1
	data["OrderMatchQty2"] = task_data.OrderMatchQty2;					///< 成交量2
	data["ErrorCode"] = task_data.ErrorCode;						///< 最后一次操作错误信息码
	data["ErrorText"] = task_data.ErrorText;						///< 错误信息
	data["IsBackInput"] = task_data.IsBackInput;					///< 是否为录入委托单
	data["IsDeleted"] = task_data.IsDeleted;						///< 委托成交删除标
	data["IsAddOne"] = task_data.IsAddOne;						///< 是否为T+1单
	this->onRspOrderAction(task.task_id, data);
};

void TdApi::processRtnOrder(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIOrderInfo task_data = any_cast<TapAPIOrderInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金帐号
	data["ExchangeNo"] = task_data.ExchangeNo;						///< 交易所编号
	data["CommodityType"] = task_data.CommodityType;					///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;					///< 品种编码类型
	data["ContractNo"] = task_data.ContractNo;						///< 合约1
	data["StrikePrice"] = task_data.StrikePrice;					///< 执行价格1
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< 看张看跌1
	data["ContractNo2"] = task_data.ContractNo2;					///< 合约2
	data["StrikePrice2"] = task_data.StrikePrice2;					///< 执行价格2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;					///< 看张看跌2
	data["OrderType"] = task_data.OrderType;						///< 委托类型
	data["OrderSource"] = task_data.OrderSource;					///< 委托来源
	data["TimeInForce"] = task_data.TimeInForce;					///< 委托有效类型
	data["ExpireTime"] = task_data.ExpireTime;						///< 有效日期(GTD情况下使用)
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< 是否风险报单
	data["OrderSide"] = task_data.OrderSide;						///< 买入卖出
	data["PositionEffect"] = task_data.PositionEffect;					///< 开平标志1
	data["PositionEffect2"] = task_data.PositionEffect2;				///< 开平标志2
	data["InquiryNo"] = task_data.InquiryNo;						///< 询价号
	data["HedgeFlag"] = task_data.HedgeFlag;						///< 投机保值
	data["OrderPrice"] = task_data.OrderPrice;						///< 委托价格1
	data["OrderPrice2"] = task_data.OrderPrice2;					///< 委托价格2，做市商应价使用
	data["StopPrice"] = task_data.StopPrice;						///< 触发价格
	data["OrderQty"] = task_data.OrderQty;						///< 委托数量
	data["OrderMinQty"] = task_data.OrderMinQty;					///< 最小成交量
	data["RefInt"] = task_data.RefInt;							///< 整型参考值
	data["RefString"] = task_data.RefString;                                      ///< 字符串参考值
	data["MinClipSize"] = task_data.MinClipSize;					///< 冰山单最小随机量
	data["MaxClipSize"] = task_data.MaxClipSize;					///< 冰山单最大随机量
	data["LicenseNo"] = task_data.LicenseNo;						///< 软件授权号
	data["ServerFlag"] = task_data.ServerFlag;						///< 服务器标识
	data["OrderNo"] = task_data.OrderNo;						///< 委托编码
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< 客户端本地委托编号
	data["TacticsType"] = task_data.TacticsType;					///< 策略单类型
	data["TriggerCondition"] = task_data.TriggerCondition;				///< 触发条件
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< 触发价格类型
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< 是否T+1有效
	data["OrderStreamID"] = task_data.OrderStreamID;					///< 委托流水号
	data["UpperNo"] = task_data.UpperNo;						///< 上手号
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< 上手通道号
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< 本地号
	data["UpperStreamID"] = task_data.UpperStreamID;					///< 上手流号
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< 系统号
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< 交易所系统号 
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< 父单系统号
	data["OrderInsertUserNo"] = task_data.OrderInsertUserNo;				///< 下单人
	data["OrderInsertTime"] = task_data.OrderInsertTime;				///< 下单时间
	data["OrderCommandUserNo"] = task_data.OrderCommandUserNo;				///< 录单操作人
	data["OrderUpdateUserNo"] = task_data.OrderUpdateUserNo;				///< 委托更新人
	data["OrderUpdateTime"] = task_data.OrderUpdateTime;				///< 委托更新时间
	data["OrderState"] = task_data.OrderState;						///< 委托状态
	data["OrderMatchPrice"] = task_data.OrderMatchPrice;				///< 成交价1
	data["OrderMatchPrice2"] = task_data.OrderMatchPrice2;				///< 成交价2
	data["OrderMatchQty"] = task_data.OrderMatchQty;					///< 成交量1
	data["OrderMatchQty2"] = task_data.OrderMatchQty2;					///< 成交量2
	data["ErrorCode"] = task_data.ErrorCode;						///< 最后一次操作错误信息码
	data["ErrorText"] = task_data.ErrorText;						///< 错误信息
	data["IsBackInput"] = task_data.IsBackInput;					///< 是否为录入委托单
	data["IsDeleted"] = task_data.IsDeleted;						///< 委托成交删除标
	data["IsAddOne"] = task_data.IsAddOne;						///< 是否为T+1单
	this->onRtnOrder(data);
};

void TdApi::processRspQryOrder(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIOrderInfo task_data = any_cast<TapAPIOrderInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金帐号
	data["ExchangeNo"] = task_data.ExchangeNo;						///< 交易所编号
	data["CommodityType"] = task_data.CommodityType;					///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;					///< 品种编码类型
	data["ContractNo"] = task_data.ContractNo;						///< 合约1
	data["StrikePrice"] = task_data.StrikePrice;					///< 执行价格1
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< 看张看跌1
	data["ContractNo2"] = task_data.ContractNo2;					///< 合约2
	data["StrikePrice2"] = task_data.StrikePrice2;					///< 执行价格2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;					///< 看张看跌2
	data["OrderType"] = task_data.OrderType;						///< 委托类型
	data["OrderSource"] = task_data.OrderSource;					///< 委托来源
	data["TimeInForce"] = task_data.TimeInForce;					///< 委托有效类型
	data["ExpireTime"] = task_data.ExpireTime;						///< 有效日期(GTD情况下使用)
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< 是否风险报单
	data["OrderSide"] = task_data.OrderSide;						///< 买入卖出
	data["PositionEffect"] = task_data.PositionEffect;					///< 开平标志1
	data["PositionEffect2"] = task_data.PositionEffect2;				///< 开平标志2
	data["InquiryNo"] = task_data.InquiryNo;						///< 询价号
	data["HedgeFlag"] = task_data.HedgeFlag;						///< 投机保值
	data["OrderPrice"] = task_data.OrderPrice;						///< 委托价格1
	data["OrderPrice2"] = task_data.OrderPrice2;					///< 委托价格2，做市商应价使用
	data["StopPrice"] = task_data.StopPrice;						///< 触发价格
	data["OrderQty"] = task_data.OrderQty;						///< 委托数量
	data["OrderMinQty"] = task_data.OrderMinQty;					///< 最小成交量
	data["RefInt"] = task_data.RefInt;							///< 整型参考值
	data["RefString"] = task_data.RefString;                                      ///< 字符串参考值
	data["MinClipSize"] = task_data.MinClipSize;					///< 冰山单最小随机量
	data["MaxClipSize"] = task_data.MaxClipSize;					///< 冰山单最大随机量
	data["LicenseNo"] = task_data.LicenseNo;						///< 软件授权号
	data["ServerFlag"] = task_data.ServerFlag;						///< 服务器标识
	data["OrderNo"] = task_data.OrderNo;						///< 委托编码
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< 客户端本地委托编号
	data["TacticsType"] = task_data.TacticsType;					///< 策略单类型
	data["TriggerCondition"] = task_data.TriggerCondition;				///< 触发条件
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< 触发价格类型
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< 是否T+1有效
	data["OrderStreamID"] = task_data.OrderStreamID;					///< 委托流水号
	data["UpperNo"] = task_data.UpperNo;						///< 上手号
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< 上手通道号
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< 本地号
	data["UpperStreamID"] = task_data.UpperStreamID;					///< 上手流号
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< 系统号
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< 交易所系统号 
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< 父单系统号
	data["OrderInsertUserNo"] = task_data.OrderInsertUserNo;				///< 下单人
	data["OrderInsertTime"] = task_data.OrderInsertTime;				///< 下单时间
	data["OrderCommandUserNo"] = task_data.OrderCommandUserNo;				///< 录单操作人
	data["OrderUpdateUserNo"] = task_data.OrderUpdateUserNo;				///< 委托更新人
	data["OrderUpdateTime"] = task_data.OrderUpdateTime;				///< 委托更新时间
	data["OrderState"] = task_data.OrderState;						///< 委托状态
	data["OrderMatchPrice"] = task_data.OrderMatchPrice;				///< 成交价1
	data["OrderMatchPrice2"] = task_data.OrderMatchPrice2;				///< 成交价2
	data["OrderMatchQty"] = task_data.OrderMatchQty;					///< 成交量1
	data["OrderMatchQty2"] = task_data.OrderMatchQty2;					///< 成交量2
	data["ErrorCode"] = task_data.ErrorCode;						///< 最后一次操作错误信息码
	data["ErrorText"] = task_data.ErrorText;						///< 错误信息
	data["IsBackInput"] = task_data.IsBackInput;					///< 是否为录入委托单
	data["IsDeleted"] = task_data.IsDeleted;						///< 委托成交删除标
	data["IsAddOne"] = task_data.IsAddOne;						///< 是否为T+1单
	this->onRspQryOrder(task.task_id, task.task_last, data);
};

void TdApi::processRspQryOrderProcess(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryOrderProcess(task.task_id, task.task_last, data);
};

void TdApi::processRspQryFill(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryFill(task.task_id, task.task_last, data);
};

void TdApi::processRtnFill(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFillInfo task_data = any_cast<TapAPIFillInfo>(task.task_data);
	dict data;
    data["AccountNo"] = task_data.AccountNo;						///< 客户资金帐号
    data["ExchangeNo"] = task_data.ExchangeNo;						///< 交易所编号
    data["CommodityType"] = task_data.CommodityType;					///< 品种类型
    data["CommodityNo"] = task_data.CommodityNo;					///< 品种编码类型
    data["ContractNo"] = task_data.ContractNo;						///< 合约1
    data["StrikePrice"] = task_data.StrikePrice;					///< 执行价格
    data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< 看张看跌
    data["MatchSource"] = task_data.MatchSource;					///< 委托来源
    data["MatchSide"] = task_data.MatchSide;						///< 买入卖出
    data["PositionEffect"] = task_data.PositionEffect;					///< 开平标志1
    data["ServerFlag"] = task_data.ServerFlag;						///< 服务器标识
    data["OrderNo"] = task_data.OrderNo;						///< 委托编码
    data["MatchNo"] = task_data.MatchNo;						///< 本地成交号
    data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< 交易所成交号
    data["MatchDateTime"] = task_data.MatchDateTime;					///< 成交时间
    data["UpperMatchDateTime"] = task_data.UpperMatchDateTime;				///< 上手成交时间
    data["UpperNo"] = task_data.UpperNo;						///< 上手号
    data["MatchPrice"] = task_data.MatchPrice;						///< 成交价
    data["MatchQty"] = task_data.MatchQty;						///< 成交量
    data["IsDeleted"] = task_data.IsDeleted;						///< 委托成交删除标
    data["IsAddOne"] = task_data.IsAddOne;						///< 是否为T+1单
    data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< 客户手续费币种组
    data["FeeCurrency"] = task_data.FeeCurrency;					///< 客户手续费币种
    data["FeeValue"] = task_data.FeeValue;						///< 手续费
    data["IsManualFee"] = task_data.IsManualFee;					///< 人工客户手续费标记
	this->onRtnFill(data);
};

void TdApi::processRspQryPosition(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIPositionInfo task_data = any_cast<TapAPIPositionInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金帐号
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< 上级资金账号
	data["ExchangeNo"] = task_data.ExchangeNo;						///< 交易所编号
	data["CommodityType"] = task_data.CommodityType;					///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;					///< 品种编码类型
	data["ContractNo"] = task_data.ContractNo;						///< 合约1
	data["StrikePrice"] = task_data.StrikePrice;					///< 执行价格
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< 看张看跌
	data["ExchangeNoRef"] = task_data.ExchangeNoRef;					///< 交易所编号
	data["CommodityTypeRef"] = task_data.CommodityTypeRef;				///< 品种类型
	data["CommodityNoRef"] = task_data.CommodityNoRef;					///< 品种编码类型
	data["ContractNoRef"] = task_data.ContractNoRef;					///< 合约
	data["StrikePriceRef"] = task_data.StrikePriceRef;					///< 执行价格
	data["CallOrPutFlagRef"] = task_data.CallOrPutFlagRef;				///< 看张看跌
	data["OrderExchangeNo"] = task_data.OrderExchangeNo;				///< 交易所编号
	data["OrderCommodityType"] = task_data.OrderCommodityType;				///< 品种类型
	data["OrderCommodityNo"] = task_data.OrderCommodityNo;				///< 品种编码类型
	data["OrderType"] = task_data.OrderType;						///< 委托类型
	data["MatchSource"] = task_data.MatchSource;					///< 委托来源
	data["TimeInForce"] = task_data.TimeInForce;					///< 委托有效类型
	data["ExpireTime"] = task_data.ExpireTime;						///< 有效日期(GTD情况下使用)
	data["MatchSide"] = task_data.MatchSide;						///< 买入卖出
	data["HedgeFlag"] = task_data.HedgeFlag;						///< 投机保值
	data["PositionNo"] = task_data.PositionNo;						///< 本地持仓号，服务器编写
	data["ServerFlag"] = task_data.ServerFlag;						///< 服务器标识
	data["OrderNo"] = task_data.OrderNo;						///< 委托编码
	data["MatchNo"] = task_data.MatchNo;						///< 本地成交号
	data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< 交易所成交号
	data["MatchDate"] = task_data.MatchDate;						///< 成交日期
	data["MatchTime"] = task_data.MatchTime;						///< 成交时间
	data["UpperMatchTime"] = task_data.UpperMatchTime;					///< 上手成交时间
	data["UpperNo"] = task_data.UpperNo;						///< 上手号
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< 会员号和清算代码
	data["UpperUserNo"] = task_data.UpperUserNo;					///< 上手登录号
	data["TradeNo"] = task_data.TradeNo;						///< 交易编码
	data["PositionPrice"] = task_data.PositionPrice;					///< 持仓价
	data["PositionQty"] = task_data.PositionQty;					///< 持仓量
	data["IsBackInput"] = task_data.IsBackInput;					///< 是否为录入委托单
	data["IsAddOne"] = task_data.IsAddOne;						///< 是否为T+1单
	data["MatchStreamID"] = task_data.MatchStreamID;					///< 成交流水号
	data["PositionStreamId"] = task_data.PositionStreamId;				///< 持仓流号
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< 开平方式
	data["ContractSize"] = task_data.ContractSize;					///< 每手乘数，计算参数
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< 品种币种组
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< 品种币种
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< 昨结算价
	data["SettlePrice"] = task_data.SettlePrice;					///< 当前计算价格
	data["Turnover"] = task_data.Turnover;						///< 持仓金额
	data["AccountMarginMode"] = task_data.AccountMarginMode;				///< 保证金计算方式
	data["AccountMarginParam"] = task_data.AccountMarginParam;				///< 保证金参数值
	data["UpperMarginMode"] = task_data.UpperMarginMode;				///< 保证金计算方式
	data["UpperMarginParam"] = task_data.UpperMarginParam;				///< 保证金参数值
	data["AccountInitialMargin"] = task_data.AccountInitialMargin;			///< 客户初始保证金
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< 客户维持保证金
	data["UpperInitialMargin"] = task_data.UpperInitialMargin;				///< 上手初始保证金
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< 上手维持保证金
	data["PositionProfit"] = task_data.PositionProfit;					///< 持仓盈亏
	data["LMEPositionProfit"] = task_data.LMEPositionProfit;				///< LME持仓盈亏
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< 期权市值
	data["MatchCmbNo"] = task_data.MatchCmbNo;						///< 组合持仓号
	data["IsHistory"] = task_data.IsHistory;						///< 是否历史持仓
	data["FloatingPL"] = task_data.FloatingPL;						///< 逐笔浮盈
	this->onRspQryPosition(task.task_id, task.task_last, data);
};

void TdApi::processRtnPosition(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIPositionInfo task_data = any_cast<TapAPIPositionInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< 客户资金帐号
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< 上级资金账号
	data["ExchangeNo"] = task_data.ExchangeNo;						///< 交易所编号
	data["CommodityType"] = task_data.CommodityType;					///< 品种类型
	data["CommodityNo"] = task_data.CommodityNo;					///< 品种编码类型
	data["ContractNo"] = task_data.ContractNo;						///< 合约1
	data["StrikePrice"] = task_data.StrikePrice;					///< 执行价格
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< 看张看跌
	data["ExchangeNoRef"] = task_data.ExchangeNoRef;					///< 交易所编号
	data["CommodityTypeRef"] = task_data.CommodityTypeRef;				///< 品种类型
	data["CommodityNoRef"] = task_data.CommodityNoRef;					///< 品种编码类型
	data["ContractNoRef"] = task_data.ContractNoRef;					///< 合约
	data["StrikePriceRef"] = task_data.StrikePriceRef;					///< 执行价格
	data["CallOrPutFlagRef"] = task_data.CallOrPutFlagRef;				///< 看张看跌
	data["OrderExchangeNo"] = task_data.OrderExchangeNo;				///< 交易所编号
	data["OrderCommodityType"] = task_data.OrderCommodityType;				///< 品种类型
	data["OrderCommodityNo"] = task_data.OrderCommodityNo;				///< 品种编码类型
	data["OrderType"] = task_data.OrderType;						///< 委托类型
	data["MatchSource"] = task_data.MatchSource;					///< 委托来源
	data["TimeInForce"] = task_data.TimeInForce;					///< 委托有效类型
	data["ExpireTime"] = task_data.ExpireTime;						///< 有效日期(GTD情况下使用)
	data["MatchSide"] = task_data.MatchSide;						///< 买入卖出
	data["HedgeFlag"] = task_data.HedgeFlag;						///< 投机保值
	data["PositionNo"] = task_data.PositionNo;						///< 本地持仓号，服务器编写
	data["ServerFlag"] = task_data.ServerFlag;						///< 服务器标识
	data["OrderNo"] = task_data.OrderNo;						///< 委托编码
	data["MatchNo"] = task_data.MatchNo;						///< 本地成交号
	data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< 交易所成交号
	data["MatchDate"] = task_data.MatchDate;						///< 成交日期
	data["MatchTime"] = task_data.MatchTime;						///< 成交时间
	data["UpperMatchTime"] = task_data.UpperMatchTime;					///< 上手成交时间
	data["UpperNo"] = task_data.UpperNo;						///< 上手号
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< 会员号和清算代码
	data["UpperUserNo"] = task_data.UpperUserNo;					///< 上手登录号
	data["TradeNo"] = task_data.TradeNo;						///< 交易编码
	data["PositionPrice"] = task_data.PositionPrice;					///< 持仓价
	data["PositionQty"] = task_data.PositionQty;					///< 持仓量
	data["IsBackInput"] = task_data.IsBackInput;					///< 是否为录入委托单
	data["IsAddOne"] = task_data.IsAddOne;						///< 是否为T+1单
	data["MatchStreamID"] = task_data.MatchStreamID;					///< 成交流水号
	data["PositionStreamId"] = task_data.PositionStreamId;				///< 持仓流号
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< 开平方式
	data["ContractSize"] = task_data.ContractSize;					///< 每手乘数，计算参数
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< 品种币种组
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< 品种币种
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< 昨结算价
	data["SettlePrice"] = task_data.SettlePrice;					///< 当前计算价格
	data["Turnover"] = task_data.Turnover;						///< 持仓金额
	data["AccountMarginMode"] = task_data.AccountMarginMode;				///< 保证金计算方式
	data["AccountMarginParam"] = task_data.AccountMarginParam;				///< 保证金参数值
	data["UpperMarginMode"] = task_data.UpperMarginMode;				///< 保证金计算方式
	data["UpperMarginParam"] = task_data.UpperMarginParam;				///< 保证金参数值
	data["AccountInitialMargin"] = task_data.AccountInitialMargin;			///< 客户初始保证金
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< 客户维持保证金
	data["UpperInitialMargin"] = task_data.UpperInitialMargin;				///< 上手初始保证金
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< 上手维持保证金
	data["PositionProfit"] = task_data.PositionProfit;					///< 持仓盈亏
	data["LMEPositionProfit"] = task_data.LMEPositionProfit;				///< LME持仓盈亏
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< 期权市值
	data["MatchCmbNo"] = task_data.MatchCmbNo;						///< 组合持仓号
	data["IsHistory"] = task_data.IsHistory;						///< 是否历史持仓
	data["FloatingPL"] = task_data.FloatingPL;						///< 逐笔浮盈
	this->onRtnPosition(data);
};

void TdApi::processRtnPositionProfit(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRtnPositionProfit(data);
};


///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------

int TdApi::createTapTraderApi(dict req)
{
#ifdef _DEBUG
	fopen_s(&fp, filename, "w");
	if (NULL == fp) {
		cout << "create debug file failed!" << endl;
		return -1;
	}
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif

	//取得API的版本信息
	cout << GetTapTradeAPIVersion() << endl;

	//创建API实例
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	memset(&stAppInfo, 0, sizeof(stAppInfo));
	getStr(req, "AuthCode", stAppInfo.AuthCode);
	getStr(req, "KeyOperationLogPath", stAppInfo.KeyOperationLogPath);
	api = CreateTapTradeAPI(&stAppInfo, iResult);
	if (NULL == api){
		cout << "创建API实例失败，错误码：" << iResult << endl;
		return -1;
	}

	//设定ITapAPINotify的实现类，用于异步消息的接收
	return api->SetAPINotify(this);
};


int TdApi::setHostAddress(string ip, int port)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->SetHostAddress(ip.c_str(), port);
}

int TdApi::reqUserLogin(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	//登录服务器
	TapAPITradeLoginAuth stLoginAuth;
	memset(&stLoginAuth, 0, sizeof(stLoginAuth));
	getStr(req, "UserID", stLoginAuth.UserNo);
	getStr(req, "Password", stLoginAuth.Password);
	stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
	stLoginAuth.ISDDA = APIYNFLAG_NO;
	return api->Login(&stLoginAuth);
}

int TdApi::exit()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->Disconnect();
}

int TdApi::qryAccount()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIAccQryReq stAccountQryReq;
	return api->QryAccount(&m_sessionID, &stAccountQryReq);
}

int TdApi::qryFund(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIFundReq stFundReq;
	memset(&stFundReq, 0, sizeof(stFundReq));
	getStr(req, "AccountNo", stFundReq.AccountNo);
	return api->QryFund(&m_sessionID, &stFundReq);
}

int TdApi::qryExchange()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->QryExchange(&m_sessionID);
}

int TdApi::qryCommodity()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->QryCommodity(&m_sessionID);
}

int TdApi::qryContract(dict req)
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

int TdApi::reqInsertOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPINewOrder stNewOrder;
	memset(&stNewOrder, 0, sizeof(TapAPINewOrder));
    getStr(req, "AccountNo", stNewOrder.AccountNo);						///< 客户资金帐号，必填
    getStr(req, "ExchangeNo", stNewOrder.ExchangeNo);						///< 交易所编号，必填
    getChar(req, "CommodityType",&stNewOrder.CommodityType);					///< 品种类型，必填
    getStr(req, "CommodityNo", stNewOrder.CommodityNo);					///< 品种编码类型，必填
    getStr(req, "ContractNo", stNewOrder.ContractNo);						///< 合约1，必填
    getStr(req, "StrikePrice", stNewOrder.StrikePrice);					///< 执行价格1，期权填写
    getChar(req, "CallOrPutFlag",&stNewOrder.CallOrPutFlag);					///< 看张看跌1 默认N
    getStr(req, "ContractNo2", stNewOrder.ContractNo2);					///< 合约2，默认空
    getStr(req, "StrikePrice2", stNewOrder.StrikePrice2);					///< 执行价格2，默认空
    getChar(req, "CallOrPutFlag2",&stNewOrder.CallOrPutFlag2);					///< 看张看跌2 默认N
    getChar(req, "OrderType",&stNewOrder.OrderType);						///< 委托类型 必填
    getChar(req, "OrderSource",&stNewOrder.OrderSource);					///< 委托来源，默认程序单。
    getChar(req, "TimeInForce",&stNewOrder.TimeInForce);					///< 委托有效类型,默认当日有效
    getStr(req, "ExpireTime", stNewOrder.ExpireTime);						///< 有效日期(GTD情况下使用)
    getChar(req, "IsRiskOrder",&stNewOrder.IsRiskOrder);					///< 是否风险报单，默认非风险保单
    getChar(req, "OrderSide",&stNewOrder.OrderSide);						///< 买入卖出
    getChar(req, "PositionEffect",&stNewOrder.PositionEffect);					///< 开平标志1,默认N
    getChar(req, "PositionEffect2",&stNewOrder.PositionEffect2);				///< 开平标志2，默认N
    getStr(req, "InquiryNo", stNewOrder.InquiryNo);						///< 询价号
	getChar(req, "HedgeFlag",&stNewOrder.HedgeFlag);						///< 投机保值，默认N
	getDouble(req, "OrderPrice", &stNewOrder.OrderPrice);						///< 委托价格
	getDouble(req, "StopPrice", &stNewOrder.StopPrice);						///< 触发价格
	getUInt(req, "OrderQty", &stNewOrder.OrderQty);						///< 委托数量
	getUInt(req, "OrderMinQty", &stNewOrder.OrderMinQty);					///< 最小成交量
	getUInt(req, "MinClipSize", &stNewOrder.MinClipSize);					///< 冰山单最小随机量
	getUInt(req, "MaxClipSize", &stNewOrder.MaxClipSize);					///< 冰山单最大随机量
	getInt(req, "RefInt", &stNewOrder.RefInt);							///< 整型参考值
	getStr(req, "RefString", stNewOrder.RefString);						///< 字符串参考值
	getChar(req, "TacticsType", &stNewOrder.TacticsType);					///< 策略单类型
	getChar(req, "TriggerCondition", &stNewOrder.TriggerCondition);				///< 触发条件
	getChar(req, "TriggerPriceType", &stNewOrder.TriggerPriceType);				///< 触发价格类型	
	getChar(req, "AddOneIsValid", &stNewOrder.AddOneIsValid);					///< 是否T+1有效
	getUInt(req, "OrderQty2", &stNewOrder.OrderQty2);						///< 委托数量2
	getChar(req, "HedgeFlag2", &stNewOrder.HedgeFlag2);						///< 投机保值2
	unsigned int  marketLevel;
	getUInt(req, "MarketLevel", &marketLevel);					///< 市价撮合深度
	stNewOrder.MarketLevel = marketLevel;
	getChar(req, "OrderDeleteByDisConnFlag", &stNewOrder.OrderDeleteByDisConnFlag);		///< 心跳检测失败时，服务器自动撤单标识
	getStr(req, "UpperChannelNo", stNewOrder.UpperChannelNo);					///< 上手通道号
	return api->InsertOrder(&m_sessionID, &stNewOrder);
}

int TdApi::reqCancelOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIOrderCancelReq stCancelReq;
	getInt(req, "RefInt", &stCancelReq.RefInt);							///< 整型参考值
	getStr(req, "RefString", stCancelReq.RefString);						///< 字符串参考值
	getChar(req, "ServerFlag", &stCancelReq.ServerFlag);						///< 服务器标识
	getStr(req, "OrderNo", stCancelReq.OrderNo);						///< 委托编码
	return api->CancelOrder(&m_sessionID, &stCancelReq);
}

int TdApi::qryOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIOrderQryReq stOrderQryReq;
	getChar(req, "OrderQryType", &stOrderQryReq.OrderQryType);						///< 资金账号
	return api->QryOrder(&m_sessionID, &stOrderQryReq);
}

int TdApi::qryFill()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIFillQryReq stFillQryReq;
	return api->QryFill(&m_sessionID, &stFillQryReq);
}

int TdApi::qryPosition(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIPositionQryReq stPositionReq;
	return api->QryPosition(&m_sessionID,&stPositionReq);
}


///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct TdApiWrap : TdApi, wrapper < TdApi >
{
	virtual	void onConnect() {
		try
		{
			this->get_override("onConnect")();
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspLogin(int errorCode, dict data)  {
		try
		{
			this->get_override("onRspLogin")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onExpriationDate(dict data) {
		try
		{
			this->get_override("onExpriationDate")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onAPIReady() {
		try
		{
			this->get_override("onAPIReady")();
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onDisconnect(int reasonCode) {
		try
		{
			this->get_override("onDisconnect")(reasonCode);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspChangePassword(int errorCode) {
		try
		{
			this->get_override("onRspChangePassword")(errorCode);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspSetReservedInfo(int errorCode, dict data) {
		try
		{
			this->get_override("onRspSetReservedInfo")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryAccount(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryAccount")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryFund(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryFund")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnFund(dict data) {
		try
		{
			this->get_override("onRtnFund")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryExchange(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryExchange")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryCommodity(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryCommodity")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryContract(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryContract")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnContract(dict data) {
		try
		{
			this->get_override("onRtnContract")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspOrderAction(int errorCode, dict data) {
		try
		{
			this->get_override("onRspOrderAction")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnOrder(dict data) {
		try
		{
			this->get_override("onRtnOrder")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryOrder(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryOrder")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryOrderProcess(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryOrderProcess")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryFill(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryFill")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnFill(dict data) {
		try
		{
			this->get_override("onRtnFill")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryPosition(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryPosition")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnPosition(dict data) {
		try
		{
			this->get_override("onRtnPosition")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnPositionProfit(dict data) {
		try
		{
			this->get_override("onRtnPositionProfit")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
};

BOOST_PYTHON_MODULE(vntapIntd)
{
	PyEval_InitThreads();	//导入时运行，保证先创建GIL

	class_<TdApiWrap, boost::noncopyable>("TdApi")
		.def("createTapInTraderApi", &TdApiWrap::createTapTraderApi)
		.def("setHostAddress", &TdApiWrap::setHostAddress)
		.def("reqUserLogin", &TdApiWrap::reqUserLogin)
		.def("exit", &TdApiWrap::exit)
		.def("qryAccount", &TdApiWrap::qryAccount)
		.def("qryFund", &TdApiWrap::qryFund)
		.def("qryExchange", &TdApiWrap::qryExchange)
		.def("qryCommodity", &TdApiWrap::qryCommodity)
		.def("qryContract", &TdApiWrap::qryContract)
		.def("reqInsertOrder", &TdApiWrap::reqInsertOrder)
		.def("reqCancelOrder", &TdApiWrap::reqCancelOrder)
		.def("qryOrder", &TdApiWrap::qryOrder)
		.def("qryFill", &TdApiWrap::qryFund)
		.def("qryPosition", &TdApiWrap::qryPosition)

		.def("onConnect", pure_virtual(&TdApiWrap::onConnect))
		.def("onRspLogin", pure_virtual(&TdApiWrap::onRspLogin))
		.def("onExpriationDate", pure_virtual(&TdApiWrap::onExpriationDate))
		.def("onAPIReady", pure_virtual(&TdApiWrap::onAPIReady))
		.def("onDisconnect", pure_virtual(&TdApiWrap::onDisconnect))
		.def("onRspChangePassword", pure_virtual(&TdApiWrap::onRspChangePassword))
		.def("onRspSetReservedInfo", pure_virtual(&TdApiWrap::onRspSetReservedInfo))
		.def("onRspQryAccount", pure_virtual(&TdApiWrap::onRspQryAccount))
		.def("onRspQryFund", pure_virtual(&TdApiWrap::onRspQryFund))
		.def("onRtnFund", pure_virtual(&TdApiWrap::onRtnFund))
		.def("onRspQryExchange", pure_virtual(&TdApiWrap::onRspQryExchange))
		.def("onRspQryCommodity", pure_virtual(&TdApiWrap::onRspQryCommodity))
		.def("onRspQryContract", pure_virtual(&TdApiWrap::onRspQryContract))
		.def("onRtnContract", pure_virtual(&TdApiWrap::onRtnContract))
		.def("onRspOrderAction", pure_virtual(&TdApiWrap::onRspOrderAction))
		.def("onRtnOrder", pure_virtual(&TdApiWrap::onRtnOrder))
		.def("onRspQryOrder", pure_virtual(&TdApiWrap::onRspQryOrder))
		.def("onRspQryOrderProcess", pure_virtual(&TdApiWrap::onRspQryOrderProcess))
		.def("onRspQryFill", pure_virtual(&TdApiWrap::onRspQryFill))
		.def("onRtnFill", pure_virtual(&TdApiWrap::onRtnFill))
		.def("onRspQryPosition", pure_virtual(&TdApiWrap::onRspQryPosition))
		.def("onRtnPosition", pure_virtual(&TdApiWrap::onRtnPosition))
		.def("onRtnPositionProfit", pure_virtual(&TdApiWrap::onRtnPositionProfit))
		;
}

