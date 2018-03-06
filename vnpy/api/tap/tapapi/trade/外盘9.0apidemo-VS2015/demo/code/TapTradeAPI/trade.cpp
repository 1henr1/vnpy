#include "Trade.h"
#include "iTapAPIError.h"
#include "TradeConfig.h"
#include <iostream>
#include <string.h>

using namespace std;

Trade::Trade(void):
	m_pAPI(NULL),
	m_bIsAPIReady(false)
{
}


Trade::~Trade(void)
{
}


void Trade::SetAPI(ITapTradeAPI *pAPI)
{
	m_pAPI = pAPI;
}


void Trade::printHelp()
{
	cout<<"---------------------------------\n"
		<<"iTapTradeAPI_Demo CMD\n"
		<<"--->1 : order test \n"
		<<"--->2 : cancle order test\n"
		<<"--->3 : qry fund test\n"
		<<"--->4 : qry hold test\n"
		<<"--->5 : modify order test\n"
		<<"--->6 : qry commdity\n"
		<<"--->7 : qry account\n"
		<<"--->H : help\n"
		<<"----------------------------------\n"
		<<endl;
}

void Trade::loopCmd()
{
	this->printHelp();
	while(1)
	{
		char cmd;
		cin>>cmd;
		switch (cmd)
		{
			case '1': this->t_insertOrder(); break;
			case '2': this->t_canncelOrder();break;
			case '3': this->t_qryFund();break;
			case '4': this->t_qryPostion();break;
			case '5': this->t_modify();break;
			case '6': this->t_qryCommdity(); break;
			case '7': this->t_qryAccount(); break;
			case 'H': this->printHelp();break;
			default:
				break;
		}
	}
}


void Trade::t_insertOrder()
{
	TAPIINT32 iErr = TAPIERROR_SUCCEED;
		//下单
	TapAPINewOrder stNewOrder;


	strcpy(stNewOrder.AccountNo, DEFAULT_ACCOUNT_NO);			
	strcpy(stNewOrder.ExchangeNo, DEFAULT_EXCHANGE_NO);		
	stNewOrder.CommodityType = DEFAULT_COMMODITY_TYPE;		
	strcpy(stNewOrder.CommodityNo, DEFAULT_COMMODITY_NO);		
	strcpy(stNewOrder.ContractNo, DEFAULT_CONTRACT_NO);				
	stNewOrder.OrderType = DEFAULT_ORDER_TYPE;			
	stNewOrder.OrderSource = TAPI_ORDER_SOURCE_ESUNNY_API;		
	stNewOrder.TimeInForce = TAPI_ORDER_TIMEINFORCE_GFD;		
	stNewOrder.OrderSide = DEFAULT_ORDER_SIDE;						
	stNewOrder.OrderPrice = DEFAULT_ORDER_PRICE;		
	stNewOrder.OrderQty = DEFAULT_ORDER_QTY;					
	
	m_uiSessionID = 0;
	iErr = m_pAPI->InsertOrder(&m_uiSessionID, &stNewOrder);
	if(TAPIERROR_SUCCEED != iErr) {
		cout << "InsertOrder Error:" << iErr <<endl;
		return;
	}
}

void Trade::t_canncelOrder()
{
	TAPISTR_20 orderno;
	cout<<"cancle operator :insert orderno"<<endl;
	cin>>orderno;

	TapAPIOrderCancelReq req;
	memset(&req,0,sizeof(req));
	strncpy_s(req.OrderNo,orderno,strlen(orderno));

	TAPIUINT32 reqid = 0;
	TAPIINT32 iErr = TAPIERROR_SUCCEED;

	iErr = m_pAPI->CancelOrder(&reqid,&req);
	if(iErr!= TAPIERROR_SUCCEED){
		cout<< "cancle Error:"<<iErr<<endl;
	}
}

void Trade::t_qryFund()
{
	TapAPIFundReq req;
	memset(&req,0,sizeof(req));
	strncpy_s(req.AccountNo,DEFAULT_ACCOUNT_NO,strlen(DEFAULT_ACCOUNT_NO));

	TAPIINT32 iErr = TAPIERROR_SUCCEED;
	iErr = m_pAPI->QryFund(&m_uiSessionID,&req);
	if(iErr!= TAPIERROR_SUCCEED){
		cout<<"t_qryFund Error:"<<iErr<<endl;
	}
}

void Trade::t_qryAccount()
{
	TapAPIAccQryReq req;
	memset(&req,0,sizeof(req));

	TAPIINT32 iErr = TAPIERROR_SUCCEED;
	iErr = m_pAPI->QryAccount(&m_uiSessionID,&req);
	if(iErr!= TAPIERROR_SUCCEED){
		cout<<"t_qryFund Error:"<<iErr<<endl;
	}
}

void Trade::t_qryPostion()
{
	TapAPIPositionQryReq req;
	memset(&req,0,sizeof(req));
	strncpy_s(req.AccountNo,DEFAULT_ACCOUNT_NO,strlen(DEFAULT_ACCOUNT_NO));

	TAPIINT32 iErr = TAPIERROR_SUCCEED;
	iErr = m_pAPI->QryPosition(&m_uiSessionID,&req);
	if(iErr!= TAPIERROR_SUCCEED){
		cout<<"t_qryHold Error:"<<iErr<<endl;
	}
}

void Trade::t_modify()
{
	TapAPIAmendOrder req;
	TAPISTR_20	orderNo;
	TAPIREAL64 orderPrice;
	TAPIUINT32 orderQty;		
	cout<<"t_modify test"<<endl;
	cout<<"insert orderno"<<endl;
	cin>>orderNo;
	cout<<"insert price"<<endl;
	cin>>orderPrice;
	cout<<"insert qty"<<endl;
	cin>>orderQty;
	
	strncpy_s(req.OrderNo,orderNo,strlen(orderNo));
	strncpy_s(req.ReqData.AccountNo,DEFAULT_ACCOUNT_NO,strlen(DEFAULT_ACCOUNT_NO));
	req.ReqData.OrderPrice = orderPrice;
	req.ReqData.OrderQty = orderQty;

	TAPIINT32  iErr = TAPIERROR_SUCCEED;
	iErr=m_pAPI->AmendOrder(&m_uiSessionID,&req);
	if(iErr!=TAPIERROR_SUCCEED){
		cout<<"t_modify Error:"<<iErr<<endl;
	}
}

void Trade::t_qryCommdity()
{
	TAPIINT32 iErr = TAPIERROR_SUCCEED;
	iErr = m_pAPI->QryCommodity(&m_uiSessionID);
}

void Trade::RunTest()
{
	if(NULL == m_pAPI) {
		cout << "Error: m_pAPI is NULL." << endl;
		return;
	}

	TAPIINT32 iErr = TAPIERROR_SUCCEED;


	//设定服务器IP、端口
	iErr = m_pAPI->SetHostAddress(DEFAULT_IP, DEFAULT_PORT);
	if(TAPIERROR_SUCCEED != iErr) {
		cout << "SetHostAddress Error:" << iErr <<endl;
		return;
	}

	//登录服务器
	TapAPITradeLoginAuth stLoginAuth;
	memset(&stLoginAuth, 0, sizeof(stLoginAuth));
	strcpy(stLoginAuth.UserNo, DEFAULT_USERNAME);
	strcpy(stLoginAuth.Password, DEFAULT_PASSWORD);
	stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
	iErr = m_pAPI->Login(&stLoginAuth);
	if(TAPIERROR_SUCCEED != iErr) {
		cout << "Login Error:" << iErr <<endl;
		return;
	}
	

	//等待APIReady
	m_Event.WaitEvent();
	if (!m_bIsAPIReady){
		cout << "API is not ready";
		return;
	}

	this->loopCmd();


	while (true)
	{
		m_Event.WaitEvent();
	}	
}

void TAP_CDECL Trade::OnConnect()
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspLogin( TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo )
{
	if(TAPIERROR_SUCCEED == errorCode) {
		cout << "登录成功，等待API初始化..." << endl;

	} else {
		cout << "登录失败，错误码:" << errorCode << endl;
		m_Event.SignalEvent();	
	}
}

void TAP_CDECL Trade::OnAPIReady()
{
	cout << "API初始化完成" << endl;
	m_bIsAPIReady = true;
	m_Event.SignalEvent();	
}

void TAP_CDECL Trade::OnDisconnect( TAPIINT32 reasonCode )
{
	cout << "API断开,断开原因:"<<reasonCode << endl;
}

void TAP_CDECL Trade::OnRspChangePassword( TAPIUINT32 sessionID, TAPIINT32 errorCode )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspSetReservedInfo( TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryAccount( TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
	cout << info->AccountNo << "资金账号," 
		<< info->AccountType << " 账号类型," 
		<< info->AccountState << "账号状态," 
		<< info->AccountTradeRight << "交易状态," 
		<< info->CommodityGroupNo << "可交易品种组," 
		<< info->AccountShortName << "账号简称," 
		<< info->AccountEnShortName << endl;

}

void TAP_CDECL Trade::OnRspQryFund( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info )
{
	cout << __FUNCTION__ << " is called." << endl;

	cout <<"errorcode ="<<errorCode<<endl;

	if(isLast=='Y') return;

	cout<<"账户名称:"<<info->AccountNo<<","
		<<"币种:"<<info->CurrencyNo<<","
		<<"权益:"<<info->Balance<<","
		<<"可用:"<<info->Available<<","
		<<"非LME浮盈:"<<info->PositionProfit<<","
		<<"LME浮盈:"<<info->LmePositionProfit<<endl;
	
	
}

void TAP_CDECL Trade::OnRtnFund( const TapAPIFundData *info )
{
//	cout << __FUNCTION__ << " is called." << endl;
//	cout<<"账户名称:"<<info->AccountNo<<","
//		<<"币种:"<<info->CurrencyNo<<","
//		<<"权益:"<<info->Balance<<","
//		<<"可用:"<<info->Available<<","
//		<<"非LME浮盈:"<<info->PositionProfit<<","
//		<<"LME浮盈:"<<info->LmePositionProfit<<endl;
}

void TAP_CDECL Trade::OnRspQryExchange( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryCommodity( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryContract( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnContract( const TapAPITradeContractInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnOrder( const TapAPIOrderInfoNotice *info )

{
	cout << __FUNCTION__ << " is called." << endl;

	if(NULL == info){
		return;
	}

	if (info->ErrorCode != 0) {
		cout << "服务器返回了一个关于委托信息的错误：" << info->ErrorCode << endl;
	} else {
		if (info->OrderInfo) {
			if (0!= info->OrderInfo->ErrorCode){
				cout << "报单失败，"
					<< "错误码:"<<info->OrderInfo->ErrorCode << ","
					<< "委托编号:"<<info->OrderInfo->OrderNo
					<<endl;
			} else{
				cout << "报单成功，"
					<< "品种:"<<info->OrderInfo->CommodityNo<< ","
					<<"合约:"<<info->OrderInfo->ContractNo<<","
					<< "状态:"<<info->OrderInfo->OrderState << ","
					<< "委托编号:"<<info->OrderInfo->OrderNo<< ","
					<<endl;
			}
		}
	}

}

void TAP_CDECL Trade::OnRspOrderAction( TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryOrder( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryOrderProcess( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRspQryFill( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnFill( const TapAPIFillInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
	cout<< "品种" <<info->CommodityNo << ","
		<< "合约" <<info->ContractNo << ","
		<< "成交价"<<info->MatchPrice << ","
		<< "成交量"<<info->MatchQty <<endl;
}

void TAP_CDECL Trade::OnRspQryPosition( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;

	cout <<"errorcode ="<<errorCode<<endl;

	if(isLast=='Y') return;

	cout<< "品种" << info->CommodityNo <<","
	<< "合约" << info->ContractNo << ","
	<< "持仓量"<< info->PositionQty <<","
	<< "持仓价"<< info->PositionPrice<<endl;

}

void TAP_CDECL Trade::OnRtnPosition( const TapAPIPositionInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
	cout<< "品种" << info->CommodityNo <<","
		<< "合约" << info->ContractNo << ","
		<< "持仓量"<< info->PositionQty <<","
		<< "持仓价"<< info->PositionPrice<<endl;
}

void TAP_CDECL Trade::OnRspQryClose( TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnClose( const TapAPICloseInfo *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}

void TAP_CDECL Trade::OnRtnPositionProfit( const TapAPIPositionProfitNotice *info )
{
	cout << __FUNCTION__ << " is called." << endl;
}
void TAP_CDECL Trade::OnRspQryCurrency(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICurrencyInfo *info)
{

}

void TAP_CDECL Trade::OnRspQryTradeMessage(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeMessage *info)
{
}

void TAP_CDECL Trade::OnRtnTradeMessage(const TapAPITradeMessage *info)
{
}

void TAP_CDECL Trade::OnRspQryHisOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisOrderQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryHisOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisOrderProcessQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryHisMatch(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisMatchQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryHisPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisPositionQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryHisDelivery(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIHisDeliveryQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryAccountCashAdjust(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountCashAdjustQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryBill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIBillQryRsp *info)
{
}

void TAP_CDECL Trade::OnExpriationDate(ITapTrade::TAPIDATE date, int days)
{
}

void TAP_CDECL Trade::OnRspQryAccountFeeRent(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountFeeRentQryRsp *info)
{
}

void TAP_CDECL Trade::OnRspQryAccountMarginRent(ITapTrade::TAPIUINT32 sessionID, ITapTrade::TAPIINT32 errorCode, ITapTrade::TAPIYNFLAG isLast, const ITapTrade::TapAPIAccountMarginRentQryRsp *info)
{

}
