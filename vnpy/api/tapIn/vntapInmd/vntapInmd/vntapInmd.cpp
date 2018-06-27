// vntapInmd.cpp : ���� DLL Ӧ�ó���ĵ���������
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
			//���ַ���ָ�븳ֵ����ʹ��strcpy_s, vs2013ʹ��strcpy����ͨ����
			//+1Ӧ������ΪC++�ַ����Ľ�β���ţ������ر�ȷ�����������1�����
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
///C++�Ļص����������ݱ��浽������
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
///�����̴߳Ӷ�����ȡ�����ݣ�ת��Ϊpython����󣬽�������
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
	data["UserNo"] = TOSTRING(task_data.UserNo);							///< �û���
	data["UserType"] = task_data.UserType;						///< �û�����
	data["UserName"] = TOSTRING(task_data.UserName);						///< �ǳƣ�GBK�����ʽ
	data["QuoteTempPassword"] = TOSTRING(task_data.QuoteTempPassword);				///< ������ʱ����
	data["ReservedInfo"] = TOSTRING(task_data.ReservedInfo);					///< �û��Լ����õ�Ԥ����Ϣ
	data["LastLoginIP"] = TOSTRING(task_data.LastLoginIP);					///< �ϴε�¼�ĵ�ַ
	data["LastLoginProt"] = task_data.LastLoginProt;					///< �ϴε�¼ʹ�õĶ˿�
	data["LastLoginTime"] = TOSTRING(task_data.LastLoginTime);					///< �ϴε�¼��ʱ��
	data["LastLogoutTime"] = TOSTRING(task_data.LastLogoutTime);					///< �ϴ��˳���ʱ��
	data["TradeDate"] = TOSTRING(task_data.TradeDate);						///< ��ǰ��������
	data["LastSettleTime"] = TOSTRING(task_data.LastSettleTime);					///< �ϴν���ʱ��
	data["StartTime"] = TOSTRING(task_data.StartTime);						///< ϵͳ����ʱ��
	data["InitTime"] = TOSTRING(task_data.InitTime);						///< ϵͳ��ʼ��ʱ��
	
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
	data["Commodity.ExchangeNo"] = task_data.Commodity.ExchangeNo;                             ///< ����������
	data["Commodity.CommodityType"] = task_data.Commodity.CommodityType;                          ///< Ʒ������
	data["Commodity.CommodityNo"] = task_data.Commodity.CommodityNo;
	data["CommodityName"] = task_data.CommodityName;						///< Ʒ������,GBK�����ʽ
	data["CommodityEngName"] = task_data.CommodityEngName;					///< Ʒ��Ӣ������
	data["ContractSize"] = task_data.ContractSize;						///< ÿ�ֳ���
	data["CommodityTickSize"] = task_data.CommodityTickSize;					///< ��С�䶯��λ
	data["CommodityDenominator"] = task_data.CommodityDenominator;				///< ���۷�ĸ
	data["CmbDirect"] = task_data.CmbDirect;							///< ��Ϸ���
	data["CommodityContractLen"] = task_data.CommodityContractLen;				///< Ʒ�ֺ�Լ����
	data["IsDST"] = task_data.IsDST;								///< �Ƿ�����ʱ,'Y'Ϊ��,'N'Ϊ��
	//data["RelateCommodity1"] = task_data.RelateCommodity1;					///< ����Ʒ��1
	//data["RelateCommodity2"] = task_data.RelateCommodity2;					///< ����Ʒ��2

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
	data["ExchangeNo"] = task_data.Contract.Commodity.ExchangeNo;                             ///< ����������
	data["CommodityNo"] = task_data.Contract.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.Contract.ContractNo1;                            ///< ��Լ����1
	data["StrikePrice1"] = task_data.Contract.StrikePrice1;                           ///< ִ�м�1
	data["CallOrPutFlag1"] = task_data.Contract.CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	data["ContractNo2"] = task_data.Contract.ContractNo2;                            ///< ��Լ����2
	data["StrikePrice2"] = task_data.Contract.StrikePrice2;                           ///< ִ�м�2
	data["CallOrPutFlag2"] = task_data.Contract.CallOrPutFlag2;
	data["ContractType"] = task_data.ContractType;                       ///< ��Լ����,'1'��ʾ���������Լ,'2'��ʾ�����Լ
	data["QuoteUnderlyingContract"] = task_data.QuoteUnderlyingContract;			///< ������ʵ��Լ
	data["ContractName"] = task_data.ContractName;                       ///< ��Լ����
	data["ContractExpDate"] = task_data.ContractExpDate;                    ///< ��Լ������	
	data["LastTradeDate"] = task_data.LastTradeDate;                      ///< �������
	data["FirstNoticeDate"] = task_data.FirstNoticeDate;                    ///< �״�֪ͨ��
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
	data["ExchangeNo"] = task_data.Contract.Commodity.ExchangeNo;                             ///< ����������
	data["CommodityNo"] = task_data.Contract.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.Contract.ContractNo1;                            ///< ��Լ����1
	data["StrikePrice1"] = task_data.Contract.StrikePrice1;                           ///< ִ�м�1
	data["CallOrPutFlag1"] = task_data.Contract.CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	data["ContractNo2"] = task_data.Contract.ContractNo2;                            ///< ��Լ����2
	data["StrikePrice2"] = task_data.Contract.StrikePrice2;                           ///< ִ�м�2
	data["CallOrPutFlag2"] = task_data.Contract.CallOrPutFlag2;
	data["CurrencyNo"] = task_data.CurrencyNo;						///< ���ֱ��
	data["TradingState"] = task_data.TradingState;					///< ����״̬��1,���Ͼ���;2,���Ͼ��۴��;3,��������;4,������ͣ;5,����
	data["DateTimeStamp"] = task_data.DateTimeStamp;					///< ʱ���
	data["QPreClosingPrice"] = task_data.QPreClosingPrice;				///< �����̼�
	data["QPreSettlePrice"] = task_data.QPreSettlePrice;				///< ������
	data["QPrePositionQty"] = task_data.QPrePositionQty;				///< ��ֲ���
	data["QOpeningPrice"] = task_data.QOpeningPrice;					///< ���̼�
	data["QLastPrice"] = task_data.QLastPrice;						///< ���¼�
	data["QHighPrice"] = task_data.QHighPrice;						///< ��߼�
	data["QLowPrice"] = task_data.QLowPrice;						///< ��ͼ�
	data["QHisHighPrice"] = task_data.QHisHighPrice;					///< ��ʷ��߼�
	data["QHisLowPrice"] = task_data.QHisLowPrice;					///< ��ʷ��ͼ�
	data["QLimitUpPrice"] = task_data.QLimitUpPrice;					///< ��ͣ��
	data["QLimitDownPrice"] = task_data.QLimitDownPrice;				///< ��ͣ��
	data["QTotalQty"] = task_data.QTotalQty;						///< �����ܳɽ���
	data["QTotalTurnover"] = task_data.QTotalTurnover;					///< ���ճɽ����
	data["QPositionQty"] = task_data.QPositionQty;					///< �ֲ���
	data["QAveragePrice"] = task_data.QAveragePrice;					///< ����
	data["QClosingPrice"] = task_data.QClosingPrice;					///< ���̼�
	data["QSettlePrice"] = task_data.QSettlePrice;					///< �����
	data["QLastQty"] = task_data.QLastQty;						///< ���³ɽ���
	data["QBidPrice1"] = task_data.QBidPrice[0];					///< ���1-20��
	data["QBidPrice2"] = task_data.QBidPrice[1];					///< ���1-20��
	data["QBidPrice3"] = task_data.QBidPrice[2];					///< ���1-20��
	data["QBidPrice4"] = task_data.QBidPrice[3];					///< ���1-20��
	data["QBidPrice5"] = task_data.QBidPrice[4];					///< ���1-20��
	data["QBidPrice6"] = task_data.QBidPrice[5];					///< ���1-20��
	data["QBidPrice7"] = task_data.QBidPrice[6];					///< ���1-20��
	data["QBidPrice8"] = task_data.QBidPrice[7];					///< ���1-20��
	data["QBidPrice9"] = task_data.QBidPrice[8];					///< ���1-20��
	data["QBidPrice10"] = task_data.QBidPrice[9];					///< ���1-20��
	data["QBidQty1"] = task_data.QBidQty[0];					///< ���1-20��
	data["QBidQty2"] = task_data.QBidQty[1];					///< ���1-20��
	data["QBidQty3"] = task_data.QBidQty[2];					///< ���1-20��
	data["QBidQty4"] = task_data.QBidQty[3];					///< ���1-20��
	data["QBidQty5"] = task_data.QBidQty[4];					///< ���1-20��
	data["QBidQty6"] = task_data.QBidQty[5];					///< ���1-20��
	data["QBidQty7"] = task_data.QBidQty[6];					///< ���1-20��
	data["QBidQty8"] = task_data.QBidQty[7];					///< ���1-20��
	data["QBidQty9"] = task_data.QBidQty[8];					///< ���1-20��
	data["QBidQty10"] = task_data.QBidQty[9];					///< ���1-20��
	data["QAskPrice1"] = task_data.QAskPrice[0];					///< ���1-20��
	data["QAskPrice2"] = task_data.QAskPrice[1];					///< ���1-20��
	data["QAskPrice3"] = task_data.QAskPrice[2];					///< ���1-20��
	data["QAskPrice4"] = task_data.QAskPrice[3];					///< ���1-20��
	data["QAskPrice5"] = task_data.QAskPrice[4];					///< ���1-20��
	data["QAskPrice6"] = task_data.QAskPrice[5];					///< ���1-20��
	data["QAskPrice7"] = task_data.QAskPrice[6];					///< ���1-20��
	data["QAskPrice8"] = task_data.QAskPrice[7];					///< ���1-20��
	data["QAskPrice9"] = task_data.QAskPrice[8];					///< ���1-20��
	data["QAskPrice10"] = task_data.QAskPrice[9];					///< ���1-20��
	data["QAskQty1"] = task_data.QAskQty[0];					///< ���1-20��
	data["QAskQty2"] = task_data.QAskQty[1];					///< ���1-20��
	data["QAskQty3"] = task_data.QAskQty[2];					///< ���1-20��
	data["QAskQty4"] = task_data.QAskQty[3];					///< ���1-20��
	data["QAskQty5"] = task_data.QAskQty[4];					///< ���1-20��
	data["QAskQty6"] = task_data.QAskQty[5];					///< ���1-20��
	data["QAskQty7"] = task_data.QAskQty[6];					///< ���1-20��
	data["QAskQty8"] = task_data.QAskQty[7];					///< ���1-20��
	data["QAskQty9"] = task_data.QAskQty[8];					///< ���1-20��
	data["QAskQty10"] = task_data.QAskQty[9];					///< ���1-20��
	data["QImpliedBidPrice"] = task_data.QImpliedBidPrice;				///< �������
	data["QImpliedBidQty"] = task_data.QImpliedBidQty;					///< ��������
	data["QImpliedAskPrice"] = task_data.QImpliedAskPrice;				///< ��������
	data["QImpliedAskQty"] = task_data.QImpliedAskQty;					///< ��������
	data["QPreDelta"] = task_data.QPreDelta;						///< ����ʵ��
	data["QCurrDelta"] = task_data.QCurrDelta;						///< ����ʵ��
	data["QInsideQty"] = task_data.QInsideQty;						///< ������
	data["QOutsideQty"] = task_data.QOutsideQty;					///< ������
	data["QTurnoverRate"] = task_data.QTurnoverRate;					///< ������
	data["Q5DAvgQty"] = task_data.Q5DAvgQty;						///< ���վ���
	data["QPERatio"] = task_data.QPERatio;						///< ��ӯ��
	data["QTotalValue"] = task_data.QTotalValue;					///< ����ֵ
	data["QNegotiableValue"] = task_data.QNegotiableValue;				///< ��ͨ��ֵ
	data["QPositionTrend"] = task_data.QPositionTrend;					///< �ֲ�����
	data["QChangeSpeed"] = task_data.QChangeSpeed;					///< ����
	data["QChangeRate"] = task_data.QChangeRate;					///< �Ƿ�
	data["QChangeValue"] = task_data.QChangeValue;					///< �ǵ�ֵ
	data["QSwing"] = task_data.QSwing;							///< ���
	data["QTotalBidQty"] = task_data.QTotalBidQty;					///< ί������
	data["QTotalAskQty"] = task_data.QTotalAskQty;					///< ί������

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
	data["Commodity.ExchangeNo"] = task_data.Commodity.ExchangeNo;                             ///< ����������
	data["Commodity.CommodityType"] = task_data.Commodity.CommodityType;                          ///< Ʒ������
	data["Commodity.CommodityNo"] = task_data.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.ContractNo1;                            ///< ��Լ����1
	data["StrikePrice1"] = task_data.StrikePrice1;                           ///< ִ�м�1
	data["CallOrPutFlag1"] = task_data.CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	data["ContractNo2"] = task_data.ContractNo2;                            ///< ��Լ����2
	data["StrikePrice2"] = task_data.StrikePrice2;                           ///< ִ�м�2
	data["CallOrPutFlag2"] = task_data.CallOrPutFlag2;                         ///< ���ǿ�����ʾ2
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
	data["ExchangeNo"] = task_data.Contract.Commodity.ExchangeNo;                             ///< ����������
	data["CommodityNo"] = task_data.Contract.Commodity.CommodityNo;
	data["ContractNo1"] = task_data.Contract.ContractNo1;                            ///< ��Լ����1
	data["StrikePrice1"] = task_data.Contract.StrikePrice1;                           ///< ִ�м�1
	data["CallOrPutFlag1"] = task_data.Contract.CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	data["ContractNo2"] = task_data.Contract.ContractNo2;                            ///< ��Լ����2
	data["StrikePrice2"] = task_data.Contract.StrikePrice2;                           ///< ִ�м�2
	data["CallOrPutFlag2"] = task_data.Contract.CallOrPutFlag2;
	data["CurrencyNo"] = task_data.CurrencyNo;						///< ���ֱ��
	data["TradingState"] = task_data.TradingState;					///< ����״̬��1,���Ͼ���;2,���Ͼ��۴��;3,��������;4,������ͣ;5,����
	data["DateTimeStamp"] = task_data.DateTimeStamp;					///< ʱ���
	data["QPreClosingPrice"] = task_data.QPreClosingPrice;				///< �����̼�
	data["QPreSettlePrice"] = task_data.QPreSettlePrice;				///< ������
	data["QPrePositionQty"] = task_data.QPrePositionQty;				///< ��ֲ���
	data["QOpeningPrice"] = task_data.QOpeningPrice;					///< ���̼�
	data["QLastPrice"] = task_data.QLastPrice;						///< ���¼�
	data["QHighPrice"] = task_data.QHighPrice;						///< ��߼�
	data["QLowPrice"] = task_data.QLowPrice;						///< ��ͼ�
	data["QHisHighPrice"] = task_data.QHisHighPrice;					///< ��ʷ��߼�
	data["QHisLowPrice"] = task_data.QHisLowPrice;					///< ��ʷ��ͼ�
	data["QLimitUpPrice"] = task_data.QLimitUpPrice;					///< ��ͣ��
	data["QLimitDownPrice"] = task_data.QLimitDownPrice;				///< ��ͣ��
	data["QTotalQty"] = task_data.QTotalQty;						///< �����ܳɽ���
	data["QTotalTurnover"] = task_data.QTotalTurnover;					///< ���ճɽ����
	data["QPositionQty"] = task_data.QPositionQty;					///< �ֲ���
	data["QAveragePrice"] = task_data.QAveragePrice;					///< ����
	data["QClosingPrice"] = task_data.QClosingPrice;					///< ���̼�
	data["QSettlePrice"] = task_data.QSettlePrice;					///< �����
	data["QLastQty"] = task_data.QLastQty;						///< ���³ɽ���
	data["QBidPrice1"] = task_data.QBidPrice[0];					///< ���1-20��
	data["QBidPrice2"] = task_data.QBidPrice[1];					///< ���1-20��
	data["QBidPrice3"] = task_data.QBidPrice[2];					///< ���1-20��
	data["QBidPrice4"] = task_data.QBidPrice[3];					///< ���1-20��
	data["QBidPrice5"] = task_data.QBidPrice[4];					///< ���1-20��
	data["QBidPrice6"] = task_data.QBidPrice[5];					///< ���1-20��
	data["QBidPrice7"] = task_data.QBidPrice[6];					///< ���1-20��
	data["QBidPrice8"] = task_data.QBidPrice[7];					///< ���1-20��
	data["QBidPrice9"] = task_data.QBidPrice[8];					///< ���1-20��
	data["QBidPrice10"] = task_data.QBidPrice[9];					///< ���1-20��
	data["QBidQty1"] = task_data.QBidQty[0];					///< ���1-20��
	data["QBidQty2"] = task_data.QBidQty[1];					///< ���1-20��
	data["QBidQty3"] = task_data.QBidQty[2];					///< ���1-20��
	data["QBidQty4"] = task_data.QBidQty[3];					///< ���1-20��
	data["QBidQty5"] = task_data.QBidQty[4];					///< ���1-20��
	data["QBidQty6"] = task_data.QBidQty[5];					///< ���1-20��
	data["QBidQty7"] = task_data.QBidQty[6];					///< ���1-20��
	data["QBidQty8"] = task_data.QBidQty[7];					///< ���1-20��
	data["QBidQty9"] = task_data.QBidQty[8];					///< ���1-20��
	data["QBidQty10"] = task_data.QBidQty[9];					///< ���1-20��
	data["QAskPrice1"] = task_data.QAskPrice[0];					///< ���1-20��
	data["QAskPrice2"] = task_data.QAskPrice[1];					///< ���1-20��
	data["QAskPrice3"] = task_data.QAskPrice[2];					///< ���1-20��
	data["QAskPrice4"] = task_data.QAskPrice[3];					///< ���1-20��
	data["QAskPrice5"] = task_data.QAskPrice[4];					///< ���1-20��
	data["QAskPrice6"] = task_data.QAskPrice[5];					///< ���1-20��
	data["QAskPrice7"] = task_data.QAskPrice[6];					///< ���1-20��
	data["QAskPrice8"] = task_data.QAskPrice[7];					///< ���1-20��
	data["QAskPrice9"] = task_data.QAskPrice[8];					///< ���1-20��
	data["QAskPrice10"] = task_data.QAskPrice[9];					///< ���1-20��
	data["QAskQty1"] = task_data.QAskQty[0];					///< ���1-20��
	data["QAskQty2"] = task_data.QAskQty[1];					///< ���1-20��
	data["QAskQty3"] = task_data.QAskQty[2];					///< ���1-20��
	data["QAskQty4"] = task_data.QAskQty[3];					///< ���1-20��
	data["QAskQty5"] = task_data.QAskQty[4];					///< ���1-20��
	data["QAskQty6"] = task_data.QAskQty[5];					///< ���1-20��
	data["QAskQty7"] = task_data.QAskQty[6];					///< ���1-20��
	data["QAskQty8"] = task_data.QAskQty[7];					///< ���1-20��
	data["QAskQty9"] = task_data.QAskQty[8];					///< ���1-20��
	data["QAskQty10"] = task_data.QAskQty[9];					///< ���1-20��
	data["QImpliedBidPrice"] = task_data.QImpliedBidPrice;				///< �������
	data["QImpliedBidQty"] = task_data.QImpliedBidQty;					///< ��������
	data["QImpliedAskPrice"] = task_data.QImpliedAskPrice;				///< ��������
	data["QImpliedAskQty"] = task_data.QImpliedAskQty;					///< ��������
	data["QPreDelta"] = task_data.QPreDelta;						///< ����ʵ��
	data["QCurrDelta"] = task_data.QCurrDelta;						///< ����ʵ��
	data["QInsideQty"] = task_data.QInsideQty;						///< ������
	data["QOutsideQty"] = task_data.QOutsideQty;					///< ������
	data["QTurnoverRate"] = task_data.QTurnoverRate;					///< ������
	data["Q5DAvgQty"] = task_data.Q5DAvgQty;						///< ���վ���
	data["QPERatio"] = task_data.QPERatio;						///< ��ӯ��
	data["QTotalValue"] = task_data.QTotalValue;					///< ����ֵ
	data["QNegotiableValue"] = task_data.QNegotiableValue;				///< ��ͨ��ֵ
	data["QPositionTrend"] = task_data.QPositionTrend;					///< �ֲ�����
	data["QChangeSpeed"] = task_data.QChangeSpeed;					///< ����
	data["QChangeRate"] = task_data.QChangeRate;					///< �Ƿ�
	data["QChangeValue"] = task_data.QChangeValue;					///< �ǵ�ֵ
	data["QSwing"] = task_data.QSwing;							///< ���
	data["QTotalBidQty"] = task_data.QTotalBidQty;					///< ί������
	data["QTotalAskQty"] = task_data.QTotalAskQty;					///< ί������
	this->onRtnQuote(data);
}


///-------------------------------------------------------------------------------------
///��������
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
	
	//ȡ��API�İ汾��Ϣ
	typedef const TAPICHAR* (*GET_VERSION_FUNC)();
	GET_VERSION_FUNC pGetVersion = (GET_VERSION_FUNC)GetProcAddress(m_dll, "GetTapQuoteAPIVersion");
	cout << pGetVersion() << endl;
	//cout << GetTapQuoteAPIVersion() << endl;

	typedef ITapQuoteAPI* (*CREATE_QUOTEAPI_FUNC) (const TapAPIApplicationInfo *appInfo, TAPIINT32 &iResult);
	CREATE_QUOTEAPI_FUNC pCreateQuoteAPI = (CREATE_QUOTEAPI_FUNC)(GetProcAddress(m_dll, "CreateTapQuoteAPI"));


	//����APIʵ��
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	memset(&stAppInfo, 0, sizeof(stAppInfo));
	getStr(req, "AuthCode", stAppInfo.AuthCode);
	getStr(req, "KeyOperationLogPath", stAppInfo.KeyOperationLogPath);
	//api = CreateTapQuoteAPI(&stAppInfo, iResult);
	api = pCreateQuoteAPI(&stAppInfo, iResult);
	if (NULL == api){
		cout << "����APIʵ��ʧ�ܣ������룺" << iResult << endl;
		return -1;
	}

	//�趨ITapQuoteAPINotify��ʵ���࣬�����첽��Ϣ�Ľ���
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
	//��¼������
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
///Boost.Python��װ
///-------------------------------------------------------------------------------------

struct MdApiWrap : MdApi, wrapper < MdApi >
{

	virtual void onRspLogin(int errorCode, dict data)
	{
		//���µ�try...catch...����ʵ�ֲ�׽python�����д���Ĺ��ܣ���ֹC++ֱ�ӳ���ԭ��δ֪�ı���
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
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

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
