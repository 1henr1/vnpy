// vnhfpmd.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "vnhfptd.h"
#include "HFPTradingClient.h"

using namespace hfp;

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


void TdApi::processClientClosed(Task task)
{
	PyLock lock;
	int type = any_cast<int>(task.task_data);
	this->onClientClosed(type);
}

void TdApi::processClientConnected(Task task)
{
	PyLock lock;
	this->onClientConnected();
}

void TdApi::processClientDisConnected(Task task)
{
	PyLock lock;
	int code = any_cast<int>(task.task_data);
	this->onClientDisConnected(code);
}

void TdApi::processClienthandshaked(Task task)
{
	PyLock lock;
	bool IsSuccess = any_cast<bool>(task.task_error);
	int index = task.task_id;
	string code = any_cast<string>(task.task_data);
	this->onClienthandshaked(IsSuccess, index, code);
}

void TdApi::processQuotationInfo(Task task)
{
	PyLock lock;
	quotation_data quotaData = any_cast<quotation_data>(task.task_data);
	dict marketdata;

	marketdata["seq"]  = quotaData.seq;//��ˮ��
	marketdata["datetime"] = quotaData.datetime;//����ʱ��(��)
	marketdata["contract_id"] = (string)quotaData.contract_id;//��Լ����
	marketdata["contract_name"] = (string)quotaData.contract_name;//��Լ����
	marketdata["marketId"] = (string)quotaData.marketId;//�г�����
	marketdata["open"] = quotaData.open;//���̼�
	marketdata["high"] = quotaData.high;//��߼�
	marketdata["low"] = quotaData.low;//��ͼ�
	marketdata["new"] = quotaData._new;//���¼�
	marketdata["last_close"] = quotaData.last_close;//�������̼�
	marketdata["average"] = quotaData.average;//����
	marketdata["total_volume"] = quotaData.total_volume;//�ܳɽ���
	marketdata["total_amount"] = quotaData.total_amount;//�ܳɽ���
	marketdata["subs_volume"] = quotaData.subs_volume;//�ֲܳ���
	marketdata["cur_volume"] = quotaData.cur_volume;//����
	marketdata["type"] = quotaData.type;//�������� 1-���Ͼ����걨,2-���Ͼ��۳ɽ�,3-�걨,4-�ɽ�
	marketdata["down_limit"] = quotaData.down_limit;//����޼�
	marketdata["up_limit"] = quotaData.up_limit;//����޼�
	marketdata["balance_price"] = quotaData.balance_price;//ӯ�������
	marketdata["updown_base"] = quotaData.updown_base;//�ǵ�����׼��
	marketdata["last_subs_volume"] = quotaData.last_subs_volume;//���ճֲ�
	marketdata["buy_price_1"] = quotaData.buy_price[0];//���
	marketdata["buy_volume_1"] = quotaData.buy_volume[0];//����
	marketdata["sell_price_1"] = quotaData.sell_price[0];//����
	marketdata["sell_volume_1"] = quotaData.sell_volume[0];//����

	this->onQuotationInfo(marketdata);
}


///-------------------------------------------------------------------------------------
///��������
///-------------------------------------------------------------------------------------
void TdApi::createHFPTdApi(string id, string license)
{
	//�����ͻ���
	clientSeq = client(id.c_str(), license.c_str(), true, hfp::client_type::trading);//����

	//���ûص�����
	setuserversion(clientSeq,"AFIS_C++_1.0.0");
	setkeepalive(clientSeq, true, 5000, 5000);
	setonconnected(clientSeq, TdApi::OnClientConnected);
	setonconnectfail(clientSeq, TdApi::OnClientDisConnected);
	setonclosed(clientSeq, TdApi::OnClientClosed);
	setonhandshaked(clientSeq, TdApi::OnClienthandshaked);
	setonlogin_response(clientSeq, TdApi::OnLoginResponse);
	setonlogout_push(clientSeq, TdApi::OnLogoutPush);
	setonmarketstate_push(clientSeq, TdApi::OnMarketStatePush);
	setonassociator_response(clientSeq, TdApi::OnAssociatorResponse);
	setonmarket_response(clientSeq, TdApi::OnMarketResponse);
	setoncontract_response(clientSeq, TdApi::OnContractResponse);
	setonaccount_response(clientSeq, TdApi::OnAccountResponse);
	setonreceiptcollect_response(clientSeq, TdApi::OnReceiptcollectResponse);
	setonorder_response(clientSeq, TdApi::OnOrderResponse);
	setonqueryorder_response(clientSeq, TdApi::OnQueryorderResponse);
	setoncancelorder_push(clientSeq, TdApi::OnCancelorderPush);
	setondeal_push(clientSeq, TdApi::OnDealPush);
	setonquerydeal_response(clientSeq, TdApi::OnQuerydealResponse);
	setonquerypositioncollect_response(clientSeq, TdApi::OnQueryPositioncollectResponse);
	setonquerypositiondetail_response(clientSeq, TdApi::OnQueryPositiondetailResponse);
	setonquerydepositinfo_response(clientSeq, TdApi::OnQueryDepositinfoResponse);
	setonqueryfeeinfo_response(clientSeq, TdApi::OnQueryfeeinfoResponse);
};

void TdApi::init()
{
	//����
	connect(clientSeq, "58.215.39.218", 5566);//����
}


void TdApi::connectTradeFront(string tradeFrontAddress, int tradePort)
{
	connect(clientSeq, (char*)tradeFrontAddress.c_str(), tradePort);
}

long long TdApi::getservertime(CLIENT client)
{
	return 0;
}


/*
int TdApi::reqUserLogin(dict req, int nRequestID)
{
	CThostFtdcReqUserLoginField myreq = CThostFtdcReqUserLoginField();
	memset(&myreq, 0, sizeof(myreq));
	getStr(req, "MacAddress", myreq.MacAddress);
	getStr(req, "UserProductInfo", myreq.UserProductInfo);
	getStr(req, "UserID", myreq.UserID);
	getStr(req, "TradingDay", myreq.TradingDay);
	getStr(req, "InterfaceProductInfo", myreq.InterfaceProductInfo);
	getStr(req, "BrokerID", myreq.BrokerID);
	getStr(req, "ClientIPAddress", myreq.ClientIPAddress);
	getStr(req, "OneTimePassword", myreq.OneTimePassword);
	getStr(req, "ProtocolInfo", myreq.ProtocolInfo);
	getStr(req, "Password", myreq.Password);
	int i = this->api->ReqUserLogin(&myreq, nRequestID);
	return i;
};

int TdApi::reqUserLogout(dict req, int nRequestID)
{
	CThostFtdcUserLogoutField myreq = CThostFtdcUserLogoutField();
	memset(&myreq, 0, sizeof(myreq));
	getStr(req, "UserID", myreq.UserID);
	getStr(req, "BrokerID", myreq.BrokerID);
	int i = this->api->ReqUserLogout(&myreq, nRequestID);
	return i;
};

*/



///-------------------------------------------------------------------------------------
///Boost.Python��װ
///-------------------------------------------------------------------------------------

struct TdApiWrap : TdApi, wrapper < TdApi >
{
	virtual void onClientClosed(int type)
	{
		//���µ�try...catch...����ʵ�ֲ�׽python�����д���Ĺ��ܣ���ֹC++ֱ�ӳ���ԭ��δ֪�ı���
		try
		{
			this->get_override("onClientClosed")(type);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onClientConnected()
	{
		try
		{
			this->get_override("onClientConnected")();
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onClientDisConnected(int code)
	{
		try
		{
			this->get_override("onClientDisConnected")(code);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onClienthandshaked(bool IsSuccess, int index, string code)
	{
		try
		{
			this->get_override("onClienthandshaked")(IsSuccess, index, code);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onQuotationInfo(dict data)
	{
		try
		{
			this->get_override("onQuotationInfo")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	
};


BOOST_PYTHON_MODULE(vnhfpmd)
{
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

	class_<TdApiWrap, boost::noncopyable>("TdApi")
		.def("createHFPTdApi", &TdApiWrap::createHFPTdApi)
		.def("init", &TdApiWrap::init)
		.def("connectMdFront",&TdApiWrap::connectMdFront)
		.def("onClientClosed", pure_virtual(&TdApiWrap::onClientClosed))
		.def("onClientConnected", pure_virtual(&TdApiWrap::onClientConnected))
		.def("onClientDisConnected", pure_virtual(&TdApiWrap::onClientDisConnected))
		.def("onClienthandshaked", pure_virtual(&TdApiWrap::onClienthandshaked))
		.def("onQuotationInfo", pure_virtual(&TdApiWrap::onQuotationInfo))
		;
};
