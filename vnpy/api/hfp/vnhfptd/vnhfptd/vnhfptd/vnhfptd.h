//说明部分

//系统
#ifdef WIN32
#include "stdafx.h"
#endif
#include <string>
#include <queue>

//Boost
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python/module.hpp>	//python封装
#include <boost/python/def.hpp>		//python封装
#include <boost/python/dict.hpp>	//python封装
#include <boost/python/object.hpp>	//python封装
#include <boost/python.hpp>			//python封装
#include <boost/thread.hpp>			//任务队列的线程功能
#include <boost/bind.hpp>			//任务队列的线程功能
#include <boost/any.hpp>			//任务队列的任务实现

//API
#include "HFPTradingClient.h"

//命名空间
using namespace std;
using namespace boost::python;
using namespace boost;
using namespace hfp;

//常量
#define ONCLIENTCLOSED 1
#define ONCLIENTCONNECTED 2
#define ONCLIENTDISCONNECTED 3
#define ONCLIENTHANDSHAKED 4
#define ONLOGINRESPONSE 5
#define ONLOGOUTPUSH 6
#define ONMARKETSTATEPUSH 7
#define ONASSOCIATORRESPONSE 8
#define ONMARKETRESPONSE 9
#define ONCONTRACTRESPONSE 10
#define ONACCOUNTRESPONSE 11
#define ONRECEIPTCOLLECTRESPONSE 12
#define ONORDERRESPONSE 13
#define ONQUERYORDERRESPONSE 14
#define ONCANCELORDERPUSH 15
#define ONCANCELORDERRESPONSE 16
#define ONDEALPUSH 17
#define ONQUERYDEALRESPONSE 18
#define ONQUERYPOSITIONCOLLECTRESPONSE 19
#define ONQUERYPOSITIONDETAILRESPONSE 20
#define ONQUERYDEPOSITINFORESPONSE 21
#define ONQUERYFEEINFORESPONSE 22


///-------------------------------------------------------------------------------------
///API中的部分组件
///-------------------------------------------------------------------------------------

//GIL全局锁简化获取用，
//用于帮助C++线程获得GIL锁，从而防止python崩溃
class PyLock
{
private:
	PyGILState_STATE gil_state;

public:
	//在某个函数方法中创建该对象时，获得GIL锁
	PyLock()
	{
		gil_state = PyGILState_Ensure();
	}

	//在某个函数完成后销毁该对象时，解放GIL锁
	~PyLock()
	{
		PyGILState_Release(gil_state);
	}
};


//任务结构体
struct Task
{
	int task_name;		//回调函数名称对应的常量
	any task_data;		//数据结构体
	any task_error;		//错误结构体
	int task_id;		//请求id
	bool task_last;		//是否为最后返回
};


///线程安全的队列
template<typename Data>

class ConcurrentQueue
{
private:
	queue<Data> the_queue;								//标准库队列
	mutable mutex the_mutex;							//boost互斥锁
	condition_variable the_condition_variable;			//boost条件变量

public:

	//存入新的任务
	void push(Data const& data)
	{
		mutex::scoped_lock lock(the_mutex);				//获取互斥锁
		the_queue.push(data);							//向队列中存入数据
		lock.unlock();									//释放锁
		the_condition_variable.notify_one();			//通知正在阻塞等待的线程
	}

	//检查队列是否为空
	bool empty() const
	{
		mutex::scoped_lock lock(the_mutex);
		return the_queue.empty();
	}

	//取出
	Data wait_and_pop()
	{
		mutex::scoped_lock lock(the_mutex);

		while (the_queue.empty())						//当队列为空时
		{
			the_condition_variable.wait(lock);			//等待条件变量通知
		}

		Data popped_value = the_queue.front();			//获取队列中的最后一个任务
		the_queue.pop();								//删除该任务
		return popped_value;							//返回该任务
	}

};


//从字典中获取某个建值对应的整数，并赋值到请求结构体对象的值上
void getInt(dict d, string key, int* value);


//从字典中获取某个建值对应的浮点数，并赋值到请求结构体对象的值上
void getDouble(dict d, string key, double* value);


//从字典中获取某个建值对应的字符，并赋值到请求结构体对象的值上
void getChar(dict d, string key, char* value);


//从字典中获取某个建值对应的字符串，并赋值到请求结构体对象的值上
void getStr(dict d, string key, char* value);


///-------------------------------------------------------------------------------------
///C++ SPI的回调函数方法实现
///-------------------------------------------------------------------------------------

ConcurrentQueue<Task> task_queue;	//任务队列

#ifdef _DEBUG
FILE *fp = fopen("debug.txt", "w");
#endif

//API的继承实现
class TdApi
{
private:
	thread *task_thread;				//工作线程指针（向python中推送数据）
	CLIENT clientSeq;

public:

	TdApi()
	{
		function0<void> f = boost::bind(&TdApi::processTask, this);
		thread t(f);
		this->task_thread = &t;
	};

	~TdApi()
	{
	};

	//-------------------------------------------------------------------------------------
	//API回调函数
	//-------------------------------------------------------------------------------------

	static void OnClientClosed(CLIENT client, hfp::close_type type)
	{
		Task task = Task();
		task.task_name = ONCLIENTCLOSED;
		task.task_data = type;
		task_queue.push(task);
	}

	static void OnClientConnected(CLIENT client)
	{
		Task task = Task();
		task.task_name = ONCLIENTCONNECTED;
		task_queue.push(task);
	}

	static void OnClientDisConnected(CLIENT client, int code)
	{
		Task task = Task();
		task.task_name = ONCLIENTDISCONNECTED;
		task.task_data = code;
		task_queue.push(task);
	}

	static void OnClienthandshaked(CLIENT client, bool IsSuccess, int index, const char* code)
	{
		Task task = Task();
		task.task_name = ONCLIENTHANDSHAKED;
		task.task_error = IsSuccess;
		task.task_id = index;
		task.task_data = (code == nullptr) ? "" : (string)code;
		task_queue.push(task);
	}

	static void OnLoginResponse(CLIENT client, response& rsp, const char* code)
	{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
		Task task = Task();
		task.task_name = ONLOGINRESPONSE;
		task.task_data = rsp;
		task_queue.push(task);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	}

	static void OnLogoutPush(CLIENT client, outtype& type)
	{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
		Task task = Task();
		task.task_name = ONLOGOUTPUSH;
		task.task_data = type;
		task_queue.push(task);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	}

	static void OnMarketStatePush(CLIENT client, const char* id, const char* instumentcode, market_state& marketstate)
	{
		Task task = Task();
		task.task_name = ONMARKETSTATEPUSH;
		task.task_data = instumentcode == nullptr ? ( id == nullptr ? "" : (string)id) : (string)instumentcode;
		task.task_id = marketstate;
		task_queue.push(task);
	}

	static void OnAssociatorResponse(CLIENT client, response& rsp, associator& asso)
	{
		Task task = Task();
		task.task_name = ONASSOCIATORRESPONSE;
		task_queue.push(task);

	}

	static void OnMarketResponse(CLIENT client, response& rsp, const market* marketinfo, unsigned int number)
	{
		Task task = Task();
		task.task_name = ONMARKETRESPONSE;
		if (marketinfo != nullptr)
		{
			task.task_data = *marketinfo;
			task_queue.push(task);
		}
	}

	static void OnContractResponse(CLIENT client, response& rsp, const contract* contactinfo, unsigned int num)
	{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
		unsigned int i = 0;
		while ( i < num )
		{
			Task task = Task();
			task.task_name = ONCONTRACTRESPONSE;
			task.task_error = rsp;
			task.task_data = contactinfo[i++];
			task_queue.push(task);
		}
	}

	static void OnAccountResponse(CLIENT client, response& rsp, const account* accountinfo, unsigned int num)
	{
		unsigned int i = 0;
		while (i < num)
		{
			Task task = Task();
			task.task_name = ONACCOUNTRESPONSE;
			task.task_error = rsp;
			task.task_data = accountinfo[i++];
			task_queue.push(task);
		}
	}

	static void OnReceiptcollectResponse(CLIENT client, response& rsp, const receipt_collect* receiptcollect, unsigned int num)
	{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fprintf(fp, "num = %d \n", num);
	fflush(fp);
#endif
		unsigned int i = 0;
		while (i < num)
		{
			Task task = Task();
			task.task_name = ONRECEIPTCOLLECTRESPONSE;
			task.task_error = rsp;
			task.task_data = receiptcollect[i++];
			task_queue.push(task);
		}
	}

	static void OnOrderResponse(CLIENT client, response& rsp, order_res& orderresponse)
	{
		Task task = Task();
		task.task_name = ONORDERRESPONSE;
		task.task_error = rsp;
		task.task_data = orderresponse;
		task_queue.push(task);
	}

	static void OnQueryorderResponse(CLIENT client, response& rsp, order* queryorder, unsigned int num)
	{
		unsigned int i = 0;
		while (i < num)
		{
			Task task = Task();
			task.task_name = ONQUERYORDERRESPONSE;
			task.task_error = rsp;
			task.task_data = queryorder[i++];
			task_queue.push(task);
		}
	}

	static void OnCancelorderResponse(CLIENT client, response& rsp, cancel_order_res& cancelorderres)
	{
		Task task = Task();
		task.task_name = ONCANCELORDERRESPONSE;
		task.task_error = rsp;
		task.task_data = cancelorderres;
		task_queue.push(task);
	}

	static void OnCancelorderPush(CLIENT client, cancel_order_push& cancelpush)
	{
		Task task = Task();
		task.task_name = ONCANCELORDERPUSH;
		task.task_data = cancelpush;
		task_queue.push(task);
	}

	static void OnDealPush(CLIENT client, deal& dealinfo)
	{
		Task task = Task();
		task.task_name = ONDEALPUSH;
		task.task_data = dealinfo;
		task_queue.push(task);
	}

	static void OnQuerydealResponse(CLIENT client, response& rsp, deal* dealinfo, unsigned int num)
	{
		unsigned int i = 0;
		while (i < num)
		{
			Task task = Task();
			task.task_name = ONQUERYDEALRESPONSE;
			task.task_error = rsp;
			task.task_data = dealinfo[i++];
			task_queue.push(task);
		}
	}

	static void OnQueryPositioncollectResponse(CLIENT client, response& rsp, position_collect* poscollect, unsigned int num)
	{
		unsigned int i = 0;
		while (i < num)
		{
			Task task = Task();
			task.task_name = ONQUERYPOSITIONCOLLECTRESPONSE;
			task.task_error = rsp;
			task.task_data = poscollect[i++];
			task_queue.push(task);
		}
	}

	static void OnQueryPositiondetailResponse(CLIENT client, response& rsp, position_detail* posdetail, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONQUERYPOSITIONDETAILRESPONSE;
		task_queue.push(task);

	}

	static void OnQueryDepositinfoResponse(CLIENT client, response& rsp, deposit_info* depositinfo, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONQUERYDEPOSITINFORESPONSE;
		task_queue.push(task);
	}

	static void OnQueryfeeinfoResponse(CLIENT client, response& rsp, fee_info * feeinfo, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONQUERYFEEINFORESPONSE;
		task_queue.push(task);
	}

	//-------------------------------------------------------------------------------------
	//task：任务
	//-------------------------------------------------------------------------------------
	void processTask()
	{
		while (1)
		{
			Task task = task_queue.wait_and_pop();

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
				case ONLOGINRESPONSE:
				{
					this->processLoginResponse(task);
					break;
				}
				case ONLOGOUTPUSH:
				{
					this->processLogoutPush(task);
					break;
				}
				case ONMARKETSTATEPUSH:
				{
					this->processMarketStatePush(task);
					break;
				}
				//case ONASSOCIATORRESPONSE:
				//{
				//	this->processAssociatorResponse(task);
				//	break;
				//}
				case ONMARKETRESPONSE:
				{
					this->processMarketResponse(task);
					break;
				}
				case ONCONTRACTRESPONSE:
				{
					this->processContractResponse(task);
					break;
				}
				case ONACCOUNTRESPONSE:
				{
					this->processAccountResponse(task);
					break;
				}
				case ONRECEIPTCOLLECTRESPONSE:
				{
					this->processReceiptcollectResponse(task);
					break;
				}
				case ONORDERRESPONSE:
				{
					this->processOrderResponse(task);
					break;
				}
				case ONQUERYORDERRESPONSE:
				{
					this->processQueryorderResponse(task);
					break;
				}
				case ONCANCELORDERPUSH:
				{
					this->processCancelorderPush(task);
					break;
				}
				case ONCANCELORDERRESPONSE:
				{
					this->processCancelorderResponse(task);
					break;
				}
				case ONDEALPUSH:
				{
					this->processDealPush(task);
					break;
				}
				case ONQUERYDEALRESPONSE:
				{
					this->processQuerydealResponse(task);
					break;
				}
				case ONQUERYPOSITIONCOLLECTRESPONSE:
				{
					this->processQueryPositioncollectResponse(task);
					break;
				}
				/*
				case ONQUERYPOSITIONDETAILRESPONSE:
				{
					this->processQueryPositiondetailResponse(task);
					break;
				}
				case ONQUERYDEPOSITINFORESPONSE:
				{
					this->processQueryDepositinfoResponse(task);
					break;
				}
				case ONQUERYFEEINFORESPONSE:
				{
					this->processQueryfeeinfoResponse(task);
					break;
				}
				*/
			};
		}
	};


	void processClientClosed(Task task);

	void processClientConnected(Task task);

	void processClientDisConnected(Task task);

	void processClienthandshaked(Task task);

	void processLoginResponse(Task task);

	void processLogoutPush(Task task);

	void processMarketStatePush(Task task);

	void processAssociatorResponse(Task task);

	void processMarketResponse(Task task);

	void processContractResponse(Task task);

	void processAccountResponse(Task task);

	void processReceiptcollectResponse(Task task);

	void processOrderResponse(Task task);

	void processQueryorderResponse(Task task);

	void processCancelorderPush(Task task);

	void processCancelorderResponse(Task task);

	void processDealPush(Task task);

	void processQuerydealResponse(Task task);

	void processQueryPositioncollectResponse(Task task);

	void processQueryPositiondetailResponse(Task task);

	void processQueryDepositinfoResponse(Task task);

	void processQueryfeeinfoResponse(Task task);

	//-------------------------------------------------------------------------------------
	//与python的对接,以下函数会被python端重载
	//-------------------------------------------------------------------------------------

	virtual void onClientClosed(int type) {};

	virtual void onClientConnected() {};

	virtual void onClientDisConnected(int code) {};

	virtual void onClienthandshaked(bool IsSuccess, int index, string code) {};

	virtual void onLoginResponse(dict rsp) {};

	virtual void onLogoutPush(int outtype) {};

	virtual void onMarketStatePush(dict pMarketState) {};

	virtual void onAssociatorResponse(response& rsp, associator& asso) {};

	virtual void onMarketResponse(dict pdict) {};

	virtual void onContractResponse(dict pdict) {};

	virtual void onAccountResponse(dict pdict) {};

	virtual void onReceiptcollectResponse(dict pdict) {};

	virtual void onOrderResponse(dict rsp, dict order) {};

	virtual void onQueryorderResponse(dict rsp, dict order) {};

	virtual void onCancelorderPush(dict pdict) {};

	virtual void onCancelorderResponse(dict rsp, dict pdict) {};

	virtual void onDealPush(dict pdict) {};

	virtual void onQuerydealResponse(dict rsp, dict pdict) {};

	virtual void onQueryPositioncollectResponse(dict rsp, dict pdict) {};

	virtual void onQueryPositiondetailResponse(response& rsp, position_detail* posdetail, unsigned int len) {};

	virtual void onQueryDepositinfoResponse(response& rsp, deposit_info* depositinfo, unsigned int num) {};

	virtual void onQueryfeeinfoResponse(response& rsp, fee_info * feeinfo, unsigned int num) {};


	//-------------------------------------------------------------------------------------
	//req:主动函数的请求字典
	//-------------------------------------------------------------------------------------

	void createHFPTdApi(string id, string license);

	void connectTradeFront(string tradeFrontAddress, int tradePort);

	SEQ reqUserLogin(dict req);

	SEQ reqUserLogout();

	long long reqServertime();

	//SEQ associator_request(CLIENT);

	SEQ reqMarket();

	SEQ reqContract();

	SEQ reqAccount();

	SEQ reqReceiptcollect();

	SEQ reqOrder(string, string, string, bool, int, int, bool, int, int);

	SEQ qryOrder(string);

	SEQ reqCancelorder(string, string);

	SEQ qryDeal(string);

	SEQ qryPositioncollect(string);
};
