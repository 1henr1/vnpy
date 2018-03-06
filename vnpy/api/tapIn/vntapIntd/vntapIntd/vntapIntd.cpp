// vntapIntd.cpp : ���� DLL Ӧ�ó���ĵ���������
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
///��Python����C++����ת���õĺ���
///-------------------------------------------------------------------------------------

void getInt(dict d, string key, int *value)
{
	if (d.has_key(key))		//����ֵ����Ƿ���ڸü�ֵ
	{
		object o = d[key];	//��ȡ�ü�ֵ
		extract<int> x(o);	//������ȡ��
		if (x.check())		//���������ȡ
		{
			*value = x();	//��Ŀ������ָ�븳ֵ
		}
	}
}

void getUInt(dict d, string key, unsigned int *value)
{
	if (d.has_key(key))		//����ֵ����Ƿ���ڸü�ֵ
	{
		object o = d[key];	//��ȡ�ü�ֵ
		extract<unsigned int> x(o);	//������ȡ��
		if (x.check())		//���������ȡ
		{
			*value = x();	//��Ŀ������ָ�븳ֵ
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
			//���ַ���ָ�븳ֵ����ʹ��strcpy_s, vs2013ʹ��strcpy����ͨ����
			//+1Ӧ������ΪC++�ַ����Ľ�β���ţ������ر�ȷ�����������1�����
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
///C++�Ļص����������ݱ��浽������
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
///�����̴߳Ӷ�����ȡ�����ݣ�ת��Ϊpython����󣬽�������
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
	data["UserNo"] = task_data.UserNo;							///< �û����
	data["UserType"] = task_data.UserType;						///< �û�����
	data["UserName"] = task_data.UserName;						///< �û���
	data["ReservedInfo"] = task_data.ReservedInfo;					///< Ԥ����Ϣ
	data["LastLoginIP"] = task_data.LastLoginIP;					///< �ϴε�¼IP
	data["LastLoginProt"] = task_data.LastLoginProt;					///< �ϴε�¼�˿�
	data["LastLoginTime"] = task_data.LastLoginTime;					///< �ϴε�¼ʱ��
	data["LastLogoutTime"] = task_data.LastLogoutTime;					///< �ϴ��˳�ʱ��
	data["TradeDate"] = task_data.TradeDate;						///< ��ǰ��������
	data["LastSettleTime"] = task_data.LastSettleTime;					///< �ϴν���ʱ��
	data["StartTime"] = task_data.StartTime;						///< ϵͳ����ʱ��
	data["InitTime"] = task_data.InitTime;						///< ϵͳ��ʼ��ʱ��
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
	data["AccountNo"] = task_data.AccountNo;                              ///< �ʽ��˺�
	data["AccountType"] = task_data.AccountType;                            ///< �˺�����
	data["AccountState"] = task_data.AccountState;                           ///< �˺�״̬
	data["AccountShortName"] = task_data.AccountShortName;                       ///< �˺ż��
	this->onRspQryAccount(task.task_id, task.task_last, data);
};

void TdApi::processRspQryFund(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFundData task_data = any_cast<TapAPIFundData>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��˺�
	data["CurrencyGroupNo"] = task_data.CurrencyGroupNo;				///< �������
	data["CurrencyNo"] = task_data.CurrencyNo;						///< ���ֺ�(Ϊ�ձ�ʾ����������ʽ�)
	data["TradeRate"] = task_data.TradeRate;						///< ���׻���
	data["FutureAlg"] = task_data.FutureAlg;                      ///< �ڻ��㷨
	data["OptionAlg"] = task_data.OptionAlg;                      ///< ��Ȩ�㷨
	data["PreBalance"] = task_data.PreBalance;						///< ���ս��
	data["PreUnExpProfit"] = task_data.PreUnExpProfit;					///< ����δ����ƽӯ
	data["PreLMEPositionProfit"] = task_data.PreLMEPositionProfit;			///< ����LME�ֲ�ƽӯ
	data["PreEquity"] = task_data.PreEquity;						///< ����Ȩ��
	data["PreAvailable1"] = task_data.PreAvailable1;					///< ���տ���
	data["PreMarketEquity"] = task_data.PreMarketEquity;				///< ������ֵȨ��
	data["CashInValue"] = task_data.CashInValue;					///< ���
	data["CashOutValue"] = task_data.CashOutValue;					///< ����
	data["CashAdjustValue"] = task_data.CashAdjustValue;				///< �ʽ����
	data["CashPledged"] = task_data.CashPledged;					///< ��Ѻ�ʽ�
	data["FrozenFee"] = task_data.FrozenFee;						///< ����������
	data["FrozenDeposit"] = task_data.FrozenDeposit;					///< ���ᱣ֤��
	data["AccountFee"] = task_data.AccountFee;						///< �ͻ������Ѱ�������������
	data["ExchangeFee"] = task_data.ExchangeFee;					///< ���������
	data["AccountDeliveryFee"] = task_data.AccountDeliveryFee;				///< �ͻ�����������
	data["PremiumIncome"] = task_data.PremiumIncome;					///< Ȩ������ȡ
	data["PremiumPay"] = task_data.PremiumPay;						///< Ȩ����֧��
	data["CloseProfit"] = task_data.CloseProfit;					///< ƽ��ӯ��
	data["DeliveryProfit"] = task_data.DeliveryProfit;					///< ����ӯ��
	data["UnExpProfit"] = task_data.UnExpProfit;					///< δ����ƽӯ
	data["ExpProfit"] = task_data.ExpProfit;						///< ����ƽ��ӯ��
	data["PositionProfit"] = task_data.PositionProfit;					///< ����LME�ֲ�ӯ��
	data["LmePositionProfit"] = task_data.LmePositionProfit;				///< LME�ֲ�ӯ��
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< ��Ȩ��ֵ
	data["AccountIntialMargin"] = task_data.AccountIntialMargin;			///< �ͻ���ʼ��֤��
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
	data["UpperInitalMargin"] = task_data.UpperInitalMargin;				///< ���ֳ�ʼ��֤��
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
	data["Discount"] = task_data.Discount;						///< LME����
	data["Balance"] = task_data.Balance;						///< ���ս��
	data["Equity"] = task_data.Equity;							///< ����Ȩ��
	data["Available"] = task_data.Available;						///< ���տ���
	data["CanDraw"] = task_data.CanDraw;						///< ����ȡ
	data["MarketEquity"] = task_data.MarketEquity;					///< �˻���ֵ
	this->onRspQryFund(task.task_id, task.task_last, data);
};

void TdApi::processRtnFund(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFundData task_data = any_cast<TapAPIFundData>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��˺�
	data["CurrencyGroupNo"] = task_data.CurrencyGroupNo;				///< �������
	data["CurrencyNo"] = task_data.CurrencyNo;						///< ���ֺ�(Ϊ�ձ�ʾ����������ʽ�)
	data["TradeRate"] = task_data.TradeRate;						///< ���׻���
	data["FutureAlg"] = task_data.FutureAlg;                      ///< �ڻ��㷨
	data["OptionAlg"] = task_data.OptionAlg;                      ///< ��Ȩ�㷨
	data["PreBalance"] = task_data.PreBalance;						///< ���ս��
	data["PreUnExpProfit"] = task_data.PreUnExpProfit;					///< ����δ����ƽӯ
	data["PreLMEPositionProfit"] = task_data.PreLMEPositionProfit;			///< ����LME�ֲ�ƽӯ
	data["PreEquity"] = task_data.PreEquity;						///< ����Ȩ��
	data["PreAvailable1"] = task_data.PreAvailable1;					///< ���տ���
	data["PreMarketEquity"] = task_data.PreMarketEquity;				///< ������ֵȨ��
	data["CashInValue"] = task_data.CashInValue;					///< ���
	data["CashOutValue"] = task_data.CashOutValue;					///< ����
	data["CashAdjustValue"] = task_data.CashAdjustValue;				///< �ʽ����
	data["CashPledged"] = task_data.CashPledged;					///< ��Ѻ�ʽ�
	data["FrozenFee"] = task_data.FrozenFee;						///< ����������
	data["FrozenDeposit"] = task_data.FrozenDeposit;					///< ���ᱣ֤��
	data["AccountFee"] = task_data.AccountFee;						///< �ͻ������Ѱ�������������
	data["ExchangeFee"] = task_data.ExchangeFee;					///< ���������
	data["AccountDeliveryFee"] = task_data.AccountDeliveryFee;				///< �ͻ�����������
	data["PremiumIncome"] = task_data.PremiumIncome;					///< Ȩ������ȡ
	data["PremiumPay"] = task_data.PremiumPay;						///< Ȩ����֧��
	data["CloseProfit"] = task_data.CloseProfit;					///< ƽ��ӯ��
	data["DeliveryProfit"] = task_data.DeliveryProfit;					///< ����ӯ��
	data["UnExpProfit"] = task_data.UnExpProfit;					///< δ����ƽӯ
	data["ExpProfit"] = task_data.ExpProfit;						///< ����ƽ��ӯ��
	data["PositionProfit"] = task_data.PositionProfit;					///< ����LME�ֲ�ӯ��
	data["LmePositionProfit"] = task_data.LmePositionProfit;				///< LME�ֲ�ӯ��
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< ��Ȩ��ֵ
	data["AccountIntialMargin"] = task_data.AccountIntialMargin;			///< �ͻ���ʼ��֤��
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
	data["UpperInitalMargin"] = task_data.UpperInitalMargin;				///< ���ֳ�ʼ��֤��
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
	data["Discount"] = task_data.Discount;						///< LME����
	data["Balance"] = task_data.Balance;						///< ���ս��
	data["Equity"] = task_data.Equity;							///< ����Ȩ��
	data["Available"] = task_data.Available;						///< ���տ���
	data["CanDraw"] = task_data.CanDraw;						///< ����ȡ
	data["MarketEquity"] = task_data.MarketEquity;					///< �˻���ֵ
	this->onRtnFund(data);
};

void TdApi::processRspQryExchange(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIExchangeInfo task_data = any_cast<TapAPIExchangeInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;								///< ����������
	data["ExchangeName"] = task_data.ExchangeName;							///< ����������
	this->onRspQryExchange(task.task_id, task.task_last, data);
};

void TdApi::processRspQryCommodity(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPICommodityInfo task_data = any_cast<TapAPICommodityInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;						//����������
	data["CommodityType"] = task_data.CommodityType;					//Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					//Ʒ�ֱ��
	data["CommodityName"] = task_data.CommodityName;					//Ʒ������
	data["CommodityEngName"] = task_data.CommodityEngName;				//Ʒ��Ӣ������
	data["TradeCurrency"] = task_data.TradeCurrency;					//���ױ���
	data["ContractSize"] = task_data.ContractSize;					//ÿ�ֳ���
	data["OpenCloseMode"] = task_data.OpenCloseMode;					//��ƽ��ʽ
	data["StrikePriceTimes"] = task_data.StrikePriceTimes;				//ִ�м۸���
	data["CommodityTickSize"] = task_data.CommodityTickSize;				//��С�䶯��λ
	data["CommodityDenominator"] = task_data.CommodityDenominator;			//���۷�ĸ
	data["CmbDirect"] = task_data.CmbDirect;						//��Ϸ���
	data["DeliveryMode"] = task_data.DeliveryMode;					//������Ȩ��ʽ
	data["DeliveryDays"] = task_data.DeliveryDays;					//������ƫ��
	data["AddOneTime"] = task_data.AddOneTime;						//T+1�ָ�ʱ��
	data["CommodityTimeZone"] = task_data.CommodityTimeZone;				//Ʒ��ʱ��

	this->onRspQryCommodity(task.task_id, task.task_last, data);
};

void TdApi::processRspQryContract(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPITradeContractInfo task_data = any_cast<TapAPITradeContractInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;                             ///< ����������
	data["CommodityType"] = task_data.CommodityType;                          ///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;                            ///< Ʒ�ֱ��
	data["ContractNo1"] = task_data.ContractNo1;                            ///< ��Լ����1
	data["StrikePrice1"] = task_data.StrikePrice1;                           ///< ִ�м�1
	data["CallOrPutFlag1"] = task_data.CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	data["ContractNo2"] = task_data.ContractNo2;                            ///< ��Լ����2
	data["StrikePrice2"] = task_data.StrikePrice2;                           ///< ִ�м�2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;                         ///< ���ǿ�����ʾ2
	data["ContractType"] = task_data.ContractType;                           ///< ��Լ����
	data["QuoteUnderlyingContract"] = task_data.QuoteUnderlyingContract;				///< ������ʵ��Լ
	data["ContractName"] = task_data.ContractName;                           ///< ��Լ����
	data["ContractExpDate"] = task_data.ContractExpDate;                        ///< ��Լ������	
	data["LastTradeDate"] = task_data.LastTradeDate;                          ///< �������
	data["FirstNoticeDate"] = task_data.FirstNoticeDate;                        ///< �״�֪ͨ��
	this->onRspQryContract(task.task_id, task.task_last, data);
};

void TdApi::processRtnContract(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPITradeContractInfo task_data = any_cast<TapAPITradeContractInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;                             ///< ����������
	data["CommodityType"] = task_data.CommodityType;                          ///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;                            ///< Ʒ�ֱ��
	data["ContractNo1"] = task_data.ContractNo1;                            ///< ��Լ����1
	data["StrikePrice1"] = task_data.StrikePrice1;                           ///< ִ�м�1
	data["CallOrPutFlag1"] = task_data.CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	data["ContractNo2"] = task_data.ContractNo2;                            ///< ��Լ����2
	data["StrikePrice2"] = task_data.StrikePrice2;                           ///< ִ�м�2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;                         ///< ���ǿ�����ʾ2
	data["ContractType"] = task_data.ContractType;                           ///< ��Լ����
	data["QuoteUnderlyingContract"] = task_data.QuoteUnderlyingContract;				///< ������ʵ��Լ
	data["ContractName"] = task_data.ContractName;                           ///< ��Լ����
	data["ContractExpDate"] = task_data.ContractExpDate;                        ///< ��Լ������	
	data["LastTradeDate"] = task_data.LastTradeDate;                          ///< �������
	data["FirstNoticeDate"] = task_data.FirstNoticeDate;                        ///< �״�֪ͨ��
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
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
	data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
	data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�1
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���1
	data["ContractNo2"] = task_data.ContractNo2;					///< ��Լ2
	data["StrikePrice2"] = task_data.StrikePrice2;					///< ִ�м۸�2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;					///< ���ſ���2
	data["OrderType"] = task_data.OrderType;						///< ί������
	data["OrderSource"] = task_data.OrderSource;					///< ί����Դ
	data["TimeInForce"] = task_data.TimeInForce;					///< ί����Ч����
	data["ExpireTime"] = task_data.ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< �Ƿ���ձ���
	data["OrderSide"] = task_data.OrderSide;						///< ��������
	data["PositionEffect"] = task_data.PositionEffect;					///< ��ƽ��־1
	data["PositionEffect2"] = task_data.PositionEffect2;				///< ��ƽ��־2
	data["InquiryNo"] = task_data.InquiryNo;						///< ѯ�ۺ�
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["OrderPrice"] = task_data.OrderPrice;						///< ί�м۸�1
	data["OrderPrice2"] = task_data.OrderPrice2;					///< ί�м۸�2��������Ӧ��ʹ��
	data["StopPrice"] = task_data.StopPrice;						///< �����۸�
	data["OrderQty"] = task_data.OrderQty;						///< ί������
	data["OrderMinQty"] = task_data.OrderMinQty;					///< ��С�ɽ���
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;                                      ///< �ַ����ο�ֵ
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["OrderInsertUserNo"] = task_data.OrderInsertUserNo;				///< �µ���
	data["OrderInsertTime"] = task_data.OrderInsertTime;				///< �µ�ʱ��
	data["OrderCommandUserNo"] = task_data.OrderCommandUserNo;				///< ¼��������
	data["OrderUpdateUserNo"] = task_data.OrderUpdateUserNo;				///< ί�и�����
	data["OrderUpdateTime"] = task_data.OrderUpdateTime;				///< ί�и���ʱ��
	data["OrderState"] = task_data.OrderState;						///< ί��״̬
	data["OrderMatchPrice"] = task_data.OrderMatchPrice;				///< �ɽ���1
	data["OrderMatchPrice2"] = task_data.OrderMatchPrice2;				///< �ɽ���2
	data["OrderMatchQty"] = task_data.OrderMatchQty;					///< �ɽ���1
	data["OrderMatchQty2"] = task_data.OrderMatchQty2;					///< �ɽ���2
	data["ErrorCode"] = task_data.ErrorCode;						///< ���һ�β���������Ϣ��
	data["ErrorText"] = task_data.ErrorText;						///< ������Ϣ
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsDeleted"] = task_data.IsDeleted;						///< ί�гɽ�ɾ����
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
	this->onRspOrderAction(task.task_id, data);
};

void TdApi::processRtnOrder(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIOrderInfo task_data = any_cast<TapAPIOrderInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
	data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
	data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�1
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���1
	data["ContractNo2"] = task_data.ContractNo2;					///< ��Լ2
	data["StrikePrice2"] = task_data.StrikePrice2;					///< ִ�м۸�2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;					///< ���ſ���2
	data["OrderType"] = task_data.OrderType;						///< ί������
	data["OrderSource"] = task_data.OrderSource;					///< ί����Դ
	data["TimeInForce"] = task_data.TimeInForce;					///< ί����Ч����
	data["ExpireTime"] = task_data.ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< �Ƿ���ձ���
	data["OrderSide"] = task_data.OrderSide;						///< ��������
	data["PositionEffect"] = task_data.PositionEffect;					///< ��ƽ��־1
	data["PositionEffect2"] = task_data.PositionEffect2;				///< ��ƽ��־2
	data["InquiryNo"] = task_data.InquiryNo;						///< ѯ�ۺ�
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["OrderPrice"] = task_data.OrderPrice;						///< ί�м۸�1
	data["OrderPrice2"] = task_data.OrderPrice2;					///< ί�м۸�2��������Ӧ��ʹ��
	data["StopPrice"] = task_data.StopPrice;						///< �����۸�
	data["OrderQty"] = task_data.OrderQty;						///< ί������
	data["OrderMinQty"] = task_data.OrderMinQty;					///< ��С�ɽ���
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;                                      ///< �ַ����ο�ֵ
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["OrderInsertUserNo"] = task_data.OrderInsertUserNo;				///< �µ���
	data["OrderInsertTime"] = task_data.OrderInsertTime;				///< �µ�ʱ��
	data["OrderCommandUserNo"] = task_data.OrderCommandUserNo;				///< ¼��������
	data["OrderUpdateUserNo"] = task_data.OrderUpdateUserNo;				///< ί�и�����
	data["OrderUpdateTime"] = task_data.OrderUpdateTime;				///< ί�и���ʱ��
	data["OrderState"] = task_data.OrderState;						///< ί��״̬
	data["OrderMatchPrice"] = task_data.OrderMatchPrice;				///< �ɽ���1
	data["OrderMatchPrice2"] = task_data.OrderMatchPrice2;				///< �ɽ���2
	data["OrderMatchQty"] = task_data.OrderMatchQty;					///< �ɽ���1
	data["OrderMatchQty2"] = task_data.OrderMatchQty2;					///< �ɽ���2
	data["ErrorCode"] = task_data.ErrorCode;						///< ���һ�β���������Ϣ��
	data["ErrorText"] = task_data.ErrorText;						///< ������Ϣ
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsDeleted"] = task_data.IsDeleted;						///< ί�гɽ�ɾ����
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
	this->onRtnOrder(data);
};

void TdApi::processRspQryOrder(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIOrderInfo task_data = any_cast<TapAPIOrderInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
	data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
	data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�1
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���1
	data["ContractNo2"] = task_data.ContractNo2;					///< ��Լ2
	data["StrikePrice2"] = task_data.StrikePrice2;					///< ִ�м۸�2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;					///< ���ſ���2
	data["OrderType"] = task_data.OrderType;						///< ί������
	data["OrderSource"] = task_data.OrderSource;					///< ί����Դ
	data["TimeInForce"] = task_data.TimeInForce;					///< ί����Ч����
	data["ExpireTime"] = task_data.ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< �Ƿ���ձ���
	data["OrderSide"] = task_data.OrderSide;						///< ��������
	data["PositionEffect"] = task_data.PositionEffect;					///< ��ƽ��־1
	data["PositionEffect2"] = task_data.PositionEffect2;				///< ��ƽ��־2
	data["InquiryNo"] = task_data.InquiryNo;						///< ѯ�ۺ�
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["OrderPrice"] = task_data.OrderPrice;						///< ί�м۸�1
	data["OrderPrice2"] = task_data.OrderPrice2;					///< ί�м۸�2��������Ӧ��ʹ��
	data["StopPrice"] = task_data.StopPrice;						///< �����۸�
	data["OrderQty"] = task_data.OrderQty;						///< ί������
	data["OrderMinQty"] = task_data.OrderMinQty;					///< ��С�ɽ���
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;                                      ///< �ַ����ο�ֵ
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["OrderInsertUserNo"] = task_data.OrderInsertUserNo;				///< �µ���
	data["OrderInsertTime"] = task_data.OrderInsertTime;				///< �µ�ʱ��
	data["OrderCommandUserNo"] = task_data.OrderCommandUserNo;				///< ¼��������
	data["OrderUpdateUserNo"] = task_data.OrderUpdateUserNo;				///< ί�и�����
	data["OrderUpdateTime"] = task_data.OrderUpdateTime;				///< ί�и���ʱ��
	data["OrderState"] = task_data.OrderState;						///< ί��״̬
	data["OrderMatchPrice"] = task_data.OrderMatchPrice;				///< �ɽ���1
	data["OrderMatchPrice2"] = task_data.OrderMatchPrice2;				///< �ɽ���2
	data["OrderMatchQty"] = task_data.OrderMatchQty;					///< �ɽ���1
	data["OrderMatchQty2"] = task_data.OrderMatchQty2;					///< �ɽ���2
	data["ErrorCode"] = task_data.ErrorCode;						///< ���һ�β���������Ϣ��
	data["ErrorText"] = task_data.ErrorText;						///< ������Ϣ
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsDeleted"] = task_data.IsDeleted;						///< ί�гɽ�ɾ����
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
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
    data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
    data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
    data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
    data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
    data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
    data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�
    data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���
    data["MatchSource"] = task_data.MatchSource;					///< ί����Դ
    data["MatchSide"] = task_data.MatchSide;						///< ��������
    data["PositionEffect"] = task_data.PositionEffect;					///< ��ƽ��־1
    data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
    data["OrderNo"] = task_data.OrderNo;						///< ί�б���
    data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
    data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< �������ɽ���
    data["MatchDateTime"] = task_data.MatchDateTime;					///< �ɽ�ʱ��
    data["UpperMatchDateTime"] = task_data.UpperMatchDateTime;				///< ���ֳɽ�ʱ��
    data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
    data["MatchPrice"] = task_data.MatchPrice;						///< �ɽ���
    data["MatchQty"] = task_data.MatchQty;						///< �ɽ���
    data["IsDeleted"] = task_data.IsDeleted;						///< ί�гɽ�ɾ����
    data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
    data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
    data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
    data["FeeValue"] = task_data.FeeValue;						///< ������
    data["IsManualFee"] = task_data.IsManualFee;					///< �˹��ͻ������ѱ��
	this->onRtnFill(data);
};

void TdApi::processRspQryPosition(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIPositionInfo task_data = any_cast<TapAPIPositionInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
	data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
	data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���
	data["ExchangeNoRef"] = task_data.ExchangeNoRef;					///< ���������
	data["CommodityTypeRef"] = task_data.CommodityTypeRef;				///< Ʒ������
	data["CommodityNoRef"] = task_data.CommodityNoRef;					///< Ʒ�ֱ�������
	data["ContractNoRef"] = task_data.ContractNoRef;					///< ��Լ
	data["StrikePriceRef"] = task_data.StrikePriceRef;					///< ִ�м۸�
	data["CallOrPutFlagRef"] = task_data.CallOrPutFlagRef;				///< ���ſ���
	data["OrderExchangeNo"] = task_data.OrderExchangeNo;				///< ���������
	data["OrderCommodityType"] = task_data.OrderCommodityType;				///< Ʒ������
	data["OrderCommodityNo"] = task_data.OrderCommodityNo;				///< Ʒ�ֱ�������
	data["OrderType"] = task_data.OrderType;						///< ί������
	data["MatchSource"] = task_data.MatchSource;					///< ί����Դ
	data["TimeInForce"] = task_data.TimeInForce;					///< ί����Ч����
	data["ExpireTime"] = task_data.ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	data["MatchSide"] = task_data.MatchSide;						///< ��������
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["PositionNo"] = task_data.PositionNo;						///< ���سֲֺţ���������д
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
	data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< �������ɽ���
	data["MatchDate"] = task_data.MatchDate;						///< �ɽ�����
	data["MatchTime"] = task_data.MatchTime;						///< �ɽ�ʱ��
	data["UpperMatchTime"] = task_data.UpperMatchTime;					///< ���ֳɽ�ʱ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź��������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["PositionPrice"] = task_data.PositionPrice;					///< �ֲּ�
	data["PositionQty"] = task_data.PositionQty;					///< �ֲ���
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
	data["MatchStreamID"] = task_data.MatchStreamID;					///< �ɽ���ˮ��
	data["PositionStreamId"] = task_data.PositionStreamId;				///< �ֲ�����
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< ��ƽ��ʽ
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������
	data["SettlePrice"] = task_data.SettlePrice;					///< ��ǰ����۸�
	data["Turnover"] = task_data.Turnover;						///< �ֲֽ��
	data["AccountMarginMode"] = task_data.AccountMarginMode;				///< ��֤����㷽ʽ
	data["AccountMarginParam"] = task_data.AccountMarginParam;				///< ��֤�����ֵ
	data["UpperMarginMode"] = task_data.UpperMarginMode;				///< ��֤����㷽ʽ
	data["UpperMarginParam"] = task_data.UpperMarginParam;				///< ��֤�����ֵ
	data["AccountInitialMargin"] = task_data.AccountInitialMargin;			///< �ͻ���ʼ��֤��
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
	data["UpperInitialMargin"] = task_data.UpperInitialMargin;				///< ���ֳ�ʼ��֤��
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
	data["PositionProfit"] = task_data.PositionProfit;					///< �ֲ�ӯ��
	data["LMEPositionProfit"] = task_data.LMEPositionProfit;				///< LME�ֲ�ӯ��
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< ��Ȩ��ֵ
	data["MatchCmbNo"] = task_data.MatchCmbNo;						///< ��ϳֲֺ�
	data["IsHistory"] = task_data.IsHistory;						///< �Ƿ���ʷ�ֲ�
	data["FloatingPL"] = task_data.FloatingPL;						///< ��ʸ�ӯ
	this->onRspQryPosition(task.task_id, task.task_last, data);
};

void TdApi::processRtnPosition(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIPositionInfo task_data = any_cast<TapAPIPositionInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
	data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
	data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���
	data["ExchangeNoRef"] = task_data.ExchangeNoRef;					///< ���������
	data["CommodityTypeRef"] = task_data.CommodityTypeRef;				///< Ʒ������
	data["CommodityNoRef"] = task_data.CommodityNoRef;					///< Ʒ�ֱ�������
	data["ContractNoRef"] = task_data.ContractNoRef;					///< ��Լ
	data["StrikePriceRef"] = task_data.StrikePriceRef;					///< ִ�м۸�
	data["CallOrPutFlagRef"] = task_data.CallOrPutFlagRef;				///< ���ſ���
	data["OrderExchangeNo"] = task_data.OrderExchangeNo;				///< ���������
	data["OrderCommodityType"] = task_data.OrderCommodityType;				///< Ʒ������
	data["OrderCommodityNo"] = task_data.OrderCommodityNo;				///< Ʒ�ֱ�������
	data["OrderType"] = task_data.OrderType;						///< ί������
	data["MatchSource"] = task_data.MatchSource;					///< ί����Դ
	data["TimeInForce"] = task_data.TimeInForce;					///< ί����Ч����
	data["ExpireTime"] = task_data.ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	data["MatchSide"] = task_data.MatchSide;						///< ��������
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["PositionNo"] = task_data.PositionNo;						///< ���سֲֺţ���������д
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
	data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< �������ɽ���
	data["MatchDate"] = task_data.MatchDate;						///< �ɽ�����
	data["MatchTime"] = task_data.MatchTime;						///< �ɽ�ʱ��
	data["UpperMatchTime"] = task_data.UpperMatchTime;					///< ���ֳɽ�ʱ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź��������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["PositionPrice"] = task_data.PositionPrice;					///< �ֲּ�
	data["PositionQty"] = task_data.PositionQty;					///< �ֲ���
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
	data["MatchStreamID"] = task_data.MatchStreamID;					///< �ɽ���ˮ��
	data["PositionStreamId"] = task_data.PositionStreamId;				///< �ֲ�����
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< ��ƽ��ʽ
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������
	data["SettlePrice"] = task_data.SettlePrice;					///< ��ǰ����۸�
	data["Turnover"] = task_data.Turnover;						///< �ֲֽ��
	data["AccountMarginMode"] = task_data.AccountMarginMode;				///< ��֤����㷽ʽ
	data["AccountMarginParam"] = task_data.AccountMarginParam;				///< ��֤�����ֵ
	data["UpperMarginMode"] = task_data.UpperMarginMode;				///< ��֤����㷽ʽ
	data["UpperMarginParam"] = task_data.UpperMarginParam;				///< ��֤�����ֵ
	data["AccountInitialMargin"] = task_data.AccountInitialMargin;			///< �ͻ���ʼ��֤��
	data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
	data["UpperInitialMargin"] = task_data.UpperInitialMargin;				///< ���ֳ�ʼ��֤��
	data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
	data["PositionProfit"] = task_data.PositionProfit;					///< �ֲ�ӯ��
	data["LMEPositionProfit"] = task_data.LMEPositionProfit;				///< LME�ֲ�ӯ��
	data["OptionMarketValue"] = task_data.OptionMarketValue;				///< ��Ȩ��ֵ
	data["MatchCmbNo"] = task_data.MatchCmbNo;						///< ��ϳֲֺ�
	data["IsHistory"] = task_data.IsHistory;						///< �Ƿ���ʷ�ֲ�
	data["FloatingPL"] = task_data.FloatingPL;						///< ��ʸ�ӯ
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
///��������
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

	//ȡ��API�İ汾��Ϣ
	cout << GetTapTradeAPIVersion() << endl;

	//����APIʵ��
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	memset(&stAppInfo, 0, sizeof(stAppInfo));
	getStr(req, "AuthCode", stAppInfo.AuthCode);
	getStr(req, "KeyOperationLogPath", stAppInfo.KeyOperationLogPath);
	api = CreateTapTradeAPI(&stAppInfo, iResult);
	if (NULL == api){
		cout << "����APIʵ��ʧ�ܣ������룺" << iResult << endl;
		return -1;
	}

	//�趨ITapAPINotify��ʵ���࣬�����첽��Ϣ�Ľ���
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
	//��¼������
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
    getStr(req, "AccountNo", stNewOrder.AccountNo);						///< �ͻ��ʽ��ʺţ�����
    getStr(req, "ExchangeNo", stNewOrder.ExchangeNo);						///< ��������ţ�����
    getChar(req, "CommodityType",&stNewOrder.CommodityType);					///< Ʒ�����ͣ�����
    getStr(req, "CommodityNo", stNewOrder.CommodityNo);					///< Ʒ�ֱ������ͣ�����
    getStr(req, "ContractNo", stNewOrder.ContractNo);						///< ��Լ1������
    getStr(req, "StrikePrice", stNewOrder.StrikePrice);					///< ִ�м۸�1����Ȩ��д
    getChar(req, "CallOrPutFlag",&stNewOrder.CallOrPutFlag);					///< ���ſ���1 Ĭ��N
    getStr(req, "ContractNo2", stNewOrder.ContractNo2);					///< ��Լ2��Ĭ�Ͽ�
    getStr(req, "StrikePrice2", stNewOrder.StrikePrice2);					///< ִ�м۸�2��Ĭ�Ͽ�
    getChar(req, "CallOrPutFlag2",&stNewOrder.CallOrPutFlag2);					///< ���ſ���2 Ĭ��N
    getChar(req, "OrderType",&stNewOrder.OrderType);						///< ί������ ����
    getChar(req, "OrderSource",&stNewOrder.OrderSource);					///< ί����Դ��Ĭ�ϳ��򵥡�
    getChar(req, "TimeInForce",&stNewOrder.TimeInForce);					///< ί����Ч����,Ĭ�ϵ�����Ч
    getStr(req, "ExpireTime", stNewOrder.ExpireTime);						///< ��Ч����(GTD�����ʹ��)
    getChar(req, "IsRiskOrder",&stNewOrder.IsRiskOrder);					///< �Ƿ���ձ�����Ĭ�ϷǷ��ձ���
    getChar(req, "OrderSide",&stNewOrder.OrderSide);						///< ��������
    getChar(req, "PositionEffect",&stNewOrder.PositionEffect);					///< ��ƽ��־1,Ĭ��N
    getChar(req, "PositionEffect2",&stNewOrder.PositionEffect2);				///< ��ƽ��־2��Ĭ��N
    getStr(req, "InquiryNo", stNewOrder.InquiryNo);						///< ѯ�ۺ�
	getChar(req, "HedgeFlag",&stNewOrder.HedgeFlag);						///< Ͷ����ֵ��Ĭ��N
	getDouble(req, "OrderPrice", &stNewOrder.OrderPrice);						///< ί�м۸�
	getDouble(req, "StopPrice", &stNewOrder.StopPrice);						///< �����۸�
	getUInt(req, "OrderQty", &stNewOrder.OrderQty);						///< ί������
	getUInt(req, "OrderMinQty", &stNewOrder.OrderMinQty);					///< ��С�ɽ���
	getUInt(req, "MinClipSize", &stNewOrder.MinClipSize);					///< ��ɽ����С�����
	getUInt(req, "MaxClipSize", &stNewOrder.MaxClipSize);					///< ��ɽ����������
	getInt(req, "RefInt", &stNewOrder.RefInt);							///< ���Ͳο�ֵ
	getStr(req, "RefString", stNewOrder.RefString);						///< �ַ����ο�ֵ
	getChar(req, "TacticsType", &stNewOrder.TacticsType);					///< ���Ե�����
	getChar(req, "TriggerCondition", &stNewOrder.TriggerCondition);				///< ��������
	getChar(req, "TriggerPriceType", &stNewOrder.TriggerPriceType);				///< �����۸�����	
	getChar(req, "AddOneIsValid", &stNewOrder.AddOneIsValid);					///< �Ƿ�T+1��Ч
	getUInt(req, "OrderQty2", &stNewOrder.OrderQty2);						///< ί������2
	getChar(req, "HedgeFlag2", &stNewOrder.HedgeFlag2);						///< Ͷ����ֵ2
	unsigned int  marketLevel;
	getUInt(req, "MarketLevel", &marketLevel);					///< �м۴�����
	stNewOrder.MarketLevel = marketLevel;
	getChar(req, "OrderDeleteByDisConnFlag", &stNewOrder.OrderDeleteByDisConnFlag);		///< �������ʧ��ʱ���������Զ�������ʶ
	getStr(req, "UpperChannelNo", stNewOrder.UpperChannelNo);					///< ����ͨ����
	return api->InsertOrder(&m_sessionID, &stNewOrder);
}

int TdApi::reqCancelOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIOrderCancelReq stCancelReq;
	getInt(req, "RefInt", &stCancelReq.RefInt);							///< ���Ͳο�ֵ
	getStr(req, "RefString", stCancelReq.RefString);						///< �ַ����ο�ֵ
	getChar(req, "ServerFlag", &stCancelReq.ServerFlag);						///< ��������ʶ
	getStr(req, "OrderNo", stCancelReq.OrderNo);						///< ί�б���
	return api->CancelOrder(&m_sessionID, &stCancelReq);
}

int TdApi::qryOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIOrderQryReq stOrderQryReq;
	getChar(req, "OrderQryType", &stOrderQryReq.OrderQryType);						///< �ʽ��˺�
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
///Boost.Python��װ
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
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

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

