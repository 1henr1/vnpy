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
#include "iTapTradeAPI.h"
#include "iTapAPIError.h"
#include "iTapTradeAPIDataType.h"
#include "iTapAPICommDef.h"

//�����ռ�
using namespace std;
using namespace boost::python;
using namespace boost;
using namespace ITapTrade;

//����
#define DEFINE_OnConnect                     1
#define DEFINE_OnRspLogin                    2 
#define DEFINE_OnExpriationDate              3       
#define DEFINE_OnAPIReady                    4 
#define DEFINE_OnDisconnect                  5   
#define DEFINE_OnRspChangePassword           6          
#define DEFINE_OnRspSetReservedInfo          7           
#define DEFINE_OnRspQryAccount               8      
#define DEFINE_OnRspQryFund                  9   
#define DEFINE_OnRtnFund                     10
#define DEFINE_OnRspQryExchange              11       
#define DEFINE_OnRspQryCommodity             12        
#define DEFINE_OnRspQryContract              13       
#define DEFINE_OnRtnContract                 14    
#define DEFINE_OnRspOrderAction              15       
#define DEFINE_OnRtnOrder                    16 
#define DEFINE_OnRspQryOrder                 17    
#define DEFINE_OnRspQryOrderProcess          18           
#define DEFINE_OnRspQryFill                  19   
#define DEFINE_OnRtnFill                     20
#define DEFINE_OnRspQryPosition              21       
#define DEFINE_OnRtnPosition                 22    
#define DEFINE_OnRtnPositionProfit           23          
#define DEFINE_OnRspQryCurrency              24       
#define DEFINE_OnRspQryTradeMessage          25           
#define DEFINE_OnRtnTradeMessage             26        
#define DEFINE_OnRspQryHisOrder              27       
#define DEFINE_OnRspQryHisOrderProcess       28              
#define DEFINE_OnRspQryHisMatch              29       
#define DEFINE_OnRspQryHisPosition           30          
#define DEFINE_OnRspQryHisDelivery           31          
#define DEFINE_OnRspQryAccountCashAdjust     32                
#define DEFINE_OnRspQryBill                  33   
#define DEFINE_OnRspQryAccountFeeRent        34             
#define DEFINE_OnRspQryAccountMarginRent     35                
#define DEFINE_OnRspHKMarketOrderInsert      36               
#define DEFINE_OnRspHKMarketOrderDelete      37               
#define DEFINE_OnHKMarketQuoteNotice         38            


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
	TAPISTR_50  m_clientOrderNo;
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
	virtual void TAP_CDECL OnConnect() ;
	virtual void TAP_CDECL OnRspLogin(ITapTrade::TAPIINT32 errorCode, const ITapTrade::TapAPITradeLoginRspInfo *loginRspInfo) ;
	virtual void TAP_CDECL OnExpriationDate(ITapTrade::TAPIDATE date, int days) ;
	virtual void TAP_CDECL OnAPIReady() ;
	virtual void TAP_CDECL OnDisconnect(ITapTrade::TAPIINT32 reasonCode) ;
	virtual void TAP_CDECL OnRspChangePassword(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode) ;
	virtual void TAP_CDECL OnRspSetReservedInfo(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, const ITapTrade::TAPISTR_50 info) ;
	virtual void TAP_CDECL OnRspQryAccount(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIUINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountInfo *info) ;
	virtual void TAP_CDECL OnRspQryFund(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIFundData *info) ;
	virtual void TAP_CDECL OnRtnFund(const ITapTrade::TapAPIFundData *info) ;
	virtual void TAP_CDECL OnRspQryExchange(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIExchangeInfo *info) ;
	virtual void TAP_CDECL OnRspQryCommodity(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPICommodityInfo *info) ;
	virtual void TAP_CDECL OnRspQryContract(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPITradeContractInfo *info) ;
	virtual void TAP_CDECL OnRtnContract(const ITapTrade::TapAPITradeContractInfo *info) ;
	virtual void TAP_CDECL OnRspOrderAction(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, const ITapTrade::TapAPIOrderActionRsp *info) ;
	virtual void TAP_CDECL OnRtnOrder(const ITapTrade::TapAPIOrderInfoNotice *info) ;
	virtual void TAP_CDECL OnRspQryOrder(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIOrderInfo *info) ;
	virtual void TAP_CDECL OnRspQryOrderProcess(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIOrderInfo *info) ;
	virtual void TAP_CDECL OnRspQryFill(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIFillInfo *info) ;
	virtual void TAP_CDECL OnRtnFill(const ITapTrade::TapAPIFillInfo *info) ;
	virtual void TAP_CDECL OnRspQryPosition(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIPositionInfo *info) ;
	virtual void TAP_CDECL OnRtnPosition(const ITapTrade::TapAPIPositionInfo *info) ;
	virtual void TAP_CDECL OnRtnPositionProfit(const ITapTrade::TapAPIPositionProfitNotice *info) ;
	virtual void TAP_CDECL OnRspQryCurrency(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPICurrencyInfo *info) ;
	virtual void TAP_CDECL OnRspQryTradeMessage(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPITradeMessage *info) ;
	virtual void TAP_CDECL OnRtnTradeMessage(const ITapTrade::TapAPITradeMessage *info) ;
	virtual void TAP_CDECL OnRspQryHisOrder(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisOrderQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryHisOrderProcess(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisOrderProcessQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryHisMatch(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisMatchQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryHisPosition(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisPositionQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryHisDelivery(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIHisDeliveryQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryAccountCashAdjust(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountCashAdjustQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryBill(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIBillQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryAccountFeeRent(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountFeeRentQryRsp *info) ;
	virtual void TAP_CDECL OnRspQryAccountMarginRent(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountMarginRentQryRsp *info) ;
	virtual void TAP_CDECL OnRspHKMarketOrderInsert(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode,  const ITapTrade::TapAPIOrderMarketInsertRsp *info) ;
	virtual void TAP_CDECL OnRspHKMarketOrderDelete(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, const ITapTrade::TapAPIOrderMarketDeleteRsp *info) ;
	virtual void TAP_CDECL OnHKMarketQuoteNotice( const ITapTrade::TapAPIOrderQuoteMarketNotice *info) ;


	//-------------------------------------------------------------------------------------
	//task������
	//-------------------------------------------------------------------------------------

	void processTask();
	void processConnect(Task task);
	void processRspLogin(Task task);
	void processExpriationDate(Task task);
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
	void processRspOrderAction(Task task);
	void processRtnOrder(Task task);
	void processRspQryOrder(Task task);
	void processRspQryOrderProcess(Task task);
	void processRspQryFill(Task task);
	void processRtnFill(Task task);
	void processRspQryPosition(Task task);
	void processRtnPosition(Task task);
	void processRtnPositionProfit(Task task);
	void processRspQryCurrency(Task task);
	void processRspQryTradeMessage(Task task);
	void processRtnTradeMessage(Task task);
	void processRspQryHisOrder(Task task);
	void processRspQryHisOrderProcess(Task task);
	void processRspQryHisMatch(Task task);
	void processRspQryHisPosition(Task task);
	void processRspQryHisDelivery(Task task);
	void processRspQryAccountCashAdjust(Task task);
	void processRspQryBill(Task task);
	void processRspQryAccountFeeRent(Task task);
	void processRspQryAccountMarginRent(Task task);
	void processRspHKMarketOrderInsert(Task task);
	void processRspHKMarketOrderDelete(Task task);
	void processHKMarketQuoteNotice(Task task);

	//-------------------------------------------------------------------------------------
	//data���ص������������ֵ�
	//error���ص������Ĵ����ֵ�
	//id������id
	//last���Ƿ�Ϊ��󷵻�
	//i������
	//-------------------------------------------------------------------------------------

	virtual void onConnect() {};
	virtual void onRspLogin(int errorCode, dict data)  {};
	virtual void onExpriationDate(dict data) {};
	virtual void onAPIReady() {};
	virtual void onDisconnect(int reasonCode) {};
	virtual void onRspChangePassword(int errorCode) {};
	virtual void onRspSetReservedInfo(int errorCode, dict data) {};
	virtual void onRspQryAccount(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryFund(int errorCode, bool isLast, dict data) {};
	virtual void onRtnFund(dict data) {};
	virtual void onRspQryExchange(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryCommodity(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryContract(int errorCode, bool isLast, dict data) {};
	virtual void onRtnContract(dict data) {};
	virtual void onRspOrderAction(int errorCode, dict data) {};
	virtual void onRtnOrder(dict data) {};
	virtual void onRspQryOrder(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryOrderProcess(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryFill(int errorCode, bool isLast, dict data) {};
	virtual void onRtnFill(dict data) {};
	virtual void onRspQryPosition(int errorCode, bool isLast, dict data) {};
	virtual void onRtnPosition(dict data) {};
	virtual void onRtnPositionProfit(dict data) {};
	virtual void onRspQryCurrency(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryTradeMessage(int errorCode, bool isLast, dict data) {};
	virtual void onRtnTradeMessage(dict data) {};
	virtual void onRspQryHisOrder(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryHisOrderProcess(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryHisMatch(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryHisPosition(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryHisDelivery(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryAccountCashAdjust(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryBill(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryAccountFeeRent(int errorCode, bool isLast, dict data) {};
	virtual void onRspQryAccountMarginRent(int errorCode, bool isLast, dict data) {};
	virtual void onRspHKMarketOrderInsert(int errorCode,  dict data) {};
	virtual void onRspHKMarketOrderDelete(int errorCode, dict data) {};
	virtual void onHKMarketQuoteNotice( dict data) {};

	//-------------------------------------------------------------------------------------
	//req:���������������ֵ�
	//-------------------------------------------------------------------------------------

	int createTapTraderApi(dict req);
	int setHostAddress(string frontAddress, int port);
	int reqUserLogin(dict req);
	int setVertificateCode(string VertificateCode);
	int exit();
	int qryAccount();
	int qryFund(dict req);
	int qryExchange();
	int qryCommodity();
	int qryContract(dict req);
	int reqInsertOrder(dict req);
	int reqCancelOrder(dict req);
	int reqAmendOrder(dict req);
	int qryOrder(dict req);
	int qryFill();
	int qryPosition();
};

