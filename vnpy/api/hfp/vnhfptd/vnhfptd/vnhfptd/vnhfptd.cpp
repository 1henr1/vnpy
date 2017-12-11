// vnhfpmd.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "vnhfptd.h"
#include "HFPTradingClient.h"

using namespace hfp;

///-------------------------------------------------------------------------------------
///��Python����C++����ת���õĺ���
///-------------------------------------------------------------------------------------

void getInt(dict d, string key, int *value)
{
	if (d.has_key(key))		//����ֵ����Ƿ���ڸü�ֵ
	{
		object o = d[key];	//��ȡ�ü�ֵ
		extract<int> x(o);	//������ȡ��
		if (x.check())		//���������ȡ
		{
			*value = x();	//��Ŀ������ָ�븳ֵ
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
			//���ַ���ָ�븳ֵ����ʹ��strcpy_s, vs2013ʹ��strcpy����ͨ����
			//+1Ӧ������ΪC++�ַ����Ľ�β���ţ������ر�ȷ�����������1�����
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
///C++�Ļص����������ݱ��浽������
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
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	associator item = any_cast<associator>(task.task_data);
	dict pdict;
	pdict["id"] = ToString(item.id);//��Ա����
	pdict["fullname"] = ToString(item.fullname);//ȫ��
	pdict["clientid"] = ToString(item.clientids[0]);//ȫ��
	pdict["clientsize"] = item.clientsize;//ȫ��
	pdict["bankid"] = ToString(item.bankid);//����id (0 �ް�����;1 �������������ϳ�֧��;2 ��������;3 ũҵ����;4 ��������)

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
	pdict["contractid"] = ToString(contractinfo.contractid);//��Լ����
	pdict["contractname"] = ToString(contractinfo.contractname);//��Լ����
	pdict["initprice"] = contractinfo.initprice;//��ʼ��
	pdict["beginday"] = ToString(contractinfo.beginday);//��ʼ������(yymmddhh24mi)
	pdict["endday"] = ToString(contractinfo.endday);//����������(yymmddhh24mi)
	pdict["uplimit"] = contractinfo.uplimit;//����޼�
	pdict["downlimit"] = contractinfo.downlimit;//����޼�
	pdict["maxqty"] = contractinfo.maxqty;//��������걨��
	pdict["minqty"] = contractinfo.minqty;//������С�걨��
	pdict["mindiffprice"] = contractinfo.mindiffprice;//��С�䶯��
	pdict["mindeliveryqty"] = contractinfo.mindeliveryqty;//��С������
	pdict["taxrate"] = contractinfo.taxrate;//˰��
	pdict["positionlimit"] = contractinfo.positionlimit;//ȫ�г��޲�
	pdict["fuseuplimit"] = contractinfo.fuseuplimit;//�۶�����
	pdict["fusedownlimit"] = contractinfo.fusedownlimit;//�۶�����
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
	pdict["memberid"] = ToString(item.memberid);//��Ա����
	pdict["curname"] = ToString(item.curname);//��������
	pdict["avlb"] = item.avlb;//�����ʽ�
	pdict["occp"] = item.occp;//��֤��(ʵ��)
	pdict["frzord"] = item.frzord;//��������
	pdict["frzrisk"] = item.frzrisk;//��ض���
	pdict["ins"] = item.ins;//���
	pdict["outs"] = item.outs;//����
	pdict["poundage"] = item.poundage;//����������
	pdict["cnybalance"] = item.cnybalance;//ת��ӯ��
	pdict["managefee"] = item.managefee;//���
	pdict["adjust"] = item.adjust;//�����ʽ�
	pdict["financing"] = item.financing;//�����ʽ�
	pdict["balance"] = item.balance;//����۲�
	pdict["funddynamic"] = item.funddynamic;//��̬Ȩ��
	pdict["fundstatic"] = item.fundstatic;//��̬̬Ȩ��
	pdict["frzpayment"] = item.frzpayment;//�������
	pdict["fundonway"] = item.fundonway;//�����ʽ�
	pdict["occpshould"] = item.occpshould;//��֤��(Ӧ��)
	pdict["avlbfundout"] = item.avlbfundout;//�ɳ��ʽ�
	this->onAccountResponse(pdict);
}

void TdApi::processReceiptcollectResponse(Task task)
{
	PyLock lock;
	receipt_collect item = any_cast<receipt_collect>(task.task_data);
	dict pdict;
	pdict["memberid"] = ToString(item.memberid);//��Ա����
	pdict["goodsid"] = ToString(item.goodsid);//��Ʒ����
	pdict["goodsname"] = ToString(item.goodsname);//��Ʒ����
	pdict["totalqty"] = item.totalqty;//����
	pdict["avlbqty"] = item.avlbqty;//������
	pdict["frzord"] = item.frzord;//����������
	pdict["frzrisk"] = item.frzrisk;//��ض�����
	pdict["occp"] = item.occp;//�ֲ�ռ����
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
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	order_res item = any_cast<order_res>(task.task_data);
	dict pdict;
	pdict["orderid"] = ToString(item.orderid);//��������
	pdict["ordertime"] = item.ordertime;//����ʱ��
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["contractid"] = ToString(item.contractid);//��Լ����
	pdict["clientid"] = ToString(item.clientid);//�ͻ�����
	pdict["isbuy"] = item.isbuy;//�Ƿ���
	pdict["offsetflag"] = (int)item.offsetflag;//��ƽ�ֱ��
	pdict["ordertype"] = (int)item.ordertype;//��������
	pdict["isdeposit"] = item.isdeposit;//�Ƿ񶨽�
	pdict["price"] = item.price;//�۸�
	pdict["qty"] = item.qty;//����
	pdict["leftqty"] = item.leftqty;//ʣ������
	pdict["operatorid"] = ToString(item.operatorid);//�µ�����Ա
	pdict["validate"] = item.validate;//��Ч��
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
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
#ifdef _DEBUG
	fprintf(fp, " errdes=%s    %d \n", rsp.errdesc, __LINE__);
	fflush(fp);
#endif
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	order item = any_cast<order>(task.task_data);
	dict pdict;
#ifdef _DEBUG
	fprintf(fp, " orderid=%s    %d \n", item.orderid, __LINE__);
	fflush(fp);
#endif
	pdict["orderid"] = ToString(item.orderid);//��������
	pdict["ordertime"] = item.ordertime;//����ʱ��
#ifdef _DEBUG
	fprintf(fp, " marketid=%s    %d \n", item.marketid, __LINE__);
	fflush(fp);
#endif
	pdict["marketid"] = ToString(item.marketid);//������id
#ifdef _DEBUG
	fprintf(fp, " contractid=%s    %d \n", item.contractid, __LINE__);
	fflush(fp);
#endif
	pdict["contractid"] = ToString(item.contractid);//��Լ����
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["clientid"] = ToString(item.clientid);//�ͻ�����
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["isbuy"] = item.isbuy;//�Ƿ���
	pdict["offsetflag"] = (int)(item.offsetflag);//��ƽ�ֱ��
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["ordertype"] = (int)(item.ordertype);//��������
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["isdeposit"] = item.isdeposit;//�Ƿ񶨽�
	pdict["price"] = item.price;//�۸�
	pdict["qty"] = item.qty;//����
	pdict["leftqty"] = item.leftqty;//ʣ������
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["operatorid"] = ToString(item.operatorid);//�µ�����Ա
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["validate"] = item.validate;//��Ч��
	pdict["state"] = (int)(item.state);//����״̬
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["canceloperator"] = ToString(item.canceloperator);//��������Ա
#ifdef _DEBUG
	fprintf(fp, "     %s:%d \n", __FUNCTION__, __LINE__);
	fflush(fp);
#endif
	pdict["canceltime"] = item.canceltime;//����ʱ��
	pdict["cancelqty"] = item.cancelqty;//��������
	this->onQueryorderResponse(prsp, pdict);

}

void TdApi::processCancelorderPush(Task task)
{
	PyLock lock;
	cancel_order_push item = any_cast<cancel_order_push>(task.task_data);
	dict pdict;
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["orderid"] = ToString(item.orderid);//��������
	pdict["canceltime"] = item.canceltime;//����ʱ��
	pdict["qty"] = item.qty;//��������
	pdict["operatorid"] = ToString(item.operatorid);//��������Ա
	this->onCancelorderPush(pdict);
}

void TdApi::processCancelorderResponse(Task task)
{
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	cancel_order_res item = any_cast<cancel_order_res>(task.task_data);
	dict pdict;
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["orderid"] = ToString(item.orderid);//��������
	pdict["canceltime"] = item.canceltime;//����ʱ��
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
	pdict["dealid"] = ToString(item.dealid);//�ɽ�����
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["contractid"] = ToString(item.contractid);//��Լ����
	pdict["clientid"] = ToString(item.clientid);//�ͻ�����
	pdict["isbuy"] = item.isbuy;//�Ƿ���
	pdict["offsetflag"] = (int)(item.offsetflag);//��ƽ�ֱ��
	pdict["dealtype"] = (int)(item.dealtype);//�ɽ�����
	pdict["operatetype"] = (int)(item.operatetype);//��������
	pdict["price"] = item.price;//�ɽ���
	pdict["qty"] = item.qty;//�ɽ���
	pdict["isdeposit"] = item.isdeposit;//�Ƿ񶨽�
	pdict["deposit"] = item.deposit;//������(Ӧ��)
	pdict["poundage"] = item.poundage;//������
	pdict["open_cost"] = item.open_cost;//���ֳɱ�
	pdict["balance"] = item.balance;//ƽ��ӯ��
	pdict["spread"] = item.spread;//ƽ�ּ۲�
	pdict["detailid"] = ToString(item.detailid);//�ֲ���ϸ��
	pdict["dealtime"] = item.dealtime;//�ɽ�ʱ��
	pdict["orderid"] = ToString(item.orderid);//��������
	pdict["occpdif"] = item.occpdif;//��ʵ�ձ�֤��ı仯������Ӧ�ʽ�account->occp�ֶΣ�,�ɽ��ر��и��ֶ���Ч����ѯ���سɽ���Ϣ�и��ֶ�Ϊ0
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
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	deal item = any_cast<deal>(task.task_data);
	dict pdict;
	pdict["dealid"] = ToString(item.dealid);//�ɽ�����
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["contractid"] = ToString(item.contractid);//��Լ����
	pdict["clientid"] = ToString(item.clientid);//�ͻ�����
	pdict["isbuy"] = item. isbuy;//�Ƿ���
	pdict["offsetflag"] = (int)(item.offsetflag);//��ƽ�ֱ��
	pdict["dealtype"] = (int)(item.dealtype);//�ɽ�����
	pdict["operatetype"] = (int)(item.operatetype);//��������
	pdict["price"] = item.price;//�ɽ���
	pdict["qty"] = item.qty;//�ɽ���
	pdict["isdeposit"] = item.isdeposit;//�Ƿ񶨽�
	pdict["deposit"] = item.deposit;//������(Ӧ��)
	pdict["poundage"] = item.poundage;//������
	pdict["open_cost"] = item.open_cost;//���ֳɱ�
	pdict["balance"] = item.balance;//ƽ��ӯ��
	pdict["spread"] = item.spread;//ƽ�ּ۲�
	pdict["detailid"] = ToString(item.detailid);//�ֲ���ϸ��
	pdict["dealtime"] = item.dealtime;//�ɽ�ʱ��
	pdict["orderid"] = ToString(item.orderid);//��������
	pdict["occpdif"] = item.occpdif;//��ʵ�ձ�֤��ı仯������Ӧ�ʽ�account->occp�ֶΣ�,�ɽ��ر��и��ֶ���Ч����ѯ���سɽ���Ϣ�и��ֶ�Ϊ0
	this->onQuerydealResponse(prsp, pdict);
}

void TdApi::processQueryPositioncollectResponse(Task task)
{
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	position_collect item = any_cast<position_collect>(task.task_data);
	dict pdict;
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["clientid"] = ToString(item.clientid);//�ͻ�����
	pdict["contractid"] = ToString(item.contractid);//��Լ����
	pdict["isbuy"] = item.isbuy;//�Ƿ���ֲ�
	pdict["isdeposit"] = item.isdeposit;//�Ƿ񶨽�ֲ�
	pdict["totalqty"] = item.totalqty;//�ֲܳ���
	pdict["totalqtytoday"] = item.totalqtytoday;//����ֲܳ���
	pdict["totalcost"] = item.totalcost;//�ֲֳܳɱ�
	pdict["avlbqty"] = item.avlbqty;//���óֲ�
	pdict["avlbqtytoday"] = item.avlbqtytoday;//��ֿ��óֲ�
	pdict["deposit"] = item.deposit;//�ܶ���
	pdict["frzord"] = item.frzord;//��������
	pdict["frzrisk"] = item.frzrisk;//��ض���
	pdict["frztoday"] = item.frztoday;//ƽ�񶳽�
	pdict["balance"] = item.balance;//�ֲּ۲�
	pdict["opencost"] = item.opencost;//�ܿ��ֳɱ�
	pdict["frzdelivery"] = item.frzdelivery;//���ն���
	this->onQueryPositioncollectResponse(prsp, pdict);
}

void TdApi::processQueryPositiondetailResponse(Task task)
{
	PyLock lock;
	response rsp = any_cast<response>(task.task_error);
	dict prsp;
	prsp["success"] = rsp.success;//�����Ƿ�ɹ�
	prsp["errcode"] = rsp.errcode;//������
	prsp["errdesc"] = ToString(rsp.errdesc);//��������
	prsp["sequence"] = rsp.sequence;//������ˮ��

	position_detail item = any_cast<position_detail>(task.task_data);
	dict pdict;
	pdict["id"] = ToString(item.id);//��ϸ��
	pdict["clientid"] = ToString(item.clientid);//�ͻ�����
	pdict["marketid"] = ToString(item.marketid);//������id
	pdict["contractid"] = ToString(item.contractid);//��Լ����
	pdict["isbuy"] = item.isbuy;//�Ƿ���ֲ�
	pdict["isdeposit"] = item.isdeposit;//�Ƿ񶨽�ֲ�
	pdict["openprice"] = item.openprice;//���ּ�
	pdict["positionprice"] = item.positionprice;//�ֲּ�
	pdict["dealqty"] = item.dealqty;//�ɽ�����
	pdict["leftqty"] = item.leftqty;//ʣ������
	pdict["istoday"] = item.istoday;//�Ƿ���
	pdict["dealtime"] = item.dealtime;//�ɽ�ʱ��
	pdict["frzrisk"] = item.frzrisk;//��ض���
	pdict["frzdelivery"] = item.frzdelivery;//���ն���
	this->onQueryPositiondetailResponse(prsp, pdict);

}

///-------------------------------------------------------------------------------------
///��������
///-------------------------------------------------------------------------------------
void TdApi::createHFPTdApi(string id, string license)
{
	//�����ͻ���
	clientSeq = client(id.c_str(), license.c_str(), true, hfp::client_type::trading);//����

	//���ûص�����
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
///Boost.Python��װ
///-------------------------------------------------------------------------------------

struct TdApiWrap : TdApi, wrapper < TdApi >
{
	virtual void onClientClosed(int type)
	{
		//���µ�try...catch...����ʵ�ֲ�׽python�����д���Ĺ��ܣ���ֹC++ֱ�ӳ���ԭ��δ֪�ı���
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
	PyEval_InitThreads();	//����ʱ���У���֤�ȴ���GIL

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
