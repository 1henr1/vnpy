// vntaptd.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "vntaptd.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
FILE *fp = NULL;
char filename[] = "tap_td_debug.txt";
#endif

using namespace std;
using namespace ITapTrade;


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
	}
}


///-------------------------------------------------------------------------------------
///C++�Ļص����������ݱ��浽������
///-------------------------------------------------------------------------------------

void TdApi::OnConnect() {
	Task task = Task();
	task.task_name = DEFINE_OnConnect;
	this->task_queue.push(task);
}
void TdApi::OnRspLogin(ITapTrade::TAPIINT32 errorCode, const ITapTrade::TapAPITradeLoginRspInfo *loginRspInfo) {
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
void TdApi::OnExpriationDate(ITapTrade::TAPIDATE date, int days) {
	Task task = Task();
	task.task_name = DEFINE_OnExpriationDate;
	this->task_queue.push(task);
}
void TdApi::OnAPIReady() {
	Task task = Task();
	task.task_name = DEFINE_OnAPIReady;
	this->task_queue.push(task);
}
void TdApi::OnDisconnect(ITapTrade::TAPIINT32 reasonCode) {
	Task task = Task();
	task.task_name = DEFINE_OnDisconnect;
	task.task_id = reasonCode;
	this->task_queue.push(task);
}
void TdApi::OnRspChangePassword(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode) {
	Task task = Task();
	task.task_name = DEFINE_OnRspChangePassword;
	task.task_id = errorCode;
	this->task_queue.push(task);
}
void TdApi::OnRspSetReservedInfo(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, const ITapTrade::TAPISTR_50 info) {
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
void TdApi::OnRspQryAccount(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIUINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountInfo *info) {
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
void TdApi::OnRspQryFund(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIFundData *info) {
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
void TdApi::OnRtnFund(const ITapTrade::TapAPIFundData *info) {
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
void TdApi::OnRspQryExchange(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIExchangeInfo *info) {
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
void TdApi::OnRspQryCommodity(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPICommodityInfo *info) {
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
void TdApi::OnRspQryContract(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPITradeContractInfo *info) {
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
void TdApi::OnRtnContract(const ITapTrade::TapAPITradeContractInfo *info) {
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
void TdApi::OnRspOrderAction(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, const ITapTrade::TapAPIOrderActionRsp *info) {
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
void TdApi::OnRtnOrder(const ITapTrade::TapAPIOrderInfoNotice *info) {
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
void TdApi::OnRspQryOrder(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIOrderInfo *info) {
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
void TdApi::OnRspQryOrderProcess(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIOrderInfo *info) {
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
void TdApi::OnRspQryFill(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIFillInfo *info) {
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
void TdApi::OnRtnFill(const ITapTrade::TapAPIFillInfo *info) {
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
void TdApi::OnRspQryPosition(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIPositionInfo *info) {
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
void TdApi::OnRtnPosition(const ITapTrade::TapAPIPositionInfo *info) {
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
void TdApi::OnRtnPositionProfit(const ITapTrade::TapAPIPositionProfitNotice *info) {
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
void TdApi::OnRspQryCurrency(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPICurrencyInfo *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryCurrency;
	this->task_queue.push(task);
}
void TdApi::OnRspQryTradeMessage(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPITradeMessage *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryTradeMessage;
	this->task_queue.push(task);
}
void TdApi::OnRtnTradeMessage(const ITapTrade::TapAPITradeMessage *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRtnTradeMessage;
	this->task_queue.push(task);
}
void TdApi::OnRspQryHisOrder(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisOrderQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryHisOrder;
	this->task_queue.push(task);
}
void TdApi::OnRspQryHisOrderProcess(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisOrderProcessQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryHisOrderProcess;
	this->task_queue.push(task);
}
void TdApi::OnRspQryHisMatch(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisMatchQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryHisMatch;
	this->task_queue.push(task);
}
void TdApi::OnRspQryHisPosition(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisPositionQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryHisPosition;
	this->task_queue.push(task);
}
void TdApi::OnRspQryHisDelivery(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisDeliveryQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryHisDelivery;
	this->task_queue.push(task);
}
void TdApi::OnRspQryAccountCashAdjust(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountCashAdjustQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryAccountCashAdjust;
	this->task_queue.push(task);
}
void TdApi::OnRspQryBill(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIBillQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryBill;
	this->task_queue.push(task);
}
void TdApi::OnRspQryAccountFeeRent(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountFeeRentQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryAccountFeeRent;
	this->task_queue.push(task);
}
void TdApi::OnRspQryAccountMarginRent(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountMarginRentQryRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspQryAccountMarginRent;
	this->task_queue.push(task);
}
void TdApi::OnRspHKMarketOrderInsert(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode,  const ITapTrade::TapAPIOrderMarketInsertRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspHKMarketOrderInsert;
	this->task_queue.push(task);
}
void TdApi::OnRspHKMarketOrderDelete(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, const ITapTrade::TapAPIOrderMarketDeleteRsp *info) {
	Task task = Task();
	task.task_name = DEFINE_OnRspHKMarketOrderDelete;
	this->task_queue.push(task);
}
void TdApi::OnHKMarketQuoteNotice( const ITapTrade::TapAPIOrderQuoteMarketNotice *info) {
	Task task = Task();
	task.task_name = DEFINE_OnHKMarketQuoteNotice;
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
		case DEFINE_OnRspQryCurrency:
		{
			this->processRspQryCurrency(task);
			break;
		}
		case DEFINE_OnRspQryTradeMessage:
		{
			this->processRspQryTradeMessage(task);
			break;
		}
		case DEFINE_OnRtnTradeMessage:
		{
			this->processRtnTradeMessage(task);
			break;
		}
		case DEFINE_OnRspQryHisOrder:
		{
			this->processRspQryHisOrder(task);
			break;
		}
		case DEFINE_OnRspQryHisOrderProcess:
		{
			this->processRspQryHisOrderProcess(task);
			break;
		}
		case DEFINE_OnRspQryHisMatch:
		{
			this->processRspQryHisMatch(task);
			break;
		}
		case DEFINE_OnRspQryHisPosition:
		{
			this->processRspQryHisPosition(task);
			break;
		}
		case DEFINE_OnRspQryHisDelivery:
		{
			this->processRspQryHisDelivery(task);
			break;
		}
		case DEFINE_OnRspQryAccountCashAdjust:
		{
			this->processRspQryAccountCashAdjust(task);
			break;
		}
		case DEFINE_OnRspQryBill:
		{
			this->processRspQryBill(task);
			break;
		}
		case DEFINE_OnRspQryAccountFeeRent:
		{
			this->processRspQryAccountFeeRent(task);
			break;
		}
		case DEFINE_OnRspQryAccountMarginRent:
		{
			this->processRspQryAccountMarginRent(task);
			break;
		}
		case DEFINE_OnRspHKMarketOrderInsert:
		{
			this->processRspHKMarketOrderInsert(task);
			break;
		}
		case DEFINE_OnRspHKMarketOrderDelete:
		{
			this->processRspHKMarketOrderDelete(task);
			break;
		}
		case DEFINE_OnHKMarketQuoteNotice:
		{
			this->processHKMarketQuoteNotice(task);
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
	data["AccountTradeRight"] = task_data.AccountTradeRight;					///<����״̬
	data["CommodityGroupNo"] = task_data.CommodityGroupNo;						///<�ɽ���Ʒ����.
	data["AccountShortName"] = task_data.AccountShortName;                       ///< �˺ż��
	data["AccountEnShortName"] = task_data.AccountEnShortName;						///<�˺�Ӣ�ļ��
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
	data["AuthMoney"] = task_data.AuthMoney;                      ///< �����ʽ�
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
	data["AuthMoney"] = task_data.AuthMoney;                      ///< �����ʽ�
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
	data["CurrencyGroupNo"] = task_data.CurrencyGroupNo;
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
	data["RefDouble"] = task_data.RefDouble;						///< ����ο�ֵ
	data["RefString"] = task_data.RefString;                                      ///< �ַ����ο�ֵ
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["ClientID"] = task_data.ClientID;                                           ///< �ͻ����˺�.
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["ClientLocalIP"] = task_data.ClientLocalIP;					///< �ն˱���IP
	data["ClientMac"] = task_data.ClientMac;						///< �ն˱���Mac��ַ
	data["ClientIP"] = task_data.ClientIP;						///< �ն������ַ.
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
	data["RefDouble"] = task_data.RefDouble;						///< ����ο�ֵ
	data["RefString"] = task_data.RefString;                                      ///< �ַ����ο�ֵ
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["ClientID"] = task_data.ClientID;                                           ///< �ͻ����˺�.
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["ClientLocalIP"] = task_data.ClientLocalIP;					///< �ն˱���IP
	data["ClientMac"] = task_data.ClientMac;						///< �ն˱���Mac��ַ
	data["ClientIP"] = task_data.ClientIP;						///< �ն������ַ.
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
	data["RefDouble"] = task_data.RefDouble;						///< ����ο�ֵ
	data["RefString"] = task_data.RefString;                                      ///< �ַ����ο�ֵ
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["ClientID"] = task_data.ClientID;                                           ///< �ͻ����˺�.
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["ClientLocalIP"] = task_data.ClientLocalIP;					///< �ն˱���IP
	data["ClientMac"] = task_data.ClientMac;						///< �ն˱���Mac��ַ
	data["ClientIP"] = task_data.ClientIP;						///< �ն������ַ.
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
    data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
    data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
    data["UpperMatchNo"] = task_data.UpperMatchNo;					///< ���ֳɽ���
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
    data["ClosePrositionPrice"] = task_data.ClosePrositionPrice;					///< ָ���۸�ƽ��
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
    data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
    data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
    data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
    data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
    data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�
    data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���
    data["MatchSide"] = task_data.MatchSide;						///< ��������
    data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
    data["PositionNo"] = task_data.PositionNo;						///< ���سֲֺţ���������д
    data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
    data["OrderNo"] = task_data.OrderNo;						///< ί�б���
    data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
    data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
    data["PositionPrice"] = task_data.PositionPrice;					///< �ֲּ�
    data["PositionQty"] = task_data.PositionQty;					///< �ֲ���
	data["PositionStreamId"] = task_data.PositionStreamId;				///< �ֲ�����
    data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
    data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
    data["CalculatePrice"] = task_data.CalculatePrice;					///< ��ǰ����۸�
    data["AccountInitialMargin"] = task_data.AccountInitialMargin;			///< �ͻ���ʼ��֤��
    data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
    data["UpperInitialMargin"] = task_data.UpperInitialMargin;				///< ���ֳ�ʼ��֤��
    data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
    data["PositionProfit"] = task_data.PositionProfit;					///< �ֲ�ӯ��
    data["LMEPositionProfit"] = task_data.LMEPositionProfit;				///< LME�ֲ�ӯ��
    data["OptionMarketValue"] = task_data.OptionMarketValue;				///< ��Ȩ��ֵ
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
    data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
    data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
    data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
    data["ContractNo"] = task_data.ContractNo;						///< ��Լ1
    data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�
    data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���
    data["MatchSide"] = task_data.MatchSide;						///< ��������
    data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
    data["PositionNo"] = task_data.PositionNo;						///< ���سֲֺţ���������д
    data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
    data["OrderNo"] = task_data.OrderNo;						///< ί�б���
    data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
    data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
    data["PositionPrice"] = task_data.PositionPrice;					///< �ֲּ�
    data["PositionQty"] = task_data.PositionQty;					///< �ֲ���
	data["PositionStreamId"] = task_data.PositionStreamId;				///< �ֲ�����
    data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
    data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
    data["CalculatePrice"] = task_data.CalculatePrice;					///< ��ǰ����۸�
    data["AccountInitialMargin"] = task_data.AccountInitialMargin;			///< �ͻ���ʼ��֤��
    data["AccountMaintenanceMargin"] = task_data.AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
    data["UpperInitialMargin"] = task_data.UpperInitialMargin;				///< ���ֳ�ʼ��֤��
    data["UpperMaintenanceMargin"] = task_data.UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
    data["PositionProfit"] = task_data.PositionProfit;					///< �ֲ�ӯ��
    data["LMEPositionProfit"] = task_data.LMEPositionProfit;				///< LME�ֲ�ӯ��
    data["OptionMarketValue"] = task_data.OptionMarketValue;				///< ��Ȩ��ֵ
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

void TdApi::processRspQryCurrency(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryCurrency(task.task_id, task.task_last, data);
};

void TdApi::processRspQryTradeMessage(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryTradeMessage(task.task_id, task.task_last, data);
};

void TdApi::processRtnTradeMessage(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRtnTradeMessage(data);
};

void TdApi::processRspQryHisOrder(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryHisOrder(task.task_id, task.task_last, data);
};

void TdApi::processRspQryHisOrderProcess(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryHisOrderProcess(task.task_id, task.task_last, data);
};

void TdApi::processRspQryHisMatch(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryHisMatch(task.task_id, task.task_last, data);
};

void TdApi::processRspQryHisPosition(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryHisPosition(task.task_id, task.task_last, data);
};

void TdApi::processRspQryHisDelivery(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryHisDelivery(task.task_id, task.task_last, data);
};

void TdApi::processRspQryAccountCashAdjust(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryAccountCashAdjust(task.task_id, task.task_last, data);
};

void TdApi::processRspQryBill(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryBill(task.task_id, task.task_last, data);
};

void TdApi::processRspQryAccountFeeRent(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryAccountFeeRent(task.task_id, task.task_last, data);
};

void TdApi::processRspQryAccountMarginRent(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspQryAccountMarginRent(task.task_id, task.task_last, data);
};

void TdApi::processRspHKMarketOrderInsert(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspHKMarketOrderInsert(task.task_id, data);
};

void TdApi::processRspHKMarketOrderDelete(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspHKMarketOrderDelete(task.task_id, data);
};

void TdApi::processHKMarketQuoteNotice(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onHKMarketQuoteNotice(data);
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
	cout << GetITapTradeAPIVersion() << endl;

	//����APIʵ��
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	memset(&stAppInfo, 0, sizeof(stAppInfo));
	getStr(req, "AuthCode", stAppInfo.AuthCode);
	getStr(req, "KeyOperationLogPath", stAppInfo.KeyOperationLogPath);
	api = CreateITapTradeAPI(&stAppInfo, iResult);
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
	return api->Login(&stLoginAuth);
}
int TdApi::setVertificateCode(string VertificateCode)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	return api->SetVertificateCode( (char*) (VertificateCode.c_str()) );
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
    getDouble(req, "OrderPrice",&stNewOrder.OrderPrice);						///< ί�м۸�1
    getDouble(req, "OrderPrice2",&stNewOrder.OrderPrice2);					///< ί�м۸�2��������Ӧ��ʹ��
    getDouble(req, "StopPrice",&stNewOrder.StopPrice);						///< �����۸�
    getUInt(req, "OrderQty",&stNewOrder.OrderQty);						///< ί������������
    getUInt(req, "OrderMinQty",&stNewOrder.OrderMinQty);					///< ��С�ɽ�����Ĭ��1
    getUInt(req, "MinClipSize",&stNewOrder.MinClipSize);					///< ��ɽ����С�����
    getUInt(req, "MaxClipSize",&stNewOrder.MaxClipSize);					///< ��ɽ����������
    getInt(req, "RefInt",&stNewOrder.RefInt);							///< ���Ͳο�ֵ
    getDouble(req, "RefDouble",&stNewOrder.RefDouble);						///<����ο�ֵ
    getStr(req, "RefString", stNewOrder.RefString);						///< �ַ����ο�ֵ
    getStr(req, "ClientID", stNewOrder.ClientID);						///<�ͻ����˺ţ�����������˺ţ��������ϱ����˺�
    getChar(req, "TacticsType",&stNewOrder.TacticsType);					///< ���Ե����ͣ�Ĭ��N
    getChar(req, "TriggerCondition",&stNewOrder.TriggerCondition);				///< ����������Ĭ��N
    getChar(req, "TriggerPriceType",&stNewOrder.TriggerPriceType);				///< �����۸����ͣ�Ĭ��N
    getChar(req, "AddOneIsValid",&stNewOrder.AddOneIsValid);					///< �Ƿ�T+1��Ч,Ĭ��T+1��Ч��
	return api->InsertOrder(&m_sessionID, &m_clientOrderNo, &stNewOrder);
}

int TdApi::reqCancelOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIOrderCancelReq stCancelReq;
	getInt(req, "RefInt", &stCancelReq.RefInt);							///< ���Ͳο�ֵ
	getDouble(req, "RefDouble", &stCancelReq.RefDouble);						///< ����ο�ֵ
	getStr(req, "RefString", stCancelReq.RefString);						///< �ַ����ο�ֵ
	getChar(req, "ServerFlag", &stCancelReq.ServerFlag);						///< ��������ʶ
	getStr(req, "OrderNo", stCancelReq.OrderNo);						///< ί�б���
	return api->CancelOrder(&m_sessionID, &stCancelReq);
}

int TdApi::reqAmendOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIAmendOrder stAmendOrder;
    getStr(req, "AccountNo", stAmendOrder.ReqData.AccountNo);						///< �ͻ��ʽ��ʺţ�����
    getStr(req, "ExchangeNo", stAmendOrder.ReqData.ExchangeNo);						///< ��������ţ�����
    getChar(req, "CommodityType",&stAmendOrder.ReqData.CommodityType);					///< Ʒ�����ͣ�����
    getStr(req, "CommodityNo", stAmendOrder.ReqData.CommodityNo);					///< Ʒ�ֱ������ͣ�����
    getStr(req, "ContractNo", stAmendOrder.ReqData.ContractNo);						///< ��Լ1������
    getStr(req, "StrikePrice", stAmendOrder.ReqData.StrikePrice);					///< ִ�м۸�1����Ȩ��д
    getChar(req, "CallOrPutFlag",&stAmendOrder.ReqData.CallOrPutFlag);					///< ���ſ���1 Ĭ��N
    getStr(req, "ContractNo2", stAmendOrder.ReqData.ContractNo2);					///< ��Լ2��Ĭ�Ͽ�
    getStr(req, "StrikePrice2", stAmendOrder.ReqData.StrikePrice2);					///< ִ�м۸�2��Ĭ�Ͽ�
    getChar(req, "CallOrPutFlag2",&stAmendOrder.ReqData.CallOrPutFlag2);					///< ���ſ���2 Ĭ��N
    getChar(req, "OrderType",&stAmendOrder.ReqData.OrderType);						///< ί������ ����
    getChar(req, "OrderSource",&stAmendOrder.ReqData.OrderSource);					///< ί����Դ��Ĭ�ϳ��򵥡�
    getChar(req, "TimeInForce",&stAmendOrder.ReqData.TimeInForce);					///< ί����Ч����,Ĭ�ϵ�����Ч
    getStr(req, "ExpireTime", stAmendOrder.ReqData.ExpireTime);						///< ��Ч����(GTD�����ʹ��)
    getChar(req, "IsRiskOrder",&stAmendOrder.ReqData.IsRiskOrder);					///< �Ƿ���ձ�����Ĭ�ϷǷ��ձ���
    getChar(req, "OrderSide",&stAmendOrder.ReqData.OrderSide);						///< ��������
    getChar(req, "PositionEffect",&stAmendOrder.ReqData.PositionEffect);					///< ��ƽ��־1,Ĭ��N
    getChar(req, "PositionEffect2",&stAmendOrder.ReqData.PositionEffect2);				///< ��ƽ��־2��Ĭ��N
    getStr(req, "InquiryNo", stAmendOrder.ReqData.InquiryNo);						///< ѯ�ۺ�
    getChar(req, "HedgeFlag",&stAmendOrder.ReqData.HedgeFlag);						///< Ͷ����ֵ��Ĭ��N
    getDouble(req, "OrderPrice",&stAmendOrder.ReqData.OrderPrice);						///< ί�м۸�1
    getDouble(req, "OrderPrice2",&stAmendOrder.ReqData.OrderPrice2);					///< ί�м۸�2��������Ӧ��ʹ��
    getDouble(req, "StopPrice",&stAmendOrder.ReqData.StopPrice);						///< �����۸�
    getUInt(req, "OrderQty",&stAmendOrder.ReqData.OrderQty);						///< ί������������
    getUInt(req, "OrderMinQty",&stAmendOrder.ReqData.OrderMinQty);					///< ��С�ɽ�����Ĭ��1
    getUInt(req, "MinClipSize",&stAmendOrder.ReqData.MinClipSize);					///< ��ɽ����С�����
    getUInt(req, "MaxClipSize",&stAmendOrder.ReqData.MaxClipSize);					///< ��ɽ����������
    getInt(req, "RefInt",&stAmendOrder.ReqData.RefInt);							///< ���Ͳο�ֵ
    getDouble(req, "RefDouble",&stAmendOrder.ReqData.RefDouble);						///<����ο�ֵ
    getStr(req, "RefString", stAmendOrder.ReqData.RefString);						///< �ַ����ο�ֵ
    getStr(req, "ClientID", stAmendOrder.ReqData.ClientID);						///<�ͻ����˺ţ�����������˺ţ��������ϱ����˺�
    getChar(req, "TacticsType",&stAmendOrder.ReqData.TacticsType);					///< ���Ե����ͣ�Ĭ��N
    getChar(req, "TriggerCondition",&stAmendOrder.ReqData.TriggerCondition);				///< ����������Ĭ��N
    getChar(req, "TriggerPriceType",&stAmendOrder.ReqData.TriggerPriceType);				///< �����۸����ͣ�Ĭ��N
    getChar(req, "AddOneIsValid",&stAmendOrder.ReqData.AddOneIsValid);					///< �Ƿ�T+1��Ч,Ĭ��T+1��Ч��
	getChar(req, "ServerFlag",&stAmendOrder.ServerFlag);			
	getStr(req, "OrderNo", stAmendOrder.OrderNo);			
	return api->AmendOrder(&m_sessionID, &stAmendOrder);
}

int TdApi::qryOrder(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	TapAPIOrderQryReq stOrderQryReq;
    getStr(req, "AccountNo", stOrderQryReq.AccountNo);						///< �ʽ��˺�
    getStr(req, "ExchangeNo", stOrderQryReq.ExchangeNo);						///< ���������
    getChar(req, "CommodityType", &stOrderQryReq.CommodityType);					///< Ʒ������
    getStr(req, "CommodityNo", stOrderQryReq.CommodityNo);					///< Ʒ�ֱ�������
    getChar(req, "OrderType", &stOrderQryReq.OrderType);						///< ί������
    getChar(req, "OrderSource", &stOrderQryReq.OrderSource);					///< ί����Դ
    getChar(req, "TimeInForce", &stOrderQryReq.TimeInForce);					///< ί����Ч����
    getStr(req, "ExpireTime", stOrderQryReq.ExpireTime);						///< ��Ч����(GTD�����ʹ��)
    getChar(req, "IsRiskOrder", &stOrderQryReq.IsRiskOrder);					///< �Ƿ���ձ���
    getChar(req, "ServerFlag", &stOrderQryReq.ServerFlag);						///< ��������ʶ
    getStr(req, "OrderNo", stOrderQryReq.OrderNo);                        ///< ί�б��
    getChar(req, "IsBackInput", &stOrderQryReq.IsBackInput);					///< �Ƿ�Ϊ¼��ί�е�
    getChar(req, "IsDeleted", &stOrderQryReq.IsDeleted);						///< ί�гɽ�ɾ����
    getChar(req, "IsAddOne", &stOrderQryReq.IsAddOne);						///< �Ƿ�ΪT+1��
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
    getStr(req, "AccountNo", stPositionReq.AccountNo);						///< �ͻ��ʽ��ʺţ�����
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
	virtual void onRspQryCurrency(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryCurrency")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryTradeMessage(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryTradeMessage")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRtnTradeMessage(dict data) {
		try
		{
			this->get_override("onRtnTradeMessage")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryHisOrder(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryHisOrder")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryHisOrderProcess(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryHisOrderProcess")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryHisMatch(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryHisMatch")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryHisPosition(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryHisPosition")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryHisDelivery(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryHisDelivery")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryAccountCashAdjust(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryAccountCashAdjust")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryBill(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryBill")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryAccountFeeRent(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryAccountFeeRent")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspQryAccountMarginRent(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspQryAccountMarginRent")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspHKMarketOrderInsert(int errorCode, dict data) {
		try
		{
			this->get_override("onRspHKMarketOrderInsert")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onRspHKMarketOrderDelete(int errorCode, dict data) {
		try
		{
			this->get_override("onRspHKMarketOrderDelete")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	virtual void onHKMarketQuoteNotice(dict data) {
		try
		{
			this->get_override("onHKMarketQuoteNotice")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
};

BOOST_PYTHON_MODULE(vntaptd)
{
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

	class_<TdApiWrap, boost::noncopyable>("TdApi")
		.def("createTapTraderApi", &TdApiWrap::createTapTraderApi)
		.def("setHostAddress", &TdApiWrap::setHostAddress)
		.def("reqUserLogin", &TdApiWrap::reqUserLogin)
		.def("setVertificateCode", &TdApiWrap::setVertificateCode)
		.def("exit", &TdApiWrap::exit)
		.def("qryAccount", &TdApiWrap::qryAccount)
		.def("qryFund", &TdApiWrap::qryFund)
		.def("qryExchange", &TdApiWrap::qryExchange)
		.def("qryCommodity", &TdApiWrap::qryCommodity)
		.def("qryContract", &TdApiWrap::qryContract)
		.def("reqInsertOrder", &TdApiWrap::reqInsertOrder)
		.def("reqCancelOrder", &TdApiWrap::reqCancelOrder)
		.def("reqAmendOrder", &TdApiWrap::reqAmendOrder)
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
		.def("onRspQryCurrency", pure_virtual(&TdApiWrap::onRspQryCurrency))
		.def("onRspQryTradeMessage", pure_virtual(&TdApiWrap::onRspQryTradeMessage))
		.def("onRtnTradeMessage", pure_virtual(&TdApiWrap::onRtnTradeMessage))
		.def("onRspQryHisOrder", pure_virtual(&TdApiWrap::onRspQryHisOrder))
		.def("onRspQryHisOrderProcess", pure_virtual(&TdApiWrap::onRspQryHisOrderProcess))
		.def("onRspQryHisMatch", pure_virtual(&TdApiWrap::onRspQryHisMatch))
		.def("onRspQryHisPosition", pure_virtual(&TdApiWrap::onRspQryHisPosition))
		.def("onRspQryHisDelivery", pure_virtual(&TdApiWrap::onRspQryHisDelivery))
		.def("onRspQryAccountCashAdjust", pure_virtual(&TdApiWrap::onRspQryAccountCashAdjust))
		.def("onRspQryBill", pure_virtual(&TdApiWrap::onRspQryBill))
		.def("onRspQryAccountFeeRent", pure_virtual(&TdApiWrap::onRspQryAccountFeeRent))
		.def("onRspQryAccountMarginRent", pure_virtual(&TdApiWrap::onRspQryAccountMarginRent))
		.def("onRspHKMarketOrderInsert", pure_virtual(&TdApiWrap::onRspHKMarketOrderInsert))
		.def("onRspHKMarketOrderDelete", pure_virtual(&TdApiWrap::onRspHKMarketOrderDelete))
		.def("onHKMarketQuoteNotice", pure_virtual(&TdApiWrap::onHKMarketQuoteNotice))
		;
}

