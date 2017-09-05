// vnhfpmd.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "vnhfpmd.h"
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


void  MdApi::OnClientClosed(CLIENT client, hfp::close_type type)
{
	Task task = Task();
	task.task_name = ONCLIENTCLOSED;
	task.task_data = type;
	this->task_queue.push(task);
}

void  MdApi::OnClientConnected(CLIENT client)
{
	Task task = Task();
	task.task_name = ONCLIENTCONNECTED;
	this->task_queue.push(task);
}

void  MdApi::OnClientDisConnected(CLIENT client, int code)
{
	Task task = Task();
	task.task_name = ONCLIENTDISCONNECTED;
	task.task_data = code;
	this->task_queue.push(task);
}

void  MdApi::OnClienthandshaked(CLIENT client, bool IsSuccess, int index, const char* code)
{
	Task task = Task();
	task.task_name = ONCLIENTHANDSHAKED;
	task.task_error = IsSuccess;
	task.task_id = index;
	task.task_data = *code;
	this->task_queue.push(task);
}

void  MdApi::OnQuotationInfo(CLIENT client, quotation_data& data)
{
	Task task = Task();
	task.task_name = ONQUOTATIONINFO;
	task.task_data = data;
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
		case ONCLIENTCLOSED:
		{
			this->processClientClosed(task);
			break;
		}
		case ONCLIENTCONNECTED:
		{
			this->processClientConnected(task);
			break;
		}
		case ONCLIENTDISCONNECTED:
		{
			this->processClientDisConnected(task);
			break;
		}
		case ONCLIENTHANDSHAKED:
		{
			this->processClienthandshaked(task);
			break;
		}
		case ONQUOTATIONINFO:
		{
			this->processQuotationInfo(task);
			break;
		}
		};
	}
};

void MdApi::processClientClosed(Task task)
{
	PyLock lock;
	int type = any_cast<int>(task.task_data);
	this->onClientClosed(0, type);
}

void MdApi::processClientConnected(Task task)
{
	PyLock lock;
	this->onClientConnected(0);
}

void MdApi::processClientDisConnected(Task task)
{
	PyLock lock;
	int code = any_cast<int>(task.task_data);
	this->onClientDisConnected(0, code);
}

void MdApi::processClienthandshaked(Task task)
{
	PyLock lock;
	bool IsSuccess = any_cast<bool>(task.task_error);
	int index = task.task_id;
	string code = any_cast<string>(task.task_data);
	this->onClienthandshaked(0, IsSuccess, index, code);
}

void MdApi::processQuotationInfo(Task task)
{
	PyLock lock;
	quotation_data quota_data = any_cast<quotation_data>(task.task_data);
	dict marketdata;
	marketdata["time"] = quota_data.datetime;
	marketdata["open"] = quota_data.open;

	this->onQuotationInfo(0, marketdata);
}


///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------

void MdApi::createHFPMdApi()
{
	clientSeq = client("3A0A64012D1084AF793F1BB1FDE2B4CB",
		"71GQ215YTJFWhw3IKaT2GM0Z0HWK6Wb51mP77r1VRH98Ga6kQ+PQ5He8HNkZYrHINorKHq91VJitAiq+VtnC1qSV",
		true,
		hfp::client_type::quotation);//测试

	//设置回调函数
	setkeepalive(clientSeq, true, 5000, 5000);
	setonconnected(clientSeq, this->OnClientConnected));
	setonconnectfail(clientSeq, this->OnClientDisConnected);
	setonclosed(clientSeq, this->OnClientClosed);
	setonhandshaked(clientSeq, this->OnClienthandshaked);//握手
	setonquotation(clientSeq, this->OnQuotationInfo);
	
};

/*

void MdApi::registerFront(string pszFrontAddress)
{
	this->api->RegisterFront((char*)pszFrontAddress.c_str());
};

int MdApi::subscribeMarketData(string instrumentID)
{
	char* buffer = (char*)instrumentID.c_str();
	char* myreq[1] = { buffer };
	int i = this->api->SubscribeMarketData(myreq, 1);
	return i;
};

int MdApi::unSubscribeMarketData(string instrumentID)
{
	char* buffer = (char*)instrumentID.c_str();
	char* myreq[1] = { buffer };;
	int i = this->api->UnSubscribeMarketData(myreq, 1);
	return i;
};

int MdApi::subscribeForQuoteRsp(string instrumentID)
{
	char* buffer = (char*)instrumentID.c_str();
	char* myreq[1] = { buffer };
	int i = this->api->SubscribeForQuoteRsp(myreq, 1);
	return i;
};

int MdApi::unSubscribeForQuoteRsp(string instrumentID)
{
	char* buffer = (char*)instrumentID.c_str();
	char* myreq[1] = { buffer };;
	int i = this->api->UnSubscribeForQuoteRsp(myreq, 1);
	return i;
};

int MdApi::reqUserLogin(dict req, int nRequestID)
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

int MdApi::reqUserLogout(dict req, int nRequestID)
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
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct MdApiWrap : MdApi, wrapper < MdApi >
{
	virtual void onClientClosed(int client, int type)
	{
		//以下的try...catch...可以实现捕捉python环境中错误的功能，防止C++直接出现原因未知的崩溃
		try
		{
			this->get_override("onClientClosed")(client, type);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onClientConnected(int client)
	{
		try
		{
			this->get_override("onClientConnected")(client);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onClientDisConnected(int client, int code)
	{
		try
		{
			this->get_override("onClientDisConnected")(client, code);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onClienthandshaked(int client, bool IsSuccess, int index, string code)
	{
		try
		{
			this->get_override("onClienthandshaked")(client, IsSuccess, index, code);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onQuotationInfo(int client, dict data)
	{
		try
		{
			this->get_override("onQuotationInfo")(client, data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	
};


BOOST_PYTHON_MODULE(vnhfpmd)
{
	PyEval_InitThreads();	//导入时运行，保证先创建GIL

	class_<MdApiWrap, boost::noncopyable>("MdApi")
		.def("createHFPMdApi", &MdApiWrap::createHFPMdApi)
		.def("onClientClosed", pure_virtual(&MdApiWrap::onClientClosed))
		.def("onClientConnected", pure_virtual(&MdApiWrap::onClientConnected))
		.def("onClientDisConnected", pure_virtual(&MdApiWrap::onClientDisConnected))
		.def("onClienthandshaked", pure_virtual(&MdApiWrap::onClienthandshaked))
		.def("onQuotationInfo", pure_virtual(&MdApiWrap::onQuotationInfo))
		;
};
