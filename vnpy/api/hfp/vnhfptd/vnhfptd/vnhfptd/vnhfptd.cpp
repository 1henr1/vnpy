// vnhfpmd.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "vnhfptd.h"
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

#define ToString(str) ((str) == NULL ? "" : string(str))

///-------------------------------------------------------------------------------------
///C++的回调函数将数据保存到队列中
///-------------------------------------------------------------------------------------


void TdApi::processClientClosed(Task task)
{
	PyLock lock;
	int type = any_cast<int>(task.task_data);
	this->onClientClosed(type);
}

void TdApi::processClientConnected(Task task)
{
	PyLock lock;
	this->onClientConnected();
}

void TdApi::processClientDisConnected(Task task)
{
	PyLock lock;
	int code = any_cast<int>(task.task_data);
	this->onClientDisConnected(code);
}

void TdApi::processClienthandshaked(Task task)
{
	PyLock lock;
	bool IsSuccess = any_cast<bool>(task.task_error);
	int index = task.task_id;
	string code = any_cast<string>(task.task_data);
	this->onClienthandshaked(IsSuccess, index, code);
}

void TdApi::processLoginResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	response rsp = any_cast<response>(task.task_data);
	dict ploginResponse;
	ploginResponse["success"] = rsp.success;
	ploginResponse["errcode"] = rsp.errcode;
	ploginResponse["errdesc"] = rsp.errdesc;
	ploginResponse["sequence"] = rsp.sequence;
	this->onLoginResponse(ploginResponse);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processLogoutPush(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	outtype type = any_cast<outtype>(task.task_data);
	int ptype = type;
	this->onLogoutPush(ptype);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processMarketStatePush(Task task)
{
	PyLock lock;
	string  instrument = any_cast<string>(task.task_data);
	int status = task.task_id;
	dict pMarketState;
	pMarketState["instrument"] = instrument;
	pMarketState["status"] = status;
	this->onMarketStatePush(pMarketState);
}

void TdApi::processAssociatorResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	associator item = any_cast<associator>(task.task_data);
	dict pdict;
	pdict["id"] = ToString(item.id);//会员编码
	pdict["fullname"] = ToString(item.fullname);//全称
	pdict["clientid"] = ToString(item.clientids[0]);//全称
	pdict["clientsize"] = item.clientsize;//全称
	pdict["bankid"] = ToString(item.bankid);//银行id (0 无绑定银行;1 工商银行无锡南长支行;2 工商银行;3 农业银行;4 中信银行)

	this->onAssociatorResponse(prsp, pdict);
}

void TdApi::processMarketResponse(Task task)
{
	PyLock lock;
	market marketinfo = any_cast<market>(task.task_data);
	dict pdict;
	pdict["curname"] = marketinfo.curname;
	pdict["id"] = marketinfo.id;
	pdict["name"] = marketinfo.name;
	this->onMarketResponse(pdict);
}

void TdApi::processContractResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	contract contractinfo = any_cast<contract>(task.task_data);
	dict pdict;
	pdict["contractid"] = ToString(contractinfo.contractid);//合约编码
	pdict["contractname"] = ToString(contractinfo.contractname);//合约名称
	pdict["initprice"] = contractinfo.initprice;//初始价
	pdict["beginday"] = ToString(contractinfo.beginday);//开始交易日(yymmddhh24mi)
	pdict["endday"] = ToString(contractinfo.endday);//结束交易日(yymmddhh24mi)
	pdict["uplimit"] = contractinfo.uplimit;//最高限价
	pdict["downlimit"] = contractinfo.downlimit;//最低限价
	pdict["maxqty"] = contractinfo.maxqty;//单笔最大申报量
	pdict["minqty"] = contractinfo.minqty;//单笔最小申报量
	pdict["mindiffprice"] = contractinfo.mindiffprice;//最小变动价
	pdict["mindeliveryqty"] = contractinfo.mindeliveryqty;//最小交收量
	pdict["taxrate"] = contractinfo.taxrate;//税率
	pdict["positionlimit"] = contractinfo.positionlimit;//全市场限仓
	pdict["fuseuplimit"] = contractinfo.fuseuplimit;//熔断上限
	pdict["fusedownlimit"] = contractinfo.fusedownlimit;//熔断下限
	this->onContractResponse(pdict);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processAccountResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	account item = any_cast<account>(task.task_data);
	dict pdict;
	pdict["memberid"] = ToString(item.memberid);//会员编码
	pdict["curname"] = ToString(item.curname);//币种名称
	pdict["avlb"] = item.avlb;//可用资金
	pdict["occp"] = item.occp;//保证金(实收)
	pdict["frzord"] = item.frzord;//报单冻结
	pdict["frzrisk"] = item.frzrisk;//风控冻结
	pdict["ins"] = item.ins;//入金
	pdict["outs"] = item.outs;//出金
	pdict["poundage"] = item.poundage;//交易手续费
	pdict["cnybalance"] = item.cnybalance;//转让盈亏
	pdict["managefee"] = item.managefee;//会费
	pdict["adjust"] = item.adjust;//调整资金
	pdict["financing"] = item.financing;//融资资金
	pdict["balance"] = item.balance;//账面价差
	pdict["funddynamic"] = item.funddynamic;//动态权益
	pdict["fundstatic"] = item.fundstatic;//静态态权益
	pdict["frzpayment"] = item.frzpayment;//冻结货款
	pdict["fundonway"] = item.fundonway;//待出资金
	pdict["occpshould"] = item.occpshould;//保证金(应收)
	pdict["avlbfundout"] = item.avlbfundout;//可出资金
	this->onAccountResponse(pdict);
}

void TdApi::processReceiptcollectResponse(Task task)
{
	PyLock lock;
	receipt_collect item = any_cast<receipt_collect>(task.task_data);
	dict pdict;
	pdict["memberid"] = ToString(item.memberid);//会员编码
	pdict["goodsid"] = ToString(item.goodsid);//商品编码
	pdict["goodsname"] = ToString(item.goodsname);//商品名称
	pdict["totalqty"] = item.totalqty;//总量
	pdict["avlbqty"] = item.avlbqty;//可用量
	pdict["frzord"] = item.frzord;//报单冻结量
	pdict["frzrisk"] = item.frzrisk;//风控冻结量
	pdict["occp"] = item.occp;//持仓占用量
	this->onReceiptcollectResponse(pdict);
}

void TdApi::processOrderResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	order_res item = any_cast<order_res>(task.task_data);
	dict pdict;
	pdict["orderid"] = ToString(item.orderid);//报单编码
	pdict["ordertime"] = item.ordertime;//报单时间
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["contractid"] = ToString(item.contractid);//合约编码
	pdict["clientid"] = ToString(item.clientid);//客户编码
	pdict["isbuy"] = item.isbuy;//是否买
	pdict["offsetflag"] = (int)item.offsetflag;//开平仓标记
	pdict["ordertype"] = (int)item.ordertype;//报单类型
	pdict["isdeposit"] = item.isdeposit;//是否定金
	pdict["price"] = item.price;//价格
	pdict["qty"] = item.qty;//数量
	pdict["leftqty"] = item.leftqty;//剩余数量
	pdict["operatorid"] = ToString(item.operatorid);//下单操作员
	pdict["validate"] = item.validate;//有效期
	this->onOrderResponse(prsp, pdict);
#ifdef _DEBUG
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
}

void TdApi::processQueryorderResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
#ifdef _DEBUG
	fprintf(fp, " errdes=%s    %d \n", rsp.errdesc, __LINE__);
	fflush(fp);
#endif
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	order item = any_cast<order>(task.task_data);
	dict pdict;
#ifdef _DEBUG
	fprintf(fp, " orderid=%s    %d \n", item.orderid, __LINE__);
	fflush(fp);
#endif
	pdict["orderid"] = ToString(item.orderid);//报单编码
	pdict["ordertime"] = item.ordertime;//报单时间
#ifdef _DEBUG
	fprintf(fp, " marketid=%s    %d \n", item.marketid, __LINE__);
	fflush(fp);
#endif
	pdict["marketid"] = ToString(item.marketid);//交易所id
#ifdef _DEBUG
	fprintf(fp, " contractid=%s    %d \n", item.contractid, __LINE__);
	fflush(fp);
#endif
	pdict["contractid"] = ToString(item.contractid);//合约编码
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["clientid"] = ToString(item.clientid);//客户编码
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["isbuy"] = item.isbuy;//是否买
	pdict["offsetflag"] = (int)(item.offsetflag);//开平仓标记
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["ordertype"] = (int)(item.ordertype);//报单类型
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["isdeposit"] = item.isdeposit;//是否定金
	pdict["price"] = item.price;//价格
	pdict["qty"] = item.qty;//数量
	pdict["leftqty"] = item.leftqty;//剩余数量
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["operatorid"] = ToString(item.operatorid);//下单操作员
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["validate"] = item.validate;//有效期
	pdict["state"] = (int)(item.state);//报单状态
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["canceloperator"] = ToString(item.canceloperator);//撤单操作员
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["canceltime"] = item.canceltime;//撤单时间
	pdict["cancelqty"] = item.cancelqty;//撤单数量
	this->onQueryorderResponse(prsp, pdict);

}

void TdApi::processCancelorderPush(Task task)
{
	PyLock lock;
	cancel_order_push item = any_cast<cancel_order_push>(task.task_data);
	dict pdict;
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["orderid"] = ToString(item.orderid);//报单编码
	pdict["canceltime"] = item.canceltime;//撤单时间
	pdict["qty"] = item.qty;//撤单数量
	pdict["operatorid"] = ToString(item.operatorid);//撤单操作员
	this->onCancelorderPush(pdict);
}

void TdApi::processCancelorderResponse(Task task)
{
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	cancel_order_res item = any_cast<cancel_order_res>(task.task_data);
	dict pdict;
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["orderid"] = ToString(item.orderid);//报单编码
	pdict["canceltime"] = item.canceltime;//撤单时间
	this->onCancelorderResponse(prsp, pdict);
}

void TdApi::processDealPush(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	deal item = any_cast<deal>(task.task_data);
	dict pdict;
	pdict["dealid"] = ToString(item.dealid);//成交单号
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["contractid"] = ToString(item.contractid);//合约编码
	pdict["clientid"] = ToString(item.clientid);//客户编码
	pdict["isbuy"] = item.isbuy;//是否买
	pdict["offsetflag"] = (int)(item.offsetflag);//开平仓标记
	pdict["dealtype"] = (int)(item.dealtype);//成交类型
	pdict["operatetype"] = (int)(item.operatetype);//操作类型
	pdict["price"] = item.price;//成交价
	pdict["qty"] = item.qty;//成交量
	pdict["isdeposit"] = item.isdeposit;//是否定金
	pdict["deposit"] = item.deposit;//定金金额(应收)
	pdict["poundage"] = item.poundage;//手续费
	pdict["open_cost"] = item.open_cost;//开仓成本
	pdict["balance"] = item.balance;//平仓盈亏
	pdict["spread"] = item.spread;//平仓价差
	pdict["detailid"] = ToString(item.detailid);//持仓明细号
	pdict["dealtime"] = item.dealtime;//成交时间
	pdict["orderid"] = ToString(item.orderid);//报单编码
	pdict["occpdif"] = item.occpdif;//总实收保证金的变化量（对应资金account->occp字段）,成交回报中该字段有效，查询返回成交信息中该字段为0
	this->onDealPush(pdict);
}

void TdApi::processQuerydealResponse(Task task)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	deal item = any_cast<deal>(task.task_data);
	dict pdict;
	pdict["dealid"] = ToString(item.dealid);//成交单号
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["contractid"] = ToString(item.contractid);//合约编码
	pdict["clientid"] = ToString(item.clientid);//客户编码
	pdict["isbuy"] = item. isbuy;//是否买
	pdict["offsetflag"] = (int)(item.offsetflag);//开平仓标记
	pdict["dealtype"] = (int)(item.dealtype);//成交类型
	pdict["operatetype"] = (int)(item.operatetype);//操作类型
	pdict["price"] = item.price;//成交价
	pdict["qty"] = item.qty;//成交量
	pdict["isdeposit"] = item.isdeposit;//是否定金
	pdict["deposit"] = item.deposit;//定金金额(应收)
	pdict["poundage"] = item.poundage;//手续费
	pdict["open_cost"] = item.open_cost;//开仓成本
	pdict["balance"] = item.balance;//平仓盈亏
	pdict["spread"] = item.spread;//平仓价差
	pdict["detailid"] = ToString(item.detailid);//持仓明细号
	pdict["dealtime"] = item.dealtime;//成交时间
	pdict["orderid"] = ToString(item.orderid);//报单编码
	pdict["occpdif"] = item.occpdif;//总实收保证金的变化量（对应资金account->occp字段）,成交回报中该字段有效，查询返回成交信息中该字段为0
	this->onQuerydealResponse(prsp, pdict);
}

void TdApi::processQueryPositioncollectResponse(Task task)
{
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	position_collect item = any_cast<position_collect>(task.task_data);
	dict pdict;
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["clientid"] = ToString(item.clientid);//客户编码
	pdict["contractid"] = ToString(item.contractid);//合约编码
	pdict["isbuy"] = item.isbuy;//是否买持仓
	pdict["isdeposit"] = item.isdeposit;//是否定金持仓
	pdict["totalqty"] = item.totalqty;//总持仓量
	pdict["totalqtytoday"] = item.totalqtytoday;//今仓总持仓量
	pdict["totalcost"] = item.totalcost;//总持仓成本
	pdict["avlbqty"] = item.avlbqty;//可用持仓
	pdict["avlbqtytoday"] = item.avlbqtytoday;//今仓可用持仓
	pdict["deposit"] = item.deposit;//总定金
	pdict["frzord"] = item.frzord;//报单冻结
	pdict["frzrisk"] = item.frzrisk;//风控冻结
	pdict["frztoday"] = item.frztoday;//平今冻结
	pdict["balance"] = item.balance;//持仓价差
	pdict["opencost"] = item.opencost;//总开仓成本
	pdict["frzdelivery"] = item.frzdelivery;//交收冻结
	this->onQueryPositioncollectResponse(prsp, pdict);
}

void TdApi::processQueryPositiondetailResponse(Task task)
{
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//请求是否成功
	prsp["errcode"] = rsp.errcode;//错误码
	prsp["errdesc"] = ToString(rsp.errdesc);//错误描述
	prsp["sequence"] = rsp.sequence;//请求流水号

	position_detail item = any_cast<position_detail>(task.task_data);
	dict pdict;
	pdict["id"] = ToString(item.id);//明细号
	pdict["clientid"] = ToString(item.clientid);//客户编码
	pdict["marketid"] = ToString(item.marketid);//交易所id
	pdict["contractid"] = ToString(item.contractid);//合约编码
	pdict["isbuy"] = item.isbuy;//是否买持仓
	pdict["isdeposit"] = item.isdeposit;//是否定金持仓
	pdict["openprice"] = item.openprice;//开仓价
	pdict["positionprice"] = item.positionprice;//持仓价
	pdict["dealqty"] = item.dealqty;//成交数量
	pdict["leftqty"] = item.leftqty;//剩余数量
	pdict["istoday"] = item.istoday;//是否今仓
	pdict["dealtime"] = item.dealtime;//成交时间
	pdict["frzrisk"] = item.frzrisk;//风控冻结
	pdict["frzdelivery"] = item.frzdelivery;//交收冻结
	this->onQueryPositiondetailResponse(prsp, pdict);

}

///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------
void TdApi::createHFPTdApi(string id, string license)
{
	//创建客户端
	clientSeq = client(id.c_str(), license.c_str(), true, hfp::client_type::trading);//测试

	//设置回调函数
	setuserversion(clientSeq,"AFIS_C++_1.0.0");
	setkeepalive(clientSeq, true, 5000, 5000);
	setonconnected(clientSeq, TdApi::OnClientConnected);
	setonconnectfail(clientSeq, TdApi::OnClientDisConnected);
	setonclosed(clientSeq, TdApi::OnClientClosed);
	setonhandshaked(clientSeq, TdApi::OnClienthandshaked);
	setonlogin_response(clientSeq, TdApi::OnLoginResponse);
	setonlogout_push(clientSeq, TdApi::OnLogoutPush);
	setonmarketstate_push(clientSeq, TdApi::OnMarketStatePush);
	setonassociator_response(clientSeq, TdApi::OnAssociatorResponse);
	setonmarket_response(clientSeq, TdApi::OnMarketResponse);
	setoncontract_response(clientSeq, TdApi::OnContractResponse);
	setonaccount_response(clientSeq, TdApi::OnAccountResponse);
	setonreceiptcollect_response(clientSeq, TdApi::OnReceiptcollectResponse);
	setonorder_response(clientSeq, TdApi::OnOrderResponse);
	setonqueryorder_response(clientSeq, TdApi::OnQueryorderResponse);
	setoncancelorder_push(clientSeq, TdApi::OnCancelorderPush);
	setoncancelorder_response(clientSeq, TdApi::OnCancelorderResponse);
	setondeal_push(clientSeq, TdApi::OnDealPush);
	setonquerydeal_response(clientSeq, TdApi::OnQuerydealResponse);
	setonquerypositioncollect_response(clientSeq, TdApi::OnQueryPositioncollectResponse);
	setonquerypositiondetail_response(clientSeq, TdApi::OnQueryPositiondetailResponse);
	setonquerydepositinfo_response(clientSeq, TdApi::OnQueryDepositinfoResponse);
	setonqueryfeeinfo_response(clientSeq, TdApi::OnQueryfeeinfoResponse);
};

void TdApi::connectTradeFront(string tradeFrontAddress, int tradePort)
{
	connect(clientSeq, (char*)tradeFrontAddress.c_str(), tradePort);
}

SEQ TdApi::reqUserLogin(dict req)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	char userID[100];
	char password[100];
	getStr(req, "userID", userID);
	getStr(req, "password", password);
#ifdef _DEBUG
	fprintf(fp, "userID=%s   password=%s \n", userID, password);
	fflush(fp);
#endif
	return login_request(clientSeq, userID, password, NULL, "ch-ZN", "4", NULL);
}

SEQ TdApi::reqUserLogout()
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	SEQ logoutSeq =  logout_request(clientSeq);
#ifdef _DEBUG
	fprintf(fp, "logout seq = %ld \n", logoutSeq);
	fprintf(fp, "Leaving %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return logoutSeq;
}


long long TdApi::reqServertime()
{
	return getservertime(clientSeq);
}

hfp::SEQ TdApi::reqAssociator()
{
	return associator_request(clientSeq);
}

SEQ TdApi::reqMarket()
{
	return market_request(clientSeq);
}


hfp::SEQ TdApi::reqContract()
{
	return contract_request(clientSeq);
}

hfp::SEQ TdApi::reqAccount()
{
	return account_request(clientSeq);
}

hfp::SEQ TdApi::reqReceiptcollect()
{
	return receiptcollect_request(clientSeq);
}

hfp::SEQ TdApi::reqOrder(string exchangeID, string instrumentID, string  clientID,
	bool isBuy, int offset, int ordertype, bool isMargin, int price, int volume)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fprintf(fp, "exchangeID=%s, instrumentID=%s clientID=%s isBuy=%d offset=%d ordertype=%d isMargin=%d price=%d volume=%d \n",
		exchangeID.c_str(), instrumentID.c_str(), clientID.c_str(), isBuy, offset, ordertype, isMargin, price, volume);
	fflush(fp);
#endif
	return order_request(clientSeq, exchangeID.c_str(), instrumentID.c_str(), clientID.c_str(),
		isBuy, (offset_flag)offset, (order_type)ordertype, isMargin, price, volume);
}

hfp::SEQ TdApi::qryOrder(string sequence)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return queryorder_request(clientSeq, sequence.c_str());
}

hfp::SEQ TdApi::reqCancelorder(string exchangeID, string orderID)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return cancelorder_request(clientSeq, exchangeID.c_str(), orderID.c_str());
}

hfp::SEQ TdApi::qryDeal(string exchangeID)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return querydeal_request(clientSeq, exchangeID.c_str());
}

hfp::SEQ TdApi::qryPositioncollect(string exchangeID)
{
#ifdef _DEBUG
	fprintf(fp, "Entering %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	return querypositioncollect_request(clientSeq, exchangeID.c_str());
}

hfp::SEQ TdApi::qryPositiondetail(string exchangeID)
{
	return querypositiondetail_request(clientSeq, exchangeID.c_str());
}

///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct TdApiWrap : TdApi, wrapper < TdApi >
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

	virtual void onAssociatorResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onAssociatorResponse")(rsp, pdict);
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


	virtual void onLoginResponse(dict data)
	{
		try
		{
			this->get_override("onLoginResponse")(data);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onLogoutPush(int outtype)
	{
		try
		{
			this->get_override("onLogoutPush")(outtype);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onMarketStatePush(dict pMarketState)
	{
		try
		{
			this->get_override("onMarketStatePush")(pMarketState);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
	
	virtual void onMarketResponse(dict pdict)
	{
		try
		{
			this->get_override("onMarketResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onContractResponse(dict pdict)
	{
		try
		{
			this->get_override("onContractResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onAccountResponse(dict pdict)
	{
		try
		{
			this->get_override("onAccountResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onReceiptcollectResponse(dict pdict)
	{
		try
		{
			this->get_override("onReceiptcollectResponse")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onOrderResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onOrderResponse")(rsp, pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onQueryorderResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onQueryorderResponse")(rsp, pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onCancelorderPush(dict pdict)
	{
		try
		{
			this->get_override("onCancelorderPush")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onCancelorderResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onCancelorderResponse")(rsp, pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onDealPush(dict pdict)
	{
		try
		{
			this->get_override("onDealPush")(pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onQuerydealResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onQuerydealResponse")(rsp, pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onQueryPositioncollectResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onQueryPositioncollectResponse")(rsp, pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};

	virtual void onQueryPositiondetailResponse(dict rsp, dict pdict)
	{
		try
		{
			this->get_override("onQueryPositiondetailResponse")(rsp, pdict);
		}
		catch (error_already_set const &)
		{
			PyErr_Print();
		}
	};
};


BOOST_PYTHON_MODULE(vnhfptd)
{
	PyEval_InitThreads();	//导入时运行，保证先创建GIL

	class_<TdApiWrap, boost::noncopyable>("TdApi")
		.def("createHFPTdApi", &TdApiWrap::createHFPTdApi)
		.def("connectTradeFront",&TdApiWrap::connectTradeFront)
		.def("reqUserLogin",&TdApiWrap::reqUserLogin)
		.def("reqUserLogout",&TdApiWrap::reqUserLogout)
		.def("reqServertime",&TdApiWrap::reqServertime)
		.def("reqAssociator",&TdApiWrap::reqAssociator)
		.def("reqMarket",&TdApiWrap::reqMarket)
		.def("reqContract",&TdApiWrap::reqContract)
		.def("reqAccount",&TdApiWrap::reqAccount)
		.def("reqReceiptcollect",&TdApiWrap::reqReceiptcollect)
		.def("reqOrder",&TdApiWrap::reqOrder)
		.def("reqCancelOrder",&TdApiWrap::reqCancelorder)
		.def("qryOrder",&TdApiWrap::qryOrder)
		.def("qryDeal",&TdApiWrap::qryDeal)
		.def("qryPositioncollect",&TdApiWrap::qryPositioncollect)
		.def("qryPositiondetail",&TdApiWrap::qryPositiondetail)

		.def("onClientClosed", pure_virtual(&TdApiWrap::onClientClosed))
		.def("onClientConnected", pure_virtual(&TdApiWrap::onClientConnected))
		.def("onClientDisConnected", pure_virtual(&TdApiWrap::onClientDisConnected))
		.def("onClienthandshaked", pure_virtual(&TdApiWrap::onClienthandshaked))
		.def("onAssociatorResponse", pure_virtual(&TdApiWrap::onAssociatorResponse))
		.def("onLoginResponse", pure_virtual(&TdApiWrap::onLoginResponse))
		.def("onLogoutPush", pure_virtual(&TdApiWrap::onLogoutPush))
		.def("onMarketStatePush", pure_virtual(&TdApiWrap::onMarketStatePush))
		.def("onMarketResponse", pure_virtual(&TdApiWrap::onMarketResponse))
		.def("onContractResponse", pure_virtual(&TdApiWrap::onContractResponse))
		.def("onAccountResponse", pure_virtual(&TdApiWrap::onAccountResponse))
		.def("onReceiptcollectResponse", pure_virtual(&TdApiWrap::onReceiptcollectResponse))
		.def("onOrderResponse", pure_virtual(&TdApiWrap::onOrderResponse))
		.def("onQueryorderResponse", pure_virtual(&TdApiWrap::onQueryorderResponse))
		.def("onCancelorderPush", pure_virtual(&TdApiWrap::onCancelorderPush))
		.def("onCancelorderResponse", pure_virtual(&TdApiWrap::onCancelorderResponse))
		.def("onDealPush", pure_virtual(&TdApiWrap::onDealPush))
		.def("onQuerydealResponse", pure_virtual(&TdApiWrap::onQuerydealResponse))
		.def("onQueryPositioncollectResponse", pure_virtual(&TdApiWrap::onQueryPositioncollectResponse))
		.def("onQueryPositiondetailResponse", pure_virtual(&TdApiWrap::onQueryPositiondetailResponse))

		;
};
