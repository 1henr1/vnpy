//˵������

//ϵͳ
#ifdef WIN32
#include "stdafx.h"
#endif
#include <string>
#include <queue>

//Boost
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python/module.hpp>	//python��װ
#include <boost/python/def.hpp>		//python��װ
#include <boost/python/dict.hpp>	//python��װ
#include <boost/python/object.hpp>	//python��װ
#include <boost/python.hpp>			//python��װ
#include <boost/thread.hpp>			//������е��̹߳���
#include <boost/bind.hpp>			//������е��̹߳���
#include <boost/any.hpp>			//������е�����ʵ��

//API
#include "HFPTradingClient.h"

//�����ռ�
using namespace std;
using namespace boost::python;
using namespace boost;
using namespace hfp;

//����
#define ONCLIENTCLOSED 1
#define ONCLIENTCONNECTED 2
#define ONCLIENTDISCONNECTED 3
#define ONCLIENTHANDSHAKED 4
#define ONQUOTATIONINFO 5


///-------------------------------------------------------------------------------------
///API�еĲ������
///-------------------------------------------------------------------------------------

//GILȫ�����򻯻�ȡ�ã�
//���ڰ���C++�̻߳��GIL�����Ӷ���ֹpython����
class PyLock
{
private:
	PyGILState_STATE gil_state;

public:
	//��ĳ�����������д����ö���ʱ�����GIL��
	PyLock()
	{
		gil_state = PyGILState_Ensure();
	}

	//��ĳ��������ɺ����ٸö���ʱ�����GIL��
	~PyLock()
	{
		PyGILState_Release(gil_state);
	}
};


//����ṹ��
struct Task
{
	int task_name;		//�ص��������ƶ�Ӧ�ĳ���
	any task_data;		//���ݽṹ��
	any task_error;		//����ṹ��
	int task_id;		//����id
	bool task_last;		//�Ƿ�Ϊ��󷵻�
};


///�̰߳�ȫ�Ķ���
template<typename Data>

class ConcurrentQueue
{
private:
	queue<Data> the_queue;								//��׼�����
	mutable mutex the_mutex;							//boost������
	condition_variable the_condition_variable;			//boost��������

public:

	//�����µ�����
	void push(Data const& data)
	{
		mutex::scoped_lock lock(the_mutex);				//��ȡ������
		the_queue.push(data);							//������д�������
		lock.unlock();									//�ͷ���
		the_condition_variable.notify_one();			//֪ͨ���������ȴ����߳�
	}

	//�������Ƿ�Ϊ��
	bool empty() const
	{
		mutex::scoped_lock lock(the_mutex);
		return the_queue.empty();
	}

	//ȡ��
	Data wait_and_pop()
	{
		mutex::scoped_lock lock(the_mutex);

		while (the_queue.empty())						//������Ϊ��ʱ
		{
			the_condition_variable.wait(lock);			//�ȴ���������֪ͨ
		}

		Data popped_value = the_queue.front();			//��ȡ�����е����һ������
		the_queue.pop();								//ɾ��������
		return popped_value;							//���ظ�����
	}

};


//���ֵ��л�ȡĳ����ֵ��Ӧ������������ֵ������ṹ������ֵ��
void getInt(dict d, string key, int* value);


//���ֵ��л�ȡĳ����ֵ��Ӧ�ĸ�����������ֵ������ṹ������ֵ��
void getDouble(dict d, string key, double* value);


//���ֵ��л�ȡĳ����ֵ��Ӧ���ַ�������ֵ������ṹ������ֵ��
void getChar(dict d, string key, char* value);


//���ֵ��л�ȡĳ����ֵ��Ӧ���ַ���������ֵ������ṹ������ֵ��
void getStr(dict d, string key, char* value);


///-------------------------------------------------------------------------------------
///C++ SPI�Ļص���������ʵ��
///-------------------------------------------------------------------------------------


//API�ļ̳�ʵ��
class MdApi 
{
private:
	//CThostFtdcMdApi* api;				//API����
	thread *task_thread;				//�����߳�ָ�루��python���������ݣ�
	CLIENT clientSeq;
	CLIENT tradeSeq;

public:
	static ConcurrentQueue<Task> task_queue;	//�������

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
	//API�ص�����
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
	//task������
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
	//data���ص������������ֵ�
	//error���ص������Ĵ����ֵ�
	//id������id
	//last���Ƿ�Ϊ��󷵻�
	//i������
	//-------------------------------------------------------------------------------------

	virtual void onClientClosed(int client, int type) {};

	virtual void onClientConnected(int client) {};

	virtual void onClientDisConnected(int client, int code) {};

	virtual void onClienthandshaked(int client, bool IsSuccess, int index, string code) {};

	virtual void onQuotationInfo(int client, dict data) {};

	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	//void createHFPMdApi();
	void createHFPMdApi()
	{
		clientSeq = client("3A0A64012D1084AF793F1BB1FDE2B4CB",
			"71GQ215YTJFWhw3IKaT2GM0Z0HWK6Wb51mP77r1VRH98Ga6kQ+PQ5He8HNkZYrHINorKHq91VJitAiq+VtnC1qSV",
			true,
			hfp::client_type::quotation);//����

		//���ûص�����
		setkeepalive(clientSeq, true, 5000, 5000);
		setonconnected(clientSeq, MdApi::OnClientConnected);
		setonconnectfail(clientSeq, MdApi::OnClientDisConnected);
		setonclosed(clientSeq, MdApi::OnClientClosed);
		setonhandshaked(clientSeq, MdApi::OnClienthandshaked);//����
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
