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
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return getservertime(clientSeq);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
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

		.def("onClientClosed", pure_virtual(&TdApiWrap::onClientClosed))
		.def("onClientConnected", pure_virtual(&TdApiWrap::onClientConnected))
		.def("onClientDisConnected", pure_virtual(&TdApiWrap::onClientDisConnected))
		.def("onClienthandshaked", pure_virtual(&TdApiWrap::onClienthandshaked))
		.def("onLoginResponse", pure_virtual(&TdApiWrap::onLoginResponse))
		.def("onLogoutPush", pure_virtual(&TdApiWrap::onLogoutPush))

		;
};
