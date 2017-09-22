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
#define ONLOGINRESPONSE 5
#define ONLOGOUTPUSH 6
#define ONMARKETSTATEPUSH 7
#define ONMARKETSTATEPUSH 8
#define ONASSOCIATORRESPONSE 9
#define ONMARKETRESPONSE 10
#define ONCONTRACTRESPONSE 11
#define ONACCOUNTRESPONSE 12
#define ONRECEIPTCOLLECTRESPONSE 13
#define ONORDERRESPONSE 14
#define ONQUERYORDERRESPONSE 15
#define ONCANCELORDERPUSH 16
#define ONDEALPUSH 17
#define ONQUERYDEALRESPONSE 18
#define ONQUERYPOSITIONCOLLECTRESPONSE 19
#define ONQUERYPOSITIONDETAILRESPONSE 20
#define ONQUERYDEPOSITINFORESPONSE 21
#define ONQUERYFEEINFORESPONSE 22


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

ConcurrentQueue<Task> task_queue;	//�������
FILE *fp = fopen("debug.txt", "w");

//API�ļ̳�ʵ��
class TdApi
{
private:
	thread *task_thread;				//�����߳�ָ�루��python���������ݣ�
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
	//API�ص�����
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
		Task task = Task();
		task.task_name = ONLOGINRESPONSE;
		task.task_data = rsp;
		task_queue.push(task);
	}

	static void OnLogoutPush(CLIENT client, outtype& type)
	{
		Task task = Task();
		task.task_name = ONLOGOUTPUSH;
		task.task_data = type;
		task_queue.push(task);
	}

	static void OnMarketStatePush(CLIENT client, const char* id, const char* instumentcode, market_state& marketstate)
	{
		Task task = Task();
		task.task_name = ONMARKETSTATEPUSH;
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
		task_queue.push(task);
	}

	static void OnContractResponse(CLIENT client, response& rsp, const contract* contactinfo, unsigned int number)
	{
		Task task = Task();
		task.task_name = ONCONTRACTRESPONSE;
		task_queue.push(task);
	}

	static void OnAccountResponse(CLIENT client, response& rsp, const account* accountinfo, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONACCOUNTRESPONSE;
		task_queue.push(task);
	}

	static void OnReceiptcollectResponse(CLIENT client, response& rsp, const receipt_collect* receiptcollect, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONRECEIPTCOLLECTRESPONSE;
		task_queue.push(task);
	}

	static void OnOrderResponse(CLIENT client, response& rsp, order_res& orderresponse)
	{
		Task task = Task();
		task.task_name = ONORDERRESPONSE;
		task_queue.push(task);
	}

	static void OnQueryorderResponse(CLIENT client, response& rsp, order* queryorder, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONQUERYORDERRESPONSE;
		task_queue.push(task);
	}

	static void OnCancelorderPush(CLIENT client, cancel_order_push& cancelpush)
	{
		Task task = Task();
		task.task_name = ONCANCELORDERPUSH;
		task_queue.push(task);
	}

	static void OnDealPush(CLIENT client, deal& dealinfo)
	{
		Task task = Task();
		task.task_name = ONDEALPUSH;
		task_queue.push(task);
	}

	static void OnQuerydealResponse(CLIENT client, response& rsp, deal* dealinfo, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONQUERYDEALRESPONSE;
		task_queue.push(task);
	}

	static void OnQueryPositioncollectResponse(CLIENT client, response& rsp, position_collect* poscollect, unsigned int num)
	{
		Task task = Task();
		task.task_name = ONQUERYPOSITIONCOLLECTRESPONSE;
		task_queue.push(task);
	}

	static void OnQueryPositiondetailResponse(CLIENT client, response& rsp, position_detail* posdetail, unsigned int len)
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
	//task������
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
			};
		}
	};


	void processClientClosed(Task task);

	void processClientConnected(Task task);

	void processClientDisConnected(Task task);

	void processClienthandshaked(Task task);

	void processQuotationInfo(Task task);

	virtual void onClientClosed(int type) {};

	virtual void onClientConnected() {};

	virtual void onClientDisConnected(int code) {};

	virtual void onClienthandshaked(bool IsSuccess, int index, string code) {};

	virtual void onQuotationInfo(dict data) {};

	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	void createHFPTdApi(string id, string license);

	void init();

	void connectTradeFront(string tradeFrontAddress, int tradePort);

	int reqUserLogin(dict req, int nRequestID);

	int reqUserLogout(dict req, int nRequestID);

	long long getservertime(CLIENT client);

	SEQ associator_request(CLIENT);
};
