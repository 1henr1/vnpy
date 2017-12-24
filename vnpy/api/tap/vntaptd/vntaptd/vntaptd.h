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
#include "TapTradeAPI.h"

//�����ռ�
using namespace std;
using namespace boost::python;
using namespace boost;

//����
#define ONCONNECT 1
#define ONRSPLOGIN 2
#define ONAPIREADY 3
#define ONDISCONNECT 4
#define ONRSPCHANGEPASSWORD 5
#define ONRSPSETRESERVEDINFO 6
#define ONRSPQRYACCOUNT 7
#define ONRSPQRYFUND 8
#define ONRTNFUND 9
#define ONRSPQRYEXCHANGE 10
#define ONRSPQRYCOMMODITY 11
#define ONRSPQRYCONTRACT 12
#define ONRTNCONTRACT 13
#define ONRTNORDER 14
#define ONRSPORDERACTION 15
#define ONRSPQRYORDER 16
#define ONRSPQRYORDERPROCESS 17
#define ONRSPQRYFILL 18
#define ONRTNFILL 19
#define ONRSPQRYPOSITION 20
#define ONRTNPOSITION 21
#define ONRTNCLOSE 22
#define ONRTNPOSITIONPROFIT 23
#define ONRSPQRYDEEPQUOTE 24
#define ONRSPQRYEXCHANGESTATEINFO 25
#define ONRTNEXCHANGESTATEINFO 26
#define ONRTNREQQUOTENOTICE 27
#define ONRSPUPPERCHANNELINFO 28

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
class TdApi : public ITapTradeAPINotify 
{
private:
	ITapTradeAPI *api;			//API����
	bool m_bAPIReady;
	TAPIUINT32  m_sessionID;
	thread *task_thread;				//�����߳�ָ�루��python���������ݣ�
	ConcurrentQueue<Task> task_queue;	//�������

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
	virtual void TAP_CDECL OnConnect()  {} ;
	virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo)  {} ;
	virtual void TAP_CDECL OnAPIReady()  {} ;
	virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode)  {} ;
	virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode)  {} ;
	virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info)  {} ;
	virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info)  {} ;
	virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info)  {} ;
	virtual void TAP_CDECL OnRtnFund(const TapAPIFundData *info)  {} ;
	virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info)  {} ;
	virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info)  {} ;
	virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info)  {} ;
	virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo *info)  {} ;
	virtual void TAP_CDECL OnRtnOrder(const TapAPIOrderInfoNotice *info)  {} ;
	virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info)  {} ;
	virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info)  {} ;
	virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info)  {} ;
	virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info)  {} ;
	virtual void TAP_CDECL OnRtnFill(const TapAPIFillInfo *info)  {} ;
	virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info)  {} ;
	virtual void TAP_CDECL OnRtnPosition(const TapAPIPositionInfo *info)  {} ;
	virtual void TAP_CDECL OnRtnClose(const TapAPICloseInfo *info)  {} ;
	virtual void TAP_CDECL OnRtnPositionProfit(const TapAPIPositionProfitNotice *info)  {} ;
	virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp *info)  {} ;
	virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast,const TapAPIExchangeStateInfo * info) {} ;
	virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info) {} ;
	virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info)  {} ;
	virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info) {} ;


	//-------------------------------------------------------------------------------------
	//task������
	//-------------------------------------------------------------------------------------

	void processTask();
	void processConnect(Task task);
	void processRspLogin(Task task);
	void processAPIReady(Task task);
	void processDisconnect(Task task);
	void processRspChangePassword(Task task);
	void processRspSetReservedInfo(Task task);
	void processRspQryAccount(Task task);
	void processRspQryFund(Task task);
	void processRtnFund(Task task);
	void processRspQryExchange(Task task);
	void processRspQryCommodity(Task task);
	void processRspQryContract(Task task);
	void processRtnContract(Task task);
	void processRtnOrder(Task task);
	void processRspOrderAction(Task task);
	void processRspQryOrder(Task task);
	void processRspQryOrderProcess(Task task);
	void processRspQryFill(Task task);
	void processRtnFill(Task task);
	void processRspQryPosition(Task task);
	void processRtnPosition(Task task);
	void processRtnClose(Task task);
	void processRtnPositionProfit(Task task);
	void processRspQryDeepQuote(Task task);
	void processRspQryExchangeStateInfo(Task task);
	void processRtnExchangeStateInfo(Task task);
	void processRtnReqQuoteNotice(Task task);
	void processRspUpperChannelInfo(Task task);

	//-------------------------------------------------------------------------------------
	//data���ص������������ֵ�
	//error���ص������Ĵ����ֵ�
	//id������id
	//last���Ƿ�Ϊ��󷵻�
	//i������
	//-------------------------------------------------------------------------------------

	virtual void  onConnect()  {} ;
	virtual void  onRspLogin(int errorCode, dict data)  {} ;
	virtual void  onAPIReady()  {} ;
	virtual void  onDisconnect(int reasonCode)  {} ;
	virtual void  onRspChangePassword( int errorCode)  {} ;
	virtual void  onRspSetReservedInfo( int errorCode, dict data)  {} ;
	virtual void  onRspQryAccount( TAPIUINT32 errorCode, bool isLast, dict data)  {} ;
	virtual void  onRspQryFund( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRtnFund(dict data)  {} ;
	virtual void  onRspQryExchange( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRspQryCommodity( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRspQryContract( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRtnContract(dict data)  {} ;
	virtual void  onRtnOrder(int errorCode, dict data)  {} ;
	virtual void  onRspOrderAction( TAPIUINT32 errorCode, dict data)  {} ;
	virtual void  onRspQryOrder( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRspQryOrderProcess( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRspQryFill( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRtnFill(dict data)  {} ;
	virtual void  onRspQryPosition( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRtnPosition(dict data)  {} ;
	virtual void  onRtnClose(dict data)  {} ;
	virtual void  onRtnPositionProfit(dict data)  {} ;
	virtual void  onRspQryDeepQuote( int errorCode, bool isLast, dict data)  {} ;
	virtual void  onRspQryExchangeStateInfo(int errorCode, bool isLast,dict data) {} ;
	virtual void  onRtnExchangeStateInfo(dict data) {} ;
	virtual void  onRtnReqQuoteNotice(dict data)  {} ;
	virtual void  onRspUpperChannelInfo(int errorCode, bool isLast, dict data) {} ;

	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	int createTapTraderApi(dict req);
	int setHostAddress(string frontAddress, int port);
	int reqUserLogin(dict req);
	int exit();
	int qryAccount(dict req);
	int qryFund(dict req);
	int qryExchange();
	int qryCommodity();
	int qryContract(dict req);
	int reqInsertOrder(dict req);
	int reqCancelOrder(dict req);
	int qryOrder(dict req);
	int qryFill();
	int qryPosition();
};

