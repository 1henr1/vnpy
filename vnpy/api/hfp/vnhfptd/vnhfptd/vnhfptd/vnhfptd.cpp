// vnhfpmd.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "vnhfptd.h"
#include "HFPTradingClient.h"

using namespace hfp;

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
	pdict["contractid"] = (string)(contractinfo.contractid);//合约编码
	pdict["contractname"] = (string)(contractinfo.contractname);//合约名称
	pdict["initprice"] = contractinfo.initprice;//初始价
	pdict["beginday"] = (string)(contractinfo.beginday);//开始交易日(yymmddhh24mi)
	pdict["endday"] = (string)(contractinfo.endday);//结束交易日(yymmddhh24mi)
	pdict["uplimit"] = contractinfo.uplimit;//最高限价
	pdict["downlimit"] = contractinfo.downlimit;//最低限价
	pdict["maxqty"] = contractinfo.maxqty;//单笔最大申报量
	pdict["minqty"] = contractinfo.minqty;//单笔最小申报量
	pdict["mindiffprice"] = contractinfo.mindiffprice;//最小变动价
	pdict["mindeliveryqty"] = contractinfo.mindeliveryqty;//最小交收量
	pdict["taxrate"] = contractinfo.taxrate;//税率
	pdict["positionlimit"] = contractinfo.positionlimit;//全市场限仓
	pdict["fuseuplimit"] = contractinfo.fuseuplimit;//熔断上限
	pdict["fusedownlimit"] = contractinfo.fusedownlimit;//熔断下限
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
	pdict["memberid"] = (string)item.memberid;//会员编码
	pdict["curname"] = (string)item.curname;//币种名称
	pdict["avlb"] = item.avlb;//可用资金
	pdict["occp"] = item.occp;//保证金(实收)
	pdict["frzord"] = item.frzord;//报单冻结
	pdict["frzrisk"] = item.frzrisk;//风控冻结
	pdict["ins"] = item.ins;//入金
	pdict["outs"] = item.outs;//出金
	pdict["poundage"] = item.poundage;//交易手续费
	pdict["cnybalance"] = item.cnybalance;//转让盈亏
	pdict["managefee"] = item.managefee;//会费
	pdict["adjust"] = item.adjust;//调整资金
	pdict["financing"] = item.financing;//融资资金
	pdict["balance"] = item.balance;//账面价差
	pdict["funddynamic"] = item.funddynamic;//动态权益
	pdict["fundstatic"] = item.fundstatic;//静态态权益
	pdict["frzpayment"] = item.frzpayment;//冻结货款
	pdict["fundonway"] = item.fundonway;//待出资金
	pdict["occpshould"] = item.occpshould;//保证金(应收)
	pdict["avlbfundout"] = item.avlbfundout;//可出资金
	this->onAccountResponse(pdict);
}

void TdApi::processReceiptcollectResponse(Task task)
{
	PyLock lock;
	receipt_collect item = any_cast<receipt_collect>(task.task_data);
	dict pdict;
	pdict["memberid"] = (string)item.memberid;//会员编码
	pdict["goodsid"] = (string)item.goodsid;//商品编码
	pdict["goodsname"] = (string)item.goodsname;//商品名称
	pdict["totalqty"] = item.totalqty;//总量
	pdict["avlbqty"] = item.avlbqty;//可用量
	pdict["frzord"] = item.frzord;//报单冻结量
	pdict["frzrisk"] = item.frzrisk;//风控冻结量
	pdict["occp"] = item.occp;//持仓占用量
	this->onReceiptcollectResponse(pdict);
}

///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------
void TdApi::createHFPTdApi(string id, string license)
{
	//创建客户端
	clientSeq = client(id.c_str(), license.c_str(), true, hfp::client_type::trading);//测试

	//设置回调函数
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
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct TdApiWrap : TdApi, wrapper < TdApi >
{
	virtual void onClientClosed(int type)
	{
		//以下的try...catch...可以实现捕捉python环境中错误的功能，防止C++直接出现原因未知的崩溃
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
	PyEval_InitThreads();	//导入时运行，保证先创建GIL

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
