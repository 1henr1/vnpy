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

void TdApi::processLoginResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	response rsp = any_cast<response>(task.task_data);
	dict ploginResponse;
	ploginResponse["success"] = rsp.success;
	ploginResponse["errcode"] = rsp.errcode;
	ploginResponse["errdesc"] = rsp.errdesc;
	ploginResponse["sequence"] = rsp.sequence;
	this->onLoginResponse(ploginResponse);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processLogoutPush(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	outtype type = any_cast<outtype>(task.task_data);
	int ptype = type;
	this->onLogoutPush(ptype);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processMarketStatePush(Task task)
{
	PyLock lock;
	string  instrument = any_cast<string>(task.task_data);
	int status = task.task_id;
	dict pMarketState;
	pMarketState["instrument"] = instrument;
	pMarketState["status"] = status;
	this->onMarketStatePush(pMarketState);
}

void TdApi::processMarketResponse(Task task)
{
	PyLock lock;
	market marketinfo = any_cast<market>(task.task_data);
	dict pdict;
	pdict["curname"] = marketinfo.curname;
	pdict["id"] = marketinfo.id;
	pdict["name"] = marketinfo.name;
	this->onMarketResponse(pdict);
}

void TdApi::processContractResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	contract contractinfo = any_cast<contract>(task.task_data);
	dict pdict;
	pdict["contractid"] = (string)(contractinfo.contractid);//��Լ����
	pdict["contractname"] = (string)(contractinfo.contractname);//��Լ����
	pdict["initprice"] = contractinfo.initprice;//��ʼ��
	pdict["beginday"] = (string)(contractinfo.beginday);//��ʼ������(yymmddhh24mi)
	pdict["endday"] = (string)(contractinfo.endday);//����������(yymmddhh24mi)
	pdict["uplimit"] = contractinfo.uplimit;//����޼�
	pdict["downlimit"] = contractinfo.downlimit;//����޼�
	pdict["maxqty"] = contractinfo.maxqty;//��������걨��
	pdict["minqty"] = contractinfo.minqty;//������С�걨��
	pdict["mindiffprice"] = contractinfo.mindiffprice;//��С�䶯��
	pdict["mindeliveryqty"] = contractinfo.mindeliveryqty;//��С������
	pdict["taxrate"] = contractinfo.taxrate;//˰��
	pdict["positionlimit"] = contractinfo.positionlimit;//ȫ�г��޲�
	pdict["fuseuplimit"] = contractinfo.fuseuplimit;//�۶�����
	pdict["fusedownlimit"] = contractinfo.fusedownlimit;//�۶�����
	this->onContractResponse(pdict);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processAccountResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	account item = any_cast<account>(task.task_data);
	dict pdict;
	pdict["memberid"] = (string)item.memberid;//��Ա����
	pdict["curname"] = (string)item.curname;//��������
	pdict["avlb"] = item.avlb;//�����ʽ�
	pdict["occp"] = item.occp;//��֤��(ʵ��)
	pdict["frzord"] = item.frzord;//��������
	pdict["frzrisk"] = item.frzrisk;//��ض���
	pdict["ins"] = item.ins;//���
	pdict["outs"] = item.outs;//����
	pdict["poundage"] = item.poundage;//����������
	pdict["cnybalance"] = item.cnybalance;//ת��ӯ��
	pdict["managefee"] = item.managefee;//���
	pdict["adjust"] = item.adjust;//�����ʽ�
	pdict["financing"] = item.financing;//�����ʽ�
	pdict["balance"] = item.balance;//����۲�
	pdict["funddynamic"] = item.funddynamic;//��̬Ȩ��
	pdict["fundstatic"] = item.fundstatic;//��̬̬Ȩ��
	pdict["frzpayment"] = item.frzpayment;//�������
	pdict["fundonway"] = item.fundonway;//�����ʽ�
	pdict["occpshould"] = item.occpshould;//��֤��(Ӧ��)
	pdict["avlbfundout"] = item.avlbfundout;//�ɳ��ʽ�
	this->onAccountResponse(pdict);
}

void TdApi::processReceiptcollectResponse(Task task)
{
	PyLock lock;
	receipt_collect item = any_cast<receipt_collect>(task.task_data);
	dict pdict;
	pdict["memberid"] = (string)item.memberid;//��Ա����
	pdict["goodsid"] = (string)item.goodsid;//��Ʒ����
	pdict["goodsname"] = (string)item.goodsname;//��Ʒ����
	pdict["totalqty"] = item.totalqty;//����
	pdict["avlbqty"] = item.avlbqty;//������
	pdict["frzord"] = item.frzord;//����������
	pdict["frzrisk"] = item.frzrisk;//��ض�����
	pdict["occp"] = item.occp;//�ֲ�ռ����
	this->onReceiptcollectResponse(pdict);
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

void TdApi::connectTradeFront(string tradeFrontAddress, int tradePort)
{
	connect(clientSeq, (char*)tradeFrontAddress.c_str(), tradePort);
}

SEQ TdApi::reqUserLogin(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	char userID[100];
	char password[100];
	getStr(req, "userID", userID);
	getStr(req, "password", password);
#ifdef _DEBUG
	fprintf(fp, "userID=%s   password=%s \n", userID, password);
	fflush(fp);
#endif
	return login_request(clientSeq, userID, password, NULL, "ch-ZN", "4", NULL);
}

SEQ TdApi::reqUserLogout()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	SEQ logoutSeq =  logout_request(clientSeq);
#ifdef _DEBUG
	fprintf(fp, "logout seq = %ld \n", logoutSeq);
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return logoutSeq;
}


long long TdApi::reqServertime()
{
	return getservertime(clientSeq);
}

SEQ TdApi::reqMarket()
{
	return market_request(clientSeq);
}


hfp::SEQ TdApi::reqContract()
{
	return contract_request(clientSeq);
}

hfp::SEQ TdApi::reqAccount()
{
	return account_request(clientSeq);
}

hfp::SEQ TdApi::reqReceiptcollect()
{
	return receiptcollect_request(clientSeq);
}

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


	virtual void onLoginResponse(dict data)
	{
		try
		{
			this->get_override("onLoginResponse")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onLogoutPush(int outtype)
	{
		try
		{
			this->get_override("onLogoutPush")(outtype);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onMarketStatePush(dict pMarketState)
	{
		try
		{
			this->get_override("onMarketStatePush")(pMarketState);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	
	virtual void onMarketResponse(dict pdict)
	{
		try
		{
			this->get_override("onMarketResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onContractResponse(dict pdict)
	{
		try
		{
			this->get_override("onContractResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onAccountResponse(dict pdict)
	{
		try
		{
			this->get_override("onAccountResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onReceiptcollectResponse(dict pdict)
	{
		try
		{
			this->get_override("onReceiptcollectResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
};


BOOST_PYTHON_MODULE(vnhfptd)
{
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

	class_<TdApiWrap, boost::noncopyable>("TdApi")
		.def("createHFPTdApi", &TdApiWrap::createHFPTdApi)
		.def("connectTradeFront",&TdApiWrap::connectTradeFront)
		.def("reqUserLogin",&TdApiWrap::reqUserLogin)
		.def("reqUserLogout",&TdApiWrap::reqUserLogout)
		.def("reqServertime",&TdApiWrap::reqServertime)
		.def("reqMarket",&TdApiWrap::reqMarket)
		.def("reqContract",&TdApiWrap::reqContract)
		.def("reqAccount",&TdApiWrap::reqAccount)
		.def("reqReceiptcollect",&TdApiWrap::reqReceiptcollect)

		.def("onClientClosed", pure_virtual(&TdApiWrap::onClientClosed))
		.def("onClientConnected", pure_virtual(&TdApiWrap::onClientConnected))
		.def("onClientDisConnected", pure_virtual(&TdApiWrap::onClientDisConnected))
		.def("onClienthandshaked", pure_virtual(&TdApiWrap::onClienthandshaked))
		.def("onLoginResponse", pure_virtual(&TdApiWrap::onLoginResponse))
		.def("onLogoutPush", pure_virtual(&TdApiWrap::onLogoutPush))
		.def("onMarketStatePush", pure_virtual(&TdApiWrap::onMarketStatePush))
		.def("onMarketResponse", pure_virtual(&TdApiWrap::onMarketResponse))
		.def("onContractResponse", pure_virtual(&TdApiWrap::onContractResponse))
		.def("onAccountResponse", pure_virtual(&TdApiWrap::onAccountResponse))
		.def("onReceiptcollectResponse", pure_virtual(&TdApiWrap::onReceiptcollectResponse))

		;
};
