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
#define ONQUOTATIONINFO 5


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


//API的继承实现
class MdApi 
{
private:
	//CThostFtdcMdApi* api;				//API对象
	thread *task_thread;				//工作线程指针（向python中推送数据）
	CLIENT clientSeq;
	CLIENT tradeSeq;

public:
	static ConcurrentQueue<Task> task_queue;	//任务队列

	MdApi()
	{
		function0<void> f = boost::bind(&MdApi::processTask, this);
		thread t(f);
		this->task_thread = &t;
	};

	~MdApi()
	{
	};

	//-------------------------------------------------------------------------------------
	//API回调函数
	//-------------------------------------------------------------------------------------

	static void MdApi::OnClientClosed(CLIENT client, hfp::close_type type)
	{
		Task task = Task();
		task.task_name = ONCLIENTCLOSED;
		task.task_data = type;
		MdApi::task_queue.push(task);
	}

	static void  MdApi::OnClientConnected(CLIENT client)
	{
		Task task = Task();
		task.task_name = ONCLIENTCONNECTED;
		MdApi::task_queue.push(task);
	}

	static void MdApi::OnClientDisConnected(CLIENT client, int code)
	{
		Task task = Task();
		task.task_name = ONCLIENTDISCONNECTED;
		task.task_data = code;
		MdApi::task_queue.push(task);
	}

	static void MdApi::OnClienthandshaked(CLIENT client, bool IsSuccess, int index, const char* code)
	{
		Task task = Task();
		task.task_name = ONCLIENTHANDSHAKED;
		task.task_error = IsSuccess;
		task.task_id = index;
		task.task_data = *code;
		MdApi::task_queue.push(task);
	}

	static void MdApi::OnQuotationInfo(CLIENT client, quotation_data& data)
	{
		Task task = Task();
		task.task_name = ONQUOTATIONINFO;
		task.task_data = data;
		MdApi::task_queue.push(task);
	}

	//-------------------------------------------------------------------------------------
	//task：任务
	//-------------------------------------------------------------------------------------

	//void processTask();


	void processTask()
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


	void processClientClosed(Task task);

	void processClientConnected(Task task);

	void processClientDisConnected(Task task);

	void processClienthandshaked(Task task);

	void processQuotationInfo(Task task);


	//-------------------------------------------------------------------------------------
	//data：回调函数的数据字典
	//error：回调函数的错误字典
	//id：请求id
	//last：是否为最后返回
	//i：整数
	//-------------------------------------------------------------------------------------

	virtual void onClientClosed(int client, int type) {};

	virtual void onClientConnected(int client) {};

	virtual void onClientDisConnected(int client, int code) {};

	virtual void onClienthandshaked(int client, bool IsSuccess, int index, string code) {};

	virtual void onQuotationInfo(int client, dict data) {};

	//-------------------------------------------------------------------------------------
	//req:主动函数的请求字典
	//-------------------------------------------------------------------------------------

	//void createHFPMdApi();
	void createHFPMdApi()
	{
		clientSeq = client("3A0A64012D1084AF793F1BB1FDE2B4CB",
			"71GQ215YTJFWhw3IKaT2GM0Z0HWK6Wb51mP77r1VRH98Ga6kQ+PQ5He8HNkZYrHINorKHq91VJitAiq+VtnC1qSV",
			true,
			hfp::client_type::quotation);//测试

		//设置回调函数
		setkeepalive(clientSeq, true, 5000, 5000);
		setonconnected(clientSeq, MdApi::OnClientConnected);
		setonconnectfail(clientSeq, MdApi::OnClientDisConnected);
		setonclosed(clientSeq, MdApi::OnClientClosed);
		setonhandshaked(clientSeq, MdApi::OnClienthandshaked);//握手
		setonquotation(clientSeq, MdApi::OnQuotationInfo);

	};

	void init();

	void connectMdFront(string mdFrontAddress, int mdPort);

	void connectTradeFront(string tradeFrontAddress, int tradePort);

	/*
	void release();

	

	int join();

	int exit();

	string getTradingDay();

	

	int subscribeMarketData(string instrumentID);

	int unSubscribeMarketData(string instrumentID);

	int subscribeForQuoteRsp(string instrumentID);

	int unSubscribeForQuoteRsp(string instrumentID);

	int reqUserLogin(dict req, int nRequestID);

	int reqUserLogout(dict req, int nRequestID);
	*/
};
