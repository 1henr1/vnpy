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





///-------------------------------------------------------------------------------------
///工作线程从队列中取出数据，转化为python对象后，进行推送
///-------------------------------------------------------------------------------------

/*
void MdApi::processTask()
{
	while (1)
	{
		Task task = MdApi::task_queue.wait_and_pop();

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
*/

void MdApi::processClientClosed(Task task)
{
	PyLock lock;
	int type = any_cast<int>(task.task_data);
	this->onClientClosed(type);
}

void MdApi::processClientConnected(Task task)
{
	PyLock lock;
	this->onClientConnected();
}

void MdApi::processClientDisConnected(Task task)
{
	PyLock lock;
	int code = any_cast<int>(task.task_data);
	this->onClientDisConnected(code);
}

void MdApi::processClienthandshaked(Task task)
{
	PyLock lock;
	bool IsSuccess = any_cast<bool>(task.task_error);
	int index = task.task_id;
	string code = any_cast<string>(task.task_data);
	this->onClienthandshaked(IsSuccess, index, code);
}

void MdApi::processQuotationInfo(Task task)
{
	PyLock lock;
	quotation_data quotaData = any_cast<quotation_data>(task.task_data);
	dict marketdata;

	marketdata["seq"]  = quotaData.seq;//流水号
	marketdata["datetime"] = quotaData.datetime;//发生时间(秒)
	marketdata["contract_id"] = (string)quotaData.contract_id;//合约编码
	marketdata["contract_name"] = (string)quotaData.contract_name;//合约名称
	marketdata["marketId"] = (string)quotaData.marketId;//市场编码
	marketdata["open"] = quotaData.open;//开盘价
	marketdata["high"] = quotaData.high;//最高价
	marketdata["low"] = quotaData.low;//最低价
	marketdata["new"] = quotaData._new;//最新价
	marketdata["last_close"] = quotaData.last_close;//昨日收盘价
	marketdata["average"] = quotaData.average;//均价
	marketdata["total_volume"] = quotaData.total_volume;//总成交量
	marketdata["total_amount"] = quotaData.total_amount;//总成交额
	marketdata["subs_volume"] = quotaData.subs_volume;//总持仓量
	marketdata["cur_volume"] = quotaData.cur_volume;//现量
	marketdata["type"] = quotaData.type;//行情类型 1-集合竞价申报,2-集合竞价成交,3-申报,4-成交
	marketdata["down_limit"] = quotaData.down_limit;//最低限价
	marketdata["up_limit"] = quotaData.up_limit;//最高限价
	marketdata["balance_price"] = quotaData.balance_price;//盈亏计算价
	marketdata["updown_base"] = quotaData.updown_base;//涨跌幅基准价
	marketdata["last_subs_volume"] = quotaData.last_subs_volume;//昨日持仓
	marketdata["buy_price_1"] = quotaData.buy_price[0];//买价
	marketdata["buy_volume_1"] = quotaData.buy_volume[0];//买量
	marketdata["sell_price_1"] = quotaData.sell_price[0];//卖价
	marketdata["sell_volume_1"] = quotaData.sell_volume[0];//卖量

	this->onQuotationInfo(marketdata);
}


///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------
void MdApi::createHFPMdApi(string id, string license)
{
	//创建客户端
	clientSeq = client(id.c_str(), license.c_str(), true, hfp::client_type::quotation);//测试

	//设置回调函数
	setkeepalive(clientSeq, true, 5000, 5000);
	setonconnected(clientSeq, MdApi::OnClientConnected);
	setonconnectfail(clientSeq, MdApi::OnClientDisConnected);
	setonclosed(clientSeq, MdApi::OnClientClosed);
	setonhandshaked(clientSeq, MdApi::OnClienthandshaked);//握手
	setonquotation(clientSeq, MdApi::OnQuotationInfo);

};

void MdApi::init()
{
	//连接
	connect(clientSeq, "58.215.39.218", 5566);//测试
}


void MdApi::connectMdFront(string mdFrontAddress, int mdPort)
{
	connect(clientSeq, mdFrontAddress.c_str(), mdPort);
}

void MdApi::connectTradeFront(string tradeFrontAddress, int tradePort)
{
	connect(clientSeq, (char*)tradeFrontAddress.c_str(), tradePort);
}

/*
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
	PyEval_InitThreads();	//导入时运行，保证先创建GIL

	class_<MdApiWrap, boost::noncopyable>("MdApi")
		.def("createHFPMdApi", &MdApiWrap::createHFPMdApi)
		.def("init", &MdApiWrap::init)
		.def("connectMdFront",&MdApiWrap::connectMdFront)
		.def("onClientClosed", pure_virtual(&MdApiWrap::onClientClosed))
		.def("onClientConnected", pure_virtual(&MdApiWrap::onClientConnected))
		.def("onClientDisConnected", pure_virtual(&MdApiWrap::onClientDisConnected))
		.def("onClienthandshaked", pure_virtual(&MdApiWrap::onClienthandshaked))
		.def("onQuotationInfo", pure_virtual(&MdApiWrap::onQuotationInfo))
		;
};
