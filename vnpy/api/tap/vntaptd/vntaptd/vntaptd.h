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
#include "iTapTradeAPI.h"
#include "iTapAPIError.h"
#include "iTapTradeAPIDataType.h"
#include "iTapAPICommDef.h"

//命名空间
using namespace std;
using namespace boost::python;
using namespace boost;
using namespace ITapTrade;

//常量
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
class TdApi : public ITapTradeAPINotify 
{
private:
	ITapTradeAPI *api;			//API对象
	bool m_bAPIReady;
	TAPIUINT32  m_sessionID;
	TAPISTR_50  m_clientOrderNo;
	thread *task_thread;				//工作线程指针（向python中推送数据）
	ConcurrentQueue<Task> task_queue;	//任务队列

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
	//task：任务
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
	//data：回调函数的数据字典
	//error：回调函数的错误字典
	//id：请求id
	//last：是否为最后返回
	//i：整数
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
	//req:主动函数的请求字典
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

