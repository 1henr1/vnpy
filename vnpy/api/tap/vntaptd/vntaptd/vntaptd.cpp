// vntaptd.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "vntaptd.h"
#include "TapTradeAPI.h"
#include "TapAPIError.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
FILE *fp = NULL;
char filename[] = "tap_td_debug.txt";
#endif

using namespace std;

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

void TdApi::OnConnect()  {
	Task task = Task();
	task.task_name = ONCONNECT;
	this->task_queue.push(task);
} ;
void TdApi::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo)  {
	Task task = Task();
	task.task_name = ONRSPLOGIN;
	task.task_id = errorCode;
	if (loginRspInfo)
	{
		task.task_data = *loginRspInfo;
	}
	else
	{
		TapAPITradeLoginRspInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnAPIReady()  {
	Task task = Task();
	task.task_name = ONAPIREADY;
	this->task_queue.push(task);
} ;
void TdApi::OnDisconnect(TAPIINT32 reasonCode)  {
	Task task = Task();
	task.task_name = ONDISCONNECT;
	task.task_id = reasonCode;
	this->task_queue.push(task);
} ;
void TdApi::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode)  {
	Task task = Task();
	task.task_name = ONRSPCHANGEPASSWORD;
	task.task_id = errorCode;
	this->task_queue.push(task);
} ;
void TdApi::OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info)  {
	Task task = Task();
	task.task_name = ONRSPSETRESERVEDINFO;
	task.task_id = errorCode;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TAPISTR_50 empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYACCOUNT;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIAccountInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYFUND;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFundData empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnFund(const TapAPIFundData *info)  {
	Task task = Task();
	task.task_name = ONRTNFUND;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFundData empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYEXCHANGE;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIExchangeInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYCOMMODITY;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPICommodityInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYCONTRACT;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPITradeContractInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnContract(const TapAPITradeContractInfo *info)  {
	Task task = Task();
	task.task_name = ONRTNCONTRACT;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPITradeContractInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnOrder(const TapAPIOrderInfoNotice *info)  {
	Task task = Task();
	task.task_name = ONRTNORDER;
	if (info)
	{
		task.task_id = info->ErrorCode;
		task.task_data = *(info->OrderInfo);
	}
	else
	{
		TapAPIOrderInfo empty_data;
		task.task_id = -1;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info)  {
	Task task = Task();
	task.task_name = ONRSPORDERACTION;
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
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYORDER;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIOrderInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYORDERPROCESS;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIOrderInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYFILL;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFillInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnFill(const TapAPIFillInfo *info)  {
	Task task = Task();
	task.task_name = ONRTNFILL;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIFillInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYPOSITION;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIPositionInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnPosition(const TapAPIPositionInfo *info)  {
	Task task = Task();
	task.task_name = ONRTNPOSITION;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIPositionInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnClose(const TapAPICloseInfo *info)  {
	Task task = Task();
	task.task_name = ONRTNCLOSE;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPICloseInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnPositionProfit(const TapAPIPositionProfitNotice *info)  {
	Task task = Task();
	task.task_name = ONRTNPOSITIONPROFIT;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIPositionProfitNotice empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp *info)  {
	Task task = Task();
	task.task_name = ONRSPQRYDEEPQUOTE;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIDeepQuoteQryRsp empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast,const TapAPIExchangeStateInfo * info) {
	Task task = Task();
	task.task_name = ONRSPQRYEXCHANGESTATEINFO;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIExchangeStateInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info) {
	Task task = Task();
	task.task_name = ONRTNEXCHANGESTATEINFO;
	if (info)
	{
		task.task_data = info->ExchangeStateInfo;
		task.task_last = (info->IsLast == APIYNFLAG_YES) ? true : false;
	}
	else
	{
		TapAPIExchangeStateInfo empty_data;
		task.task_data = empty_data;
		task.task_last = true;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info)  {
	Task task = Task();
	task.task_name = ONRTNREQQUOTENOTICE;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIReqQuoteNotice empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;
void TdApi::OnRspUpperChannelInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info) {
	Task task = Task();
	task.task_name = ONRSPUPPERCHANNELINFO;
	task.task_id = errorCode;
	task.task_last = isLast;
	if (info)
	{
		task.task_data = *info;
	}
	else
	{
		TapAPIUpperChannelInfo empty_data;
		task.task_data = empty_data;
	}
	this->task_queue.push(task);
} ;

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
			case ONCONNECT:
			{
				this->processConnect(task);
				break;
			}
			case ONRSPLOGIN:
			{
				this->processRspLogin(task);
				break;
			}
			case ONAPIREADY:
			{
				this->processAPIReady(task);
				break;
			}
			case ONDISCONNECT:
			{
				this->processDisconnect(task);
				break;
			}
			case ONRSPCHANGEPASSWORD:
			{
				this->processRspChangePassword(task);
				break;
			}
			case ONRSPSETRESERVEDINFO:
			{
				this->processRspSetReservedInfo(task);
				break;
			}
			case ONRSPQRYACCOUNT:
			{
				this->processRspQryAccount(task);
				break;
			}
			case ONRSPQRYFUND:
			{
				this->processRspQryFund(task);
				break;
			}
			case ONRTNFUND:
			{
				this->processRtnFund(task);
				break;
			}
			case ONRSPQRYEXCHANGE:
			{
				this->processRspQryExchange(task);
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
			case ONRTNCONTRACT:
			{
				this->processRtnContract(task);
				break;
			}
			case ONRTNORDER:
			{
				this->processRtnOrder(task);
				break;
			}
			case ONRSPORDERACTION:
			{
				this->processRspOrderAction(task);
				break;
			}
			case ONRSPQRYORDER:
			{
				this->processRspQryOrder(task);
				break;
			}
			case ONRSPQRYORDERPROCESS:
			{
				this->processRspQryOrderProcess(task);
				break;
			}
			case ONRSPQRYFILL:
			{
				this->processRspQryFill(task);
				break;
			}
			case ONRTNFILL:
			{
				this->processRtnFill(task);
				break;
			}
			case ONRSPQRYPOSITION:
			{
				this->processRspQryPosition(task);
				break;
			}
			case ONRTNPOSITION:
			{
				this->processRtnPosition(task);
				break;
			}
			case ONRTNCLOSE:
			{
				this->processRtnClose(task);
				break;
			}
			case ONRTNPOSITIONPROFIT:
			{
				this->processRtnPositionProfit(task);
				break;
			}
			case ONRSPQRYDEEPQUOTE:
			{
				this->processRspQryDeepQuote(task);
				break;
			}
			case ONRSPQRYEXCHANGESTATEINFO:
			{
				this->processRspQryExchangeStateInfo(task);
				break;
			}
			case ONRTNEXCHANGESTATEINFO:
			{
				this->processRtnExchangeStateInfo(task);
				break;
			}
			case ONRTNREQQUOTENOTICE:
			{
				this->processRtnReqQuoteNotice(task);
				break;
			}
			case ONRSPUPPERCHANNELINFO:
			{
				this->processRspUpperChannelInfo(task);
				break;
			}
		}
	}
}

void TdApi::processConnect(Task task) {
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onConnect();
}
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
	data["QuoteTempPassword"] = task_data.QuoteTempPassword;				///< ������ʱ����
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
}
void TdApi::processAPIReady(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onAPIReady();
}
void TdApi::processDisconnect(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onDisconnect(task.task_id);
}
void TdApi::processRspChangePassword(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	this->onRspChangePassword(task.task_id);
}
void TdApi::processRspSetReservedInfo(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	dict data;
	this->onRspSetReservedInfo(task.task_id, data);
}
void TdApi::processRspQryAccount(Task task){
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
	data["AccountIsDocHolder"] = task_data.AccountIsDocHolder;                     ///< �Ƿ��֤��
	data["IsMarketMaker"] = task_data.IsMarketMaker;							///< �Ƿ���������
	data["AccountFamilyType"] = task_data.AccountFamilyType;						///< �����˺�����
	this->onRspQryAccount(task.task_id, task.task_last, data);
}
void TdApi::processRspQryFund(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFundData task_data = any_cast<TapAPIFundData>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��˺�
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
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
	data["OriginalCashInOut"] = task_data.OriginalCashInOut;				///< ����ԭʼ�����
	data["FloatingPL"] = task_data.FloatingPL;						///< ��ʸ�ӯ
	data["FrozenRiskFundValue"] = task_data.FrozenRiskFundValue;			///< ���ն����ʽ�
	data["ClosePL"] = task_data.ClosePL;						///< ���ƽӯ
	data["NoCurrencyPledgeValue"] = task_data.NoCurrencyPledgeValue;          ///< �ǻ�����Ѻ
	data["PrePledgeValue"] = task_data.PrePledgeValue;                 ///< �ڳ���Ѻ
	data["PledgeIn"] = task_data.PledgeIn;                       ///< ����
	data["PledgeOut"] = task_data.PledgeOut;                      ///< �ʳ�
	data["PledgeValue"] = task_data.PledgeValue;                    ///< ��Ѻ���
	data["BorrowValue"] = task_data.BorrowValue;                    ///< ���ý��
	data["SpecialAccountFrozenMargin"] = task_data.SpecialAccountFrozenMargin;     ///< �����Ʒ���ᱣ֤��
	data["SpecialAccountMargin"] = task_data.SpecialAccountMargin;           ///< �����Ʒ��֤��   
	data["SpecialAccountFrozenFee"] = task_data.SpecialAccountFrozenFee;        ///< �����Ʒ����������
	data["SpecialAccountFee"] = task_data.SpecialAccountFee;              ///< �����Ʒ������
	data["SpecialFloatProfit"] = task_data.SpecialFloatProfit;             ///< �����Ʒ��ӯ
	data["SpecialCloseProfit"] = task_data.SpecialCloseProfit;             ///< �����Ʒƽӯ
	data["SpecialFloatPL"] = task_data.SpecialFloatPL;                 ///< �����Ʒ��ʸ�ӯ
	data["SpecialClosePL"] = task_data.SpecialClosePL;                 ///< �����Ʒ���ƽӯ
	this->onRspQryFund(task.task_id, task.task_last, data);
}
void TdApi::processRtnFund(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFundData task_data = any_cast<TapAPIFundData>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��˺�
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
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
	data["OriginalCashInOut"] = task_data.OriginalCashInOut;				///< ����ԭʼ�����
	data["FloatingPL"] = task_data.FloatingPL;						///< ��ʸ�ӯ
	data["FrozenRiskFundValue"] = task_data.FrozenRiskFundValue;			///< ���ն����ʽ�
	data["ClosePL"] = task_data.ClosePL;						///< ���ƽӯ
	data["NoCurrencyPledgeValue"] = task_data.NoCurrencyPledgeValue;          ///< �ǻ�����Ѻ
	data["PrePledgeValue"] = task_data.PrePledgeValue;                 ///< �ڳ���Ѻ
	data["PledgeIn"] = task_data.PledgeIn;                       ///< ����
	data["PledgeOut"] = task_data.PledgeOut;                      ///< �ʳ�
	data["PledgeValue"] = task_data.PledgeValue;                    ///< ��Ѻ���
	data["BorrowValue"] = task_data.BorrowValue;                    ///< ���ý��
	data["SpecialAccountFrozenMargin"] = task_data.SpecialAccountFrozenMargin;     ///< �����Ʒ���ᱣ֤��
	data["SpecialAccountMargin"] = task_data.SpecialAccountMargin;           ///< �����Ʒ��֤��   
	data["SpecialAccountFrozenFee"] = task_data.SpecialAccountFrozenFee;        ///< �����Ʒ����������
	data["SpecialAccountFee"] = task_data.SpecialAccountFee;              ///< �����Ʒ������
	data["SpecialFloatProfit"] = task_data.SpecialFloatProfit;             ///< �����Ʒ��ӯ
	data["SpecialCloseProfit"] = task_data.SpecialCloseProfit;             ///< �����Ʒƽӯ
	data["SpecialFloatPL"] = task_data.SpecialFloatPL;                 ///< �����Ʒ��ʸ�ӯ
	data["SpecialClosePL"] = task_data.SpecialClosePL;                 ///< �����Ʒ���ƽӯ
	this->onRtnFund(data);
}
void TdApi::processRspQryExchange(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIExchangeInfo task_data = any_cast<TapAPIExchangeInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;								///< ����������
	data["ExchangeName"] = task_data.ExchangeName;							///< ����������
	this->onRspQryExchange(task.task_id, task.task_last, data);
}
void TdApi::processRspQryCommodity(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPICommodityInfo task_data = any_cast<TapAPICommodityInfo>(task.task_data);
	dict data;
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ����������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ��
	data["CommodityName"] = task_data.CommodityName;					///< Ʒ������
	data["CommodityEngName"] = task_data.CommodityEngName;				///< Ʒ��Ӣ������
	data["RelateExchangeNo"] = task_data.RelateExchangeNo;				///< ����Ʒ�ֽ���������
	data["RelateCommodityType"] = task_data.RelateCommodityType;			///< ����Ʒ��Ʒ������
	data["RelateCommodityNo"] = task_data.RelateCommodityNo;				///< ����Ʒ��Ʒ�ֱ��
	data["RelateExchangeNo2"] = task_data.RelateExchangeNo2;				///< ����Ʒ��2����������
	data["RelateCommodityType2"] = task_data.RelateCommodityType2;			///< ����Ʒ��2Ʒ������
	data["RelateCommodityNo2"] = task_data.RelateCommodityNo2;				///< ����Ʒ��2Ʒ�ֱ��
	data["TradeCurrency"] = task_data.TradeCurrency;					///< ���ױ���
	data["SettleCurrency"] = task_data.SettleCurrency;					///< �������
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ���
	data["MarginCalculateMode"] = task_data.MarginCalculateMode;			///< �ڻ���֤��ʽ,�ֱ�'1',����'2'
	data["OptionMarginCalculateMode"] = task_data.OptionMarginCalculateMode;		///< ��Ȩ��֤��ʽ
	data["OpenCloseMode"] = task_data.OpenCloseMode;					///< ��ƽ��ʽ
	data["StrikePriceTimes"] = task_data.StrikePriceTimes;				///< ִ�м۸���
	data["IsOnlyQuoteCommodity"] = task_data.IsOnlyQuoteCommodity;			///< ������Ʒ��
	data["CommodityTickSize"] = task_data.CommodityTickSize;				///< ��С�䶯��λ
	data["CommodityDenominator"] = task_data.CommodityDenominator;			///< ���۷�ĸ
	data["CmbDirect"] = task_data.CmbDirect;						///< ��Ϸ���
	data["OnlyCanCloseDays"] = task_data.OnlyCanCloseDays;				///< ֻ��ƽ����ǰ����
	data["DeliveryMode"] = task_data.DeliveryMode;					///< ������Ȩ��ʽ
	data["DeliveryDays"] = task_data.DeliveryDays;					///< ������ƫ��
	data["AddOneTime"] = task_data.AddOneTime;						///< T+1�ָ�ʱ��
	data["CommodityTimeZone"] = task_data.CommodityTimeZone;				///< Ʒ��ʱ��
	this->onRspQryCommodity(task.task_id, task.task_last, data);
}
void TdApi::processRspQryContract(Task task){
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
	data["FutureContractNo"] = task_data.FutureContractNo;						///< �ڻ���Լ���(��Ȩ���)
	this->onRspQryContract(task.task_id, task.task_last, data);
}
void TdApi::processRtnContract(Task task){
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
	data["FutureContractNo"] = task_data.FutureContractNo;						///< �ڻ���Լ���(��Ȩ���)
	this->onRtnContract(data);
}
void TdApi::processRtnOrder(Task task){
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
	data["OrderCanceledQty"] = task_data.OrderCanceledQty;				///< ��������
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;						///< �ַ����ο�ֵ
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentNo"] = task_data.OrderParentNo;					///< ������
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
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
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["ContractSize2"] = task_data.ContractSize2;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["FeeMode"] = task_data.FeeMode;						///< �����Ѽ��㷽ʽ
	data["FeeParam"] = task_data.FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
	data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
	data["FeeMode2"] = task_data.FeeMode2;						///< �����Ѽ��㷽ʽ
	data["FeeParam2"] = task_data.FeeParam2;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup2"] = task_data.FeeCurrencyGroup2;				///< �ͻ������ѱ�����
	data["FeeCurrency2"] = task_data.FeeCurrency2;					///< �ͻ������ѱ���
	data["MarginMode"] = task_data.MarginMode;						///< ��֤����㷽ʽ
	data["MarginParam"] = task_data.MarginParam;					///< ��֤�����ֵ
	data["MarginMode2"] = task_data.MarginMode2;					///< ��֤����㷽ʽ
	data["MarginParam2"] = task_data.MarginParam2;					///< ��֤�����ֵ
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������  ������ʽ���м۵�����϶���ʹ��
	data["PreSettlePrice2"] = task_data.PreSettlePrice2;				///< ������  ������ʽ���м۵�����϶���ʹ��
	data["OpenVol"] = task_data.OpenVol;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol"] = task_data.CoverVol;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["OpenVol2"] = task_data.OpenVol2;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol2"] = task_data.CoverVol2;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["FeeValue"] = task_data.FeeValue;						///< ����������
	data["MarginValue"] = task_data.MarginValue;					///< ���ᱣ֤�� �ϲ�����Ĵ���Ϊ0
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderQty2"] = task_data.OrderQty2;						///< ί������2
	data["HedgeFlag2"] = task_data.HedgeFlag2;						///< Ͷ����ֵ2
	data["MarketLevel"] = task_data.MarketLevel;					///< �м۴�����
	data["OrderDeleteByDisConnFlag"] = task_data.OrderDeleteByDisConnFlag;		///< �������ʧ��ʱ���������Զ�������ʶ
	this->onRtnOrder(task.task_id, data);
}
void TdApi::processRspOrderAction(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TAPIORDERACT ActionType = any_cast<TAPIORDERACT>(task.task_error);
	TapAPIOrderInfo task_data = any_cast<TapAPIOrderInfo>(task.task_data);
	dict data;
	data["ActionType"] = ActionType;
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
	data["OrderCanceledQty"] = task_data.OrderCanceledQty;				///< ��������
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;						///< �ַ����ο�ֵ
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentNo"] = task_data.OrderParentNo;					///< ������
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
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
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["ContractSize2"] = task_data.ContractSize2;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["FeeMode"] = task_data.FeeMode;						///< �����Ѽ��㷽ʽ
	data["FeeParam"] = task_data.FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
	data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
	data["FeeMode2"] = task_data.FeeMode2;						///< �����Ѽ��㷽ʽ
	data["FeeParam2"] = task_data.FeeParam2;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup2"] = task_data.FeeCurrencyGroup2;				///< �ͻ������ѱ�����
	data["FeeCurrency2"] = task_data.FeeCurrency2;					///< �ͻ������ѱ���
	data["MarginMode"] = task_data.MarginMode;						///< ��֤����㷽ʽ
	data["MarginParam"] = task_data.MarginParam;					///< ��֤�����ֵ
	data["MarginMode2"] = task_data.MarginMode2;					///< ��֤����㷽ʽ
	data["MarginParam2"] = task_data.MarginParam2;					///< ��֤�����ֵ
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������  ������ʽ���м۵�����϶���ʹ��
	data["PreSettlePrice2"] = task_data.PreSettlePrice2;				///< ������  ������ʽ���м۵�����϶���ʹ��
	data["OpenVol"] = task_data.OpenVol;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol"] = task_data.CoverVol;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["OpenVol2"] = task_data.OpenVol2;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol2"] = task_data.CoverVol2;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["FeeValue"] = task_data.FeeValue;						///< ����������
	data["MarginValue"] = task_data.MarginValue;					///< ���ᱣ֤�� �ϲ�����Ĵ���Ϊ0
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderQty2"] = task_data.OrderQty2;						///< ί������2
	data["HedgeFlag2"] = task_data.HedgeFlag2;						///< Ͷ����ֵ2
	data["MarketLevel"] = task_data.MarketLevel;					///< �м۴�����
	data["OrderDeleteByDisConnFlag"] = task_data.OrderDeleteByDisConnFlag;		///< �������ʧ��ʱ���������Զ�������ʶ
	this->onRspOrderAction(task.task_id, data);
}
void TdApi::processRspQryOrder(Task task){
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
	data["OrderCanceledQty"] = task_data.OrderCanceledQty;				///< ��������
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;						///< �ַ����ο�ֵ
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentNo"] = task_data.OrderParentNo;					///< ������
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
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
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["ContractSize2"] = task_data.ContractSize2;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["FeeMode"] = task_data.FeeMode;						///< �����Ѽ��㷽ʽ
	data["FeeParam"] = task_data.FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
	data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
	data["FeeMode2"] = task_data.FeeMode2;						///< �����Ѽ��㷽ʽ
	data["FeeParam2"] = task_data.FeeParam2;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup2"] = task_data.FeeCurrencyGroup2;				///< �ͻ������ѱ�����
	data["FeeCurrency2"] = task_data.FeeCurrency2;					///< �ͻ������ѱ���
	data["MarginMode"] = task_data.MarginMode;						///< ��֤����㷽ʽ
	data["MarginParam"] = task_data.MarginParam;					///< ��֤�����ֵ
	data["MarginMode2"] = task_data.MarginMode2;					///< ��֤����㷽ʽ
	data["MarginParam2"] = task_data.MarginParam2;					///< ��֤�����ֵ
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������  ������ʽ���м۵�����϶���ʹ��
	data["PreSettlePrice2"] = task_data.PreSettlePrice2;				///< ������  ������ʽ���м۵�����϶���ʹ��
	data["OpenVol"] = task_data.OpenVol;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol"] = task_data.CoverVol;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["OpenVol2"] = task_data.OpenVol2;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol2"] = task_data.CoverVol2;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["FeeValue"] = task_data.FeeValue;						///< ����������
	data["MarginValue"] = task_data.MarginValue;					///< ���ᱣ֤�� �ϲ�����Ĵ���Ϊ0
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderQty2"] = task_data.OrderQty2;						///< ί������2
	data["HedgeFlag2"] = task_data.HedgeFlag2;						///< Ͷ����ֵ2
	data["MarketLevel"] = task_data.MarketLevel;					///< �м۴�����
	data["OrderDeleteByDisConnFlag"] = task_data.OrderDeleteByDisConnFlag;		///< �������ʧ��ʱ���������Զ�������ʶ
	this->onRspQryOrder(task.task_id, task.task_last, data);
}
void TdApi::processRspQryOrderProcess(Task task){
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
	data["OrderCanceledQty"] = task_data.OrderCanceledQty;				///< ��������
	data["MinClipSize"] = task_data.MinClipSize;					///< ��ɽ����С�����
	data["MaxClipSize"] = task_data.MaxClipSize;					///< ��ɽ����������
	data["LicenseNo"] = task_data.LicenseNo;						///< �����Ȩ��
	data["RefInt"] = task_data.RefInt;							///< ���Ͳο�ֵ
	data["RefString"] = task_data.RefString;						///< �ַ����ο�ֵ
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["ClientOrderNo"] = task_data.ClientOrderNo;					///< �ͻ��˱���ί�б��
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ���غ�
	data["OrderSystemNo"] = task_data.OrderSystemNo;					///< ϵͳ��
	data["OrderExchangeSystemNo"] = task_data.OrderExchangeSystemNo;			///< ������ϵͳ�� 
	data["OrderParentNo"] = task_data.OrderParentNo;					///< ������
	data["OrderParentSystemNo"] = task_data.OrderParentSystemNo;			///< ����ϵͳ��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
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
	data["OrderStreamID"] = task_data.OrderStreamID;					///< ί����ˮ��
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["ContractSize2"] = task_data.ContractSize2;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["FeeMode"] = task_data.FeeMode;						///< �����Ѽ��㷽ʽ
	data["FeeParam"] = task_data.FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
	data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
	data["FeeMode2"] = task_data.FeeMode2;						///< �����Ѽ��㷽ʽ
	data["FeeParam2"] = task_data.FeeParam2;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup2"] = task_data.FeeCurrencyGroup2;				///< �ͻ������ѱ�����
	data["FeeCurrency2"] = task_data.FeeCurrency2;					///< �ͻ������ѱ���
	data["MarginMode"] = task_data.MarginMode;						///< ��֤����㷽ʽ
	data["MarginParam"] = task_data.MarginParam;					///< ��֤�����ֵ
	data["MarginMode2"] = task_data.MarginMode2;					///< ��֤����㷽ʽ
	data["MarginParam2"] = task_data.MarginParam2;					///< ��֤�����ֵ
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������  ������ʽ���м۵�����϶���ʹ��
	data["PreSettlePrice2"] = task_data.PreSettlePrice2;				///< ������  ������ʽ���м۵�����϶���ʹ��
	data["OpenVol"] = task_data.OpenVol;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol"] = task_data.CoverVol;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["OpenVol2"] = task_data.OpenVol2;						///< Ԥ�������� ί�������еĿ��ֲ���
	data["CoverVol2"] = task_data.CoverVol2;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	data["FeeValue"] = task_data.FeeValue;						///< ����������
	data["MarginValue"] = task_data.MarginValue;					///< ���ᱣ֤�� �ϲ�����Ĵ���Ϊ0
	data["TacticsType"] = task_data.TacticsType;					///< ���Ե�����
	data["TriggerCondition"] = task_data.TriggerCondition;				///< ��������
	data["TriggerPriceType"] = task_data.TriggerPriceType;				///< �����۸�����
	data["AddOneIsValid"] = task_data.AddOneIsValid;					///< �Ƿ�T+1��Ч
	data["OrderQty2"] = task_data.OrderQty2;						///< ί������2
	data["HedgeFlag2"] = task_data.HedgeFlag2;						///< Ͷ����ֵ2
	data["MarketLevel"] = task_data.MarketLevel;					///< �м۴�����
	data["OrderDeleteByDisConnFlag"] = task_data.OrderDeleteByDisConnFlag;		///< �������ʧ��ʱ���������Զ�������ʶ
	this->onRspQryOrderProcess(task.task_id, task.task_last, data);
}
void TdApi::processRspQryFill(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFillInfo task_data = any_cast<TapAPIFillInfo>(task.task_data);
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
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< �Ƿ���ձ���
	data["MatchSide"] = task_data.MatchSide;						///< ��������
	data["PositionEffect"] = task_data.PositionEffect;					///< ��ƽ��־1
	data["PositionEffectRef"] = task_data.PositionEffectRef;				///< ��ƽ��־2
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ί�б��غ�
	data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
	data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< �������ɽ���
	data["MatchDateTime"] = task_data.MatchDateTime;					///< �ɽ�ʱ��
	data["UpperMatchDateTime"] = task_data.UpperMatchDateTime;				///< ���ֳɽ�ʱ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź��������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["MatchPrice"] = task_data.MatchPrice;						///< �ɽ���
	data["MatchQty"] = task_data.MatchQty;						///< �ɽ���
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsDeleted"] = task_data.IsDeleted;						///< ί�гɽ�ɾ����
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
	data["MatchStreamID"] = task_data.MatchStreamID;					///< ί����ˮ��
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< ��ƽ��ʽ
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["FeeMode"] = task_data.FeeMode;						///< �����Ѽ��㷽ʽ
	data["FeeParam"] = task_data.FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
	data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������
	data["FeeValue"] = task_data.FeeValue;						///< ������
	data["IsManualFee"] = task_data.IsManualFee;					///< �˹��ͻ������ѱ��
	data["Turnover"] = task_data.Turnover;						///< �ɽ����
	data["PremiumIncome"] = task_data.PremiumIncome;					///< Ȩ������ȡ
	data["PremiumPay"] = task_data.PremiumPay;						///< Ȩ����֧��
	data["OppoMatchNo"] = task_data.OppoMatchNo;					///< ���ֱ��سɽ��ţ����ֺ�ƽ�ֶ�Ӧ��ţ�
	data["CloseProfit"] = task_data.CloseProfit;					///< ƽ��ӯ��
	data["UnExpProfit"] = task_data.UnExpProfit;					///< δ����ƽӯ
	data["UpperMatchPrice"] = task_data.UpperMatchPrice;				///< ���ֳɽ���
	data["QuotePrice"] = task_data.QuotePrice;						///< ��ǰ�����
	data["ClosePL"] = task_data.ClosePL;                        ///< ���ƽӯ
	this->onRspQryFill(task.task_id, task.task_last, data);
}
void TdApi::processRtnFill(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIFillInfo task_data = any_cast<TapAPIFillInfo>(task.task_data);
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
	data["IsRiskOrder"] = task_data.IsRiskOrder;					///< �Ƿ���ձ���
	data["MatchSide"] = task_data.MatchSide;						///< ��������
	data["PositionEffect"] = task_data.PositionEffect;					///< ��ƽ��־1
	data["PositionEffectRef"] = task_data.PositionEffectRef;				///< ��ƽ��־2
	data["HedgeFlag"] = task_data.HedgeFlag;						///< Ͷ����ֵ
	data["ServerFlag"] = task_data.ServerFlag;						///< ��������ʶ
	data["OrderNo"] = task_data.OrderNo;						///< ί�б���
	data["OrderLocalNo"] = task_data.OrderLocalNo;					///< ί�б��غ�
	data["MatchNo"] = task_data.MatchNo;						///< ���سɽ���
	data["ExchangeMatchNo"] = task_data.ExchangeMatchNo;				///< �������ɽ���
	data["MatchDateTime"] = task_data.MatchDateTime;					///< �ɽ�ʱ��
	data["UpperMatchDateTime"] = task_data.UpperMatchDateTime;				///< ���ֳɽ�ʱ��
	data["UpperNo"] = task_data.UpperNo;						///< ���ֺ�
	data["UpperChannelNo"] = task_data.UpperChannelNo;					///< ����ͨ����
	data["UpperSettleNo"] = task_data.UpperSettleNo;					///< ��Ա�ź��������
	data["UpperUserNo"] = task_data.UpperUserNo;					///< ���ֵ�¼��
	data["TradeNo"] = task_data.TradeNo;						///< ���ױ���
	data["MatchPrice"] = task_data.MatchPrice;						///< �ɽ���
	data["MatchQty"] = task_data.MatchQty;						///< �ɽ���
	data["IsBackInput"] = task_data.IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	data["IsDeleted"] = task_data.IsDeleted;						///< ί�гɽ�ɾ����
	data["IsAddOne"] = task_data.IsAddOne;						///< �Ƿ�ΪT+1��
	data["MatchStreamID"] = task_data.MatchStreamID;					///< ί����ˮ��
	data["UpperStreamID"] = task_data.UpperStreamID;					///< ��������
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< ��ƽ��ʽ
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["FeeMode"] = task_data.FeeMode;						///< �����Ѽ��㷽ʽ
	data["FeeParam"] = task_data.FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	data["FeeCurrencyGroup"] = task_data.FeeCurrencyGroup;				///< �ͻ������ѱ�����
	data["FeeCurrency"] = task_data.FeeCurrency;					///< �ͻ������ѱ���
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������
	data["FeeValue"] = task_data.FeeValue;						///< ������
	data["IsManualFee"] = task_data.IsManualFee;					///< �˹��ͻ������ѱ��
	data["Turnover"] = task_data.Turnover;						///< �ɽ����
	data["PremiumIncome"] = task_data.PremiumIncome;					///< Ȩ������ȡ
	data["PremiumPay"] = task_data.PremiumPay;						///< Ȩ����֧��
	data["OppoMatchNo"] = task_data.OppoMatchNo;					///< ���ֱ��سɽ��ţ����ֺ�ƽ�ֶ�Ӧ��ţ�
	data["CloseProfit"] = task_data.CloseProfit;					///< ƽ��ӯ��
	data["UnExpProfit"] = task_data.UnExpProfit;					///< δ����ƽӯ
	data["UpperMatchPrice"] = task_data.UpperMatchPrice;				///< ���ֳɽ���
	data["QuotePrice"] = task_data.QuotePrice;						///< ��ǰ�����
	data["ClosePL"] = task_data.ClosePL;                        ///< ���ƽӯ
	this->onRtnFill(data);
}
void TdApi::processRspQryPosition(Task task){
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
}
void TdApi::processRtnPosition(Task task){
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
}
void TdApi::processRtnClose(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPICloseInfo task_data = any_cast<TapAPICloseInfo>(task.task_data);
	dict data;
	data["AccountNo"] = task_data.AccountNo;						///< �ͻ��ʽ��ʺ�
	data["ParentAccountNo"] = task_data.ParentAccountNo;				///< �ϼ��ʽ��˺�
	data["ExchangeNo"] = task_data.ExchangeNo;						///< ���������
	data["CommodityType"] = task_data.CommodityType;					///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;					///< Ʒ�ֱ�������
	data["ContractNo"] = task_data.ContractNo;						///< ��Լ
	data["StrikePrice"] = task_data.StrikePrice;					///< ִ�м۸�
	data["CallOrPutFlag"] = task_data.CallOrPutFlag;					///< ���ſ���
	data["OpenOrderExchangeNo"] = task_data.OpenOrderExchangeNo;			///< ���������
	data["OpenOrderCommodityType"] = task_data.OpenOrderCommodityType;			///< Ʒ������
	data["OpenOrderCommodityNo"] = task_data.OpenOrderCommodityNo;			///< Ʒ�ֱ�������
	data["CloseOrderExchangeNo"] = task_data.CloseOrderExchangeNo;			///< ���������
	data["CloseOrderCommodityType"] = task_data.CloseOrderCommodityType;		///< Ʒ������
	data["CloseOrderCommodityNo"] = task_data.CloseOrderCommodityNo;			///< Ʒ�ֱ�������
	data["OpenMatchSource"] = task_data.OpenMatchSource;
	data["CloseMatchSource"] = task_data.CloseMatchSource;
	data["CloseSide"] = task_data.CloseSide;						///< ƽ��һ�ߵ���������
	data["CloseQty"] = task_data.CloseQty;						///< ƽ�ֳɽ���
	data["OpenPrice"] = task_data.OpenPrice;						///< ���ֳɽ���
	data["ClosePrice"] = task_data.ClosePrice;						///< ƽ�ֳɽ���
	data["OpenServerFlag"] = task_data.OpenServerFlag;					///< ��������ʶ
	data["OpenOrderNo"] = task_data.OpenOrderNo;					///< ί�б���
	data["OpenMatchNo"] = task_data.OpenMatchNo;					///< ���سɽ���
	data["OpenExchangeMatchNo"] = task_data.OpenExchangeMatchNo;			///< �������ɽ���
	data["OpenMatchDateTime"] = task_data.OpenMatchDateTime;				///< �ɽ�ʱ��
	data["CloseServerFlag"] = task_data.CloseServerFlag;				///< ��������ʶ
	data["CloseOrderNo"] = task_data.CloseOrderNo;					///< ί�б���
	data["CloseMatchNo"] = task_data.CloseMatchNo;					///< ���سɽ���
	data["CloseExchangeMatchNo"] = task_data.CloseExchangeMatchNo;			///< �������ɽ���
	data["CloseMatchDateTime"] = task_data.CloseMatchDateTime;				///< �ɽ�ʱ��
	data["CloseStreamId"] = task_data.CloseStreamId;					///< ƽ������
	data["OpenCloseMode"] = task_data.OpenCloseMode;                  ///< ��ƽ��ʽ
	data["ContractSize"] = task_data.ContractSize;					///< ÿ�ֳ������������
	data["CommodityCurrencyGroup"] = task_data.CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	data["CommodityCurrency"] = task_data.CommodityCurrency;				///< Ʒ�ֱ���
	data["PreSettlePrice"] = task_data.PreSettlePrice;					///< ������
	data["PremiumIncome"] = task_data.PremiumIncome;					///< Ȩ������ȡ
	data["PremiumPay"] = task_data.PremiumPay;						///< Ȩ����֧��
	data["CloseProfit"] = task_data.CloseProfit;					///< ƽ��ӯ��
	data["UnExpProfit"] = task_data.UnExpProfit;					///< δ����ƽӯ
	data["ClosePL"] = task_data.ClosePL;                        ///< ���ƽӯ
	this->onRtnClose(data);
}
void TdApi::processRtnPositionProfit(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIPositionProfitNotice task_data = any_cast<TapAPIPositionProfitNotice>(task.task_data);
	dict data;
	this->onRtnReqQuoteNotice(data);
}
void TdApi::processRspQryDeepQuote(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIDeepQuoteQryRsp task_data = any_cast<TapAPIDeepQuoteQryRsp>(task.task_data);
	dict data;
	this->onRspQryDeepQuote(task.task_id, task.task_last, data);
}
void TdApi::processRspQryExchangeStateInfo(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIExchangeStateInfo task_data = any_cast<TapAPIExchangeStateInfo>(task.task_data);
	dict data;
	data["UpperChannelNo"] = task_data.UpperChannelNo;			///< ����ͨ�����
	data["ExchangeNo"] = task_data.ExchangeNo;				///< ���������
	data["CommodityType"] = task_data.CommodityType;			///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;			///< Ʒ�ֱ��
	data["ExchangeTime"] = task_data.ExchangeTime;			///< ������ʱ��
	data["TradingState"] = task_data.TradingState;			///< ������״̬
	this->onRspQryExchangeStateInfo(task.task_id, task.task_last, data);
}
void TdApi::processRtnExchangeStateInfo(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIExchangeStateInfo task_data = any_cast<TapAPIExchangeStateInfo>(task.task_data);
	dict data;
	data["UpperChannelNo"] = task_data.UpperChannelNo;			///< ����ͨ�����
	data["ExchangeNo"] = task_data.ExchangeNo;				///< ���������
	data["CommodityType"] = task_data.CommodityType;			///< Ʒ������
	data["CommodityNo"] = task_data.CommodityNo;			///< Ʒ�ֱ��
	data["ExchangeTime"] = task_data.ExchangeTime;			///< ������ʱ��
	data["TradingState"] = task_data.TradingState;			///< ������״̬
	this->onRtnExchangeStateInfo(data);
}
void TdApi::processRtnReqQuoteNotice(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIReqQuoteNotice task_data = any_cast<TapAPIReqQuoteNotice>(task.task_data);
	dict data;
	this->onRtnReqQuoteNotice(data);
}
void TdApi::processRspUpperChannelInfo(Task task){
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__); fflush(fp);
#endif
	PyLock lock;
	TapAPIUpperChannelInfo task_data = any_cast<TapAPIUpperChannelInfo>(task.task_data);
	dict data;
	this->onRspUpperChannelInfo(task.task_id, task.task_last, data);
}


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
	cout << ITapTrade::GetITapTradeAPIVersion() << endl;

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
	TapAPIQuoteLoginAuth stLoginAuth;
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

int TdApi::qryAccount(dict req)
{
	TapAPIAccQryReq stAccountQryReq;
	memset(&stAccountQryReq, 0, sizeof(stAccountQryReq));
	getStr(req, "AccountNo", stAccountQryReq.AccountNo);
	return api->QryAccount(&m_sessionID, &stAccountQryReq);
}

int TdApi::qryFund(dict req)
{
	TapAPIFundReq stFundReq;
	memset(&stFundReq, 0, sizeof(stFundReq));
	getStr(req, "AccountNo", stFundReq.AccountNo);
	return api->QryFund(&m_sessionID, &stFundReq);
}

int TdApi::qryExchange(dict req)
{
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
	TapAPINewOrder stNewOrder;
	getStr(req, "AccountNo", stNewOrder.AccountNo);			
	getStr(req, "ExchangeNo", stNewOrder.ExchangeNo);			
	getChar(req, "CommodityType", &(stNewOrder.CommodityType));
	getStr(req, "CommodityNo", stNewOrder.CommodityNo);			
	getStr(req, "ContractNo", stNewOrder.ContractNo);			
	getChar(req, "OrderType", &(stNewOrder.OrderType));
	getChar(req, "OrderSource", &(stNewOrder.OrderSource));
	getChar(req, "TimeInForce", &(stNewOrder.TimeInForce));
	getChar(req, "OrderSide", &(stNewOrder.OrderSide));
	getDouble(req, "OrderPrice", stNewOrder.OrderPrice);
	getInt(req, "OrderQty", stNewOrder.OrderQty);
	
	return api->InsertOrder(&m_sessionID, &stNewOrder);
}

int TdApi::reqCancelOrder(dict req)
{
	TapAPIOrderCancelReq stCancelReq;
	memset(&stCancelReq, 0, sizeof(stCancelReq));
	getStr(req, "OrderNo", stCancelReq.OrderNo);			
	return api->CancelOrder(&m_sessionID, &stCancelReq);
}

int TdApi::qryOrder(dict req)
{
	TapAPIOrderQryReq stOrderQryReq;
	getChar(req, "QryOrderType", &(stOrderQryReq.OrderQryType));
	return api->QryOrder(&m_sessionID, &stOrderQryReq);
}

int TdApi::qryFill()
{
	TapAPIFillQryReq stFillQryReq;
	return api->QryFill(&m_sessionID, &stFillQryReq);
}

int TdApi::qryPosition()
{
	TapAPIPositionQryReq stPositionReq;
	return api->QryPosition(&m_sessionID,&stPositionReq);
}


///-------------------------------------------------------------------------------------
///Boost.Python��װ
///-------------------------------------------------------------------------------------

struct TdApiWrap : TdApi, wrapper < TdApi >
{
	virtual void  onConnect()  {
		try
		{
			this->get_override("onConnect")();
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspLogin(int errorCode, dict data)  {
		try
		{
			this->get_override("onRspLogin")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onAPIReady()  {
		try
		{
			this->get_override("onAPIReady")();
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onDisconnect(int reasonCode)  {
		try
		{
			this->get_override("onDisconnect")(reasonCode);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspChangePassword( int errorCode)  {
		try
		{
			this->get_override("onRspChangePassword")(errorCode);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspSetReservedInfo( int errorCode, dict data)  {
		try
		{
			this->get_override("onRspSetReservedInfo")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryAccount( TAPIUINT32 errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryAccount")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryFund( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryFund")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnFund(dict data)  {
		try
		{
			this->get_override("onRtnFund")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryExchange( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryExchange")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryCommodity( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryCommodity")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryContract( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryContract")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnContract(dict data)  {
		try
		{
			this->get_override("onRtnContract")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnOrder(int errorCode, dict data)  {
		try
		{
			this->get_override("onRtnOrder")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspOrderAction( TAPIUINT32 errorCode, dict data)  {
		try
		{
			this->get_override("onRspOrderAction")(errorCode, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryOrder( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryOrder")(errorCode, isLast,data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryOrderProcess( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryOrderProcess")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryFill( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryFill")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnFill(dict data)  {
		try
		{
			this->get_override("onRtnFill")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryPosition( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryPosition")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnPosition(dict data)  {
		try
		{
			this->get_override("onRtnPosition")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnClose(dict data)  {
		try
		{
			this->get_override("onRtnClose")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnPositionProfit(dict data)  {
		try
		{
			this->get_override("onRtnPositionProfit")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryDeepQuote( int errorCode, bool isLast, dict data)  {
		try
		{
			this->get_override("onRspQryDeepQuote")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspQryExchangeStateInfo(int errorCode, bool isLast,dict data) {
		try
		{
			this->get_override("onRspQryExchangeStateInfo")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnExchangeStateInfo(dict data) {
		try
		{
			this->get_override("onRtnExchangeStateInfo")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRtnReqQuoteNotice(dict data)  {
		try
		{
			this->get_override("onRtnReqQuoteNotice")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;
	virtual void  onRspUpperChannelInfo(int errorCode, bool isLast, dict data) {
		try
		{
			this->get_override("onRspUpperChannelInfo")(errorCode, isLast, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	} ;

};

BOOST_PYTHON_MODULE(vnctptd)
{
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

	class_<TdApiWrap, boost::noncopyable>("TdApi")
		.def("createTapTraderApi", &TdApiWrap::createTapTraderApi)
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
        .def("onRtnOrder", pure_virtual(&TdApiWrap::onRtnOrder))
        .def("onRspOrderAction", pure_virtual(&TdApiWrap::onRspOrderAction))
        .def("onRspQryOrder", pure_virtual(&TdApiWrap::onRspQryOrder))
        .def("onRspQryOrderProcess", pure_virtual(&TdApiWrap::onRspQryOrderProcess))
        .def("onRspQryFill", pure_virtual(&TdApiWrap::onRspQryFill))
        .def("onRtnFill", pure_virtual(&TdApiWrap::onRtnFill))
        .def("onRspQryPosition", pure_virtual(&TdApiWrap::onRspQryPosition))
        .def("onRtnPosition", pure_virtual(&TdApiWrap::onRtnPosition))
        .def("onRtnClose", pure_virtual(&TdApiWrap::onRtnClose))
        .def("onRtnPositionProfit", pure_virtual(&TdApiWrap::onRtnPositionProfit))
        .def("onRspQryDeepQuote", pure_virtual(&TdApiWrap::onRspQryDeepQuote))
        .def("onRspQryExchangeStateInfo", pure_virtual(&TdApiWrap::onRspQryExchangeStateInfo))
        .def("onRtnExchangeStateInfo", pure_virtual(&TdApiWrap::onRtnExchangeStateInfo))
        .def("onRtnReqQuoteNotice", pure_virtual(&TdApiWrap::onRtnReqQuoteNotice))
        .def("onRspUpperChannelInfo", pure_virtual(&TdApiWrap::onRspUpperChannelInfo))

		;
}
