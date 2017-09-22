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
#define ONASSOCIATORRESPONSE 8
#define ONMARKETRESPONSE 9
#define ONCONTRACTRESPONSE 10
#define ONACCOUNTRESPONSE 11
#define ONRECEIPTCOLLECTRESPONSE 12
#define ONORDERRESPONSE 13
#define ONQUERYORDERRESPONSE 14
#define ONCANCELORDERPUSH 15
#define ONDEALPUSH 16
#define ONQUERYDEALRESPONSE 17
#define ONQUERYPOSITIONCOLLECTRESPONSE 18
#define ONQUERYPOSITIONDETAILRESPONSE 19
#define ONQUERYDEPOSITINFORESPONSE 20
#define ONQUERYFEEINFORESPONSE 21


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

#ifdef _DEBUG
FILE *fp = fopen("debug.txt", "w");
#endif

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
				/*
				case ONMARKETSTATEPUSH:
				{
					this->processMarketStatePush(task);
					break;
				}
				case ONASSOCIATORRESPONSE:
				{
					this->processAssociatorResponse(task);
					break;
				}
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

	void processDealPush(Task task);

	void processQuerydealResponse(Task task);

	void processQueryPositioncollectResponse(Task task);

	void processQueryPositiondetailResponse(Task task);

	void processQueryDepositinfoResponse(Task task);

	void processQueryfeeinfoResponse(Task task);

	//-------------------------------------------------------------------------------------
	//��python�ĶԽ�,���º����ᱻpython������
	//-------------------------------------------------------------------------------------

	virtual void onClientClosed(int type) {};

	virtual void onClientConnected() {};

	virtual void onClientDisConnected(int code) {};

	virtual void onClienthandshaked(bool IsSuccess, int index, string code) {};

	virtual void onLoginResponse(dict rsp) {};

	virtual void onLogoutPush(int outtype) {};

	virtual void onMarketStatePush(const char* id, const char* instumentcode, market_state& marketstate) {};

	virtual void onAssociatorResponse(response& rsp, associator& asso) {};

	virtual void onMarketResponse(response& rsp, const market* marketinfo, unsigned int number) {};

	virtual void onContractResponse(response& rsp, const contract* contactinfo, unsigned int number) {};

	virtual void onAccountResponse(response& rsp, const account* accountinfo, unsigned int num) {};

	virtual void onReceiptcollectResponse(response& rsp, const receipt_collect* receiptcollect, unsigned int num) {};

	virtual void onOrderResponse(response& rsp, order_res& orderresponse) {};

	virtual void onQueryorderResponse(response& rsp, order* queryorder, unsigned int num) {};

	virtual void onCancelorderPush(cancel_order_push& cancelpush) {};

	virtual void onDealPush(deal& dealinfo) {};

	virtual void onQuerydealResponse(response& rsp, deal* dealinfo, unsigned int num) {};

	virtual void onQueryPositioncollectResponse(response& rsp, position_collect* poscollect, unsigned int num) {};

	virtual void onQueryPositiondetailResponse(response& rsp, position_detail* posdetail, unsigned int len) {};

	virtual void onQueryDepositinfoResponse(response& rsp, deposit_info* depositinfo, unsigned int num) {};

	virtual void onQueryfeeinfoResponse(response& rsp, fee_info * feeinfo, unsigned int num) {};


	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	void createHFPTdApi(string id, string license);

	void connectTradeFront(string tradeFrontAddress, int tradePort);

	SEQ reqUserLogin(dict req);

	SEQ reqUserLogout();

	long long reqServertime();

	SEQ associator_request(CLIENT);
};
