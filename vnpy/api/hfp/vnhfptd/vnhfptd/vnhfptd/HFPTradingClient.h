#ifndef __HFP_TRADING_CLIENT__
#define __HFP_TRADING_CLIENT__

#ifdef _WIN32
#ifndef HFP_API_IMPORT
#define HFP_API  extern "C" __declspec(dllexport)
#else
#define HFP_API extern "C"  __declspec(dllimport)
#endif
#else
#define HFP_API  __attribute__ ((visibility("default")))
#endif
#pragma once
namespace hfp
{
#ifdef _WIN32
	typedef unsigned long __w64 CLIENT;
#else
	typedef unsigned long CLIENT;
#endif
	typedef long long SEQ;

	typedef enum
	{
		trading,//交易客户端
		quotation//行情客户端
	} client_type;

	typedef enum
	{
		server_force = 1,//服务端强制断开
		client_close = 2,//客户端断开
		timeout = 3,//连接超时断开
		none = 0
	} close_type;
	/**
	 * 应答头
	 */
	typedef struct
	{
		bool success;//请求是否成功
		int errcode;//错误码
		const char *errdesc;//错误描述
		SEQ sequence;//请求流水号
	} response;
	
	/**
	 * 退出类型
	 */
	typedef enum
	{
		NORMAL = 1,//正常退出
		TIMEOUT = 2,//超时退出
		FORCEOUT = 3,//强制踢出
		OTHERIN = 4//同帐号登录踢出
	} outtype;

	/**
	 * 市场状态
	 */
	typedef enum
	{
		CLOSE = 0,//闭市
		INIT = 1,//初始化
		AUCTION_ORDERING = 2, //集合竞价申报
		AUCTION_MATCH = 3, //集合竞价撮合
		TRADING = 4, //连续交易
		PAUSE = 5, //暂停

		CLOSING_DEAL = 'E'//收市处理
	} market_state;

	/**
	 * 熔断状态
	 */
	typedef enum
	{
		BeforeFuse = 'B',//熔断前
		Fusing = 'T',//熔断中
		Fused = 'A', //已熔断
	} fuse_state;

	/**
	 * 市场熔断状态信息
	 */
	typedef struct
	{
		const char *marketid;//交易所id
		const char *contractid;//合约编码
		market_state marketstate;//市场状态
		fuse_state fusestate;//熔断状态
	} market_fuse;

	/**
	 * 会员信息
	 */
	typedef struct
	{
		const char *id;//会员编码
		const char *fullname;//全称
		const char **clientids;//客户编码
		unsigned int clientsize;//客户编码数
		const char *bankid;//银行id (0 无绑定银行;1 工商银行无锡南长支行;2 工商银行;3 农业银行;4 中信银行)
	} associator;

	/**
	 * 交易所信息
	 */
	typedef struct
	{
		const char *id;//交易所id
		const char *name;//交易所名称
		const char *curname;//交易所币种名称
	} market;

	/**
	 * 合约类型
	 */
	typedef enum
	{
		future = 1,//中远期
		swap = 4//掉期
	} contract_type;

	/**
	 * 交收类型
	 */
	typedef enum
	{
		goods = 1,//实物交收
		cash = 2//现金交割
	} delivery_type;

	/**
	 * 合约状态
	 */
	typedef enum
	{
		trade = 1,//可交易
		nottrade = 0//不可交易
	} contract_status;

	/**
	 * 合约信息
	 */
	typedef struct
	{
		const char *marketid;//交易所id
		const char *contractid;//合约编码
		const char *contractname;//合约名称
		contract_type contracttype;//合约类型
		const char *goodsid;//商品id
		const char *goodsname;//商品名称
		long long initprice;//初始价
		const char *beginday;//开始交易日(yymmddhh24mi)
		const char *endday;//结束交易日(yymmddhh24mi)
		long long uplimit;//最高限价
		long long downlimit;//最低限价
		long long maxqty;//单笔最大申报量
		long long minqty;//单笔最小申报量
		long long mindiffprice;//最小变动价
		long long mindeliveryqty;//最小交收量
		float taxrate;//税率
		delivery_type deliverytype;//交收类型
		long long positionlimit;//全市场限仓
		contract_status status;//合约状态
		long long fuseuplimit;//熔断上限
		long long fusedownlimit;//熔断下限
	} contract;

	/**
	 * 资金信息
	 */
	typedef struct
	{
		const char *memberid;//会员编码
		const char *curname;//币种名称
		long long avlb;//可用资金
		long long occp;//保证金(实收)
		long long frzord;//报单冻结
		long long frzrisk;//风控冻结
		long long ins;//入金
		long long outs;//出金
		long long poundage;//交易手续费
		long long cnybalance;//转让盈亏
		long long managefee;//会费
		long long adjust;//调整资金
		long long financing;//融资资金
		long long balance;//账面价差
		long long funddynamic;//动态权益
		long long fundstatic;//静态态权益
		long long frzpayment;//冻结货款
		long long fundonway;//待出资金
		long long occpshould;//保证金(应收)
		long long avlbfundout;//可出资金
	} account;

	/**
	 * 仓单汇总信息
	 */
	typedef struct
	{
		const char *memberid;//会员编码
		const char *goodsid;//商品编码
		const char *goodsname;//商品名称
		long long totalqty;//总量
		long long avlbqty;//可用量
		long long frzord;//报单冻结量
		long long frzrisk;//风控冻结量
		long long occp;//持仓占用量
	} receipt_collect;

	/**
	 * 开平仓标记
	 */
	typedef enum
	{
		open = 1,//开仓
		close = 2,//平仓
		forceclose = 3,//强制平仓
	} offset_flag;
	/**
	 * 报单类型
	 */
	typedef enum
	{
		normal = 0,//默认
		close_today = 1//平今
	} order_type;

	typedef enum
	{
		nodeal = 1,//未成交
		partdeal = 2,//部分成交
		alldeal = 3,//完全成交
		canceled = 6,//已撤单
		invalid = 122//作废
	} order_state;

	/**
	 * 报单应答信息
	 */
	typedef struct
	{
		const char *orderid;//报单编码
		long long ordertime;//报单时间
		const char *marketid;//交易所id
		const char *contractid;//合约编码
		const char *clientid;//客户编码
		bool isbuy;//是否买
		offset_flag offsetflag;//开平仓标记
		order_type ordertype;//报单类型
		bool isdeposit;//是否定金
		long long price;//价格
		long long qty;//数量
		long long leftqty;//剩余数量
		const char *operatorid;//下单操作员
		long long validate;//有效期
	} order_res;
	/**
	 * 报单信息
	 */
	typedef struct : order_res
	{
		order_state state;//报单状态
		const char *canceloperator;//撤单操作员
		long long canceltime;//撤单时间
		long long cancelqty;//撤单数量
	} order;

	/**
	 * 撤单应答信息
	 */
	typedef struct
	{
		const char *marketid;//交易所id
		const char *orderid;//报单编码
		long long canceltime;//撤单时间
	} cancel_order_res;
	/**
	 * 撤单通告信息
	 */
	typedef struct : cancel_order_res
	{
		long long qty;//撤单数量
		const char *operatorid;//撤单操作员
	} cancel_order_push;

	/**
	 * 成交类型
	 */
	typedef order_type deal_type;
	/**
	 * 操作类型
	 */
	typedef enum
	{
		agreement_default = 0,//正常(默认类型)
		agreement_open = 1,//协议开仓
		agreement_close = 2,//协议平仓
		agreement_exchange = 3//协议换手
	} operate_type;

	/**
	 * 成交单信息
	 */
	typedef struct
	{
		const char *dealid;//成交单号
		const char *marketid;//交易所id
		const char *contractid;//合约编码
		const char *clientid;//客户编码
		bool isbuy;//是否买
		offset_flag offsetflag;//开平仓标记
		deal_type dealtype;//成交类型
		operate_type operatetype;//操作类型
		long long price;//成交价
		long long qty;//成交量
		bool isdeposit;//是否定金
		long long deposit;//定金金额(应收)
		long long poundage;//手续费
		long long open_cost;//开仓成本
		long long balance;//平仓盈亏
		long long spread;//平仓价差
		const char *detailid;//持仓明细号
		long long dealtime;//成交时间
		const char *orderid;//报单编码
		long long occpdif;//总实收保证金的变化量（对应资金account->occp字段）,成交回报中该字段有效，查询返回成交信息中该字段为0
	} deal;

	/**
	 * 持仓汇总
	 */
	typedef struct
	{
		const char *marketid;//交易所id
		const char *clientid;//客户编码
		const char *contractid;//合约编码
		bool isbuy;//是否买持仓
		bool isdeposit;//是否定金持仓
		long long totalqty;//总持仓量
		long long totalqtytoday;//今仓总持仓量
		long long totalcost;//总持仓成本
		long long avlbqty;//可用持仓
		long long avlbqtytoday;//今仓可用持仓
		long long deposit;//总定金
		long long frzord;//报单冻结
		long long frzrisk;//风控冻结
		long long frztoday;//平今冻结
		long long balance;//持仓价差
		long long opencost;//总开仓成本
		long long frzdelivery;//交收冻结
	} position_collect;
	/**
	 * 持仓明细
	 */
	typedef struct
	{
		const char *id;//明细号
		const char *clientid;//客户编码
		const char *marketid;//交易所id
		const char *contractid;//合约编码
		bool isbuy;//是否买持仓
		bool isdeposit;//是否定金持仓
		long long openprice;//开仓价
		long long positionprice;//持仓价
		long long dealqty;//成交数量
		long long leftqty;//剩余数量
		bool istoday;//是否今仓
		long long dealtime;//成交时间
		long long frzrisk;//风控冻结
		long long frzdelivery;//交收冻结
	} position_detail;
	/**
	*签约解约应答
	*/
	typedef struct
	{
		const char *code;//状态代码
		const char *codeInfo;//状态信息
		const char *clientID;//客户编号
	} sign_break_rese;

	/**
	 * 出入金信息
	 */
	typedef struct
	{
		long long id;//出入金ID
		long long transferTime;//出入金时间
		const char *currency;//币种id
		long long amount;//金额
		const char *inouttype;//出入金类型
		const char *state;//状态
	} fundinout_detail;
	/**
	 * 行情返回码
	 */
	typedef enum
	{
		Success = 0,//成功
		InvalidLicense = 1,//License非法
		InvalidIP = 2,//IP非法
		NotEnabledLicense = 3,//License未生效
		OverdueLicense = 4,//License已失效
		ExceedMaxConnection = 5//超过最大连接数限制
	} quotation_res;
	/**
	 * 行情信息
	 */
	typedef struct
	{
		long long seq;//流水号
		int datetime;//发生时间(秒)
		char contract_id[10];//合约编码
		char contract_name[20];//合约名称
		char marketId[8];//市场编码
		int open;//开盘价
		int high;//最高价
		int low;//最低价
		int _new;//最新价
		int last_close;//昨日收盘价
		int average;//均价
		long long total_volume;//总成交量
		long long total_amount;//总成交额
		long long subs_volume;//总持仓量
		int cur_volume;//现量
		int type;//行情类型 1-集合竞价申报,2-集合竞价成交,3-申报,4-成交
		int down_limit;//最低限价
		int up_limit;//最高限价
		int balance_price;//盈亏计算价
		int updown_base;//涨跌幅基准价
		int last_subs_volume;//昨日持仓
		int *buy_price;//买价
		int *buy_volume;//买量
		int *sell_price;//卖价
		int *sell_volume;//卖量
	} quotation_data;

	/**
	 * 第三方支付：绑卡状态
	 */
	typedef enum
	{
		checking = 0,//待审核
		checked = 1,//审核
		overrule = 2,//驳回
	} pay_bindcardstate;
	/**
	 * 第三方支付：绑定银行卡信息
	 */
	typedef struct
	{
		const char * bankcardno;//银行卡号
		const char * bankcardname;//开户行名称
		const char * bankcode;//银行编码
		const char * regioncode;//银行归属地编码
		const char * mobile;//预留手机
		pay_bindcardstate state;//状态：0-待审核;1-审核;2-驳回
	} pay_bindcardinfo;
	/**
	 * 第三方支付：银行信息
	 */
	typedef struct
	{
		const char * bankcode;//银行编码
		const char * bankname;//银行名称
		bool ishot;//是否热门银行
		long long singlelimit;//单笔限额：0则没有限制
		long long daylimit;//日累计限额：0则没有限制
	} pay_bankinfo;
	/**
	 * 第三方支付：银行归属地信息
	 */
	typedef struct
	{
		const char * regioncode;//银行归属地编码
		const char * regionName;//银行归属地名称
	} pay_regioninfo;

		/**
	 * 保证金收取方式
	 */
	typedef enum
	{
		openprice = 0,//开仓价
		positionprice = 1,//持仓价
	} deposit_price_type;

	/**
	 * 保证金率信息
	 */
	typedef struct
	{
		const char * maketid;//交易所id
		const char * contractid;//合约编码
		bool buyratio;//买是否比率
		long long buyvalue;//买保证金率
		bool sellratio;//卖是否比率
		long long sellvalue;//卖保证金率
		deposit_price_type pricetype;//保证金收取方式
	} deposit_info;

	/**
	 * 费率信息
	 */
	typedef struct
	{
		const char * maketid;//交易所id
		const char * contractid;//合约编码
		bool openbuyratio;//买开是否比率
		long long openbuyvalue;//买开费率
		bool opensellratio;//卖开是否比率
		long long opensellvalue;//卖开费率
		bool closebuyratio;//买平是否比率
		long long closebuyvalue;//买平费率
		bool closesellratio;//卖平是否比率
		long long closesellvalue;//卖平费率
		bool closetodaybuyratio;//买平金是否比率
		long long closetodaybuyvalue;//买平金费率
		bool closetodaysellratio;//卖平金是否比率
		long long closetodaysellvalue;//卖平金费率
	} fee_info;

	/**
	 * 创建客户端
	 * @param const char *id 授权ID
	 * @param const char *license 授权码
	 * @param bool encryption 是否加密
	 * @param client_type 客户端类型
	 *
	 * @return 客户端
	 */
	HFP_API CLIENT client(const char* = 0, const char* = 0, bool = false, client_type = trading);
	/**
	 * 设置socket参数
	 * @param CLIENT 客户端
	 * @param int level
	 * @param int optname
	 * @param const char *optval
	 * @param int optlen
	 *
	 * @return 是否成功
	 */
	HFP_API bool setoption(CLIENT, int, int, const char*, int);
	/**
	 * 获取socket参数
	 * @param CLIENT 客户端
	 * @param int level
	 * @param int optname
	 * @param char *optval 返回
	 * @param int *optlen 返回
	 *
	 * @return 是否成功
	 */
	HFP_API bool getoption(CLIENT, int, int, char*, int*);
	/**
	 * 连接保持设置
	 * @param CLIENT 客户端
	 * @param bool onoff
	 * @param unsigned long keepalivetime (milliseconds)
	 * @param unsigned long keepaliveinterval (milliseconds)
	 *
	 * @return 是否成功
	 */
	HFP_API bool setkeepalive(CLIENT, bool, unsigned long, unsigned long);
	/**
	 * 设置使用者版本号
	 * @param CLIENT 客户端
	 * @param const char* 版本字符串
	 */
	HFP_API void setuserversion(CLIENT, const char*);
	/**
	 * 获取使用者版本号
	 * @param CLIENT 客户端
	 * @return const char*
	 */
	HFP_API const char* getuserversion(CLIENT);
	/**
	 * 释放客户端
	 * @param 客户端
	 */
	HFP_API void freeclient(CLIENT);
	/**
	 * 连接
	 * @param CLIENT 客户端
	 * @param ip 对端IP
	 * @param port 对端端口
	 */
	HFP_API void connect(CLIENT, const char*, unsigned short);

	/**
	 * 设置连接事件
	 * @param CLIENT 客户端
	 * @param 回调函数
	 */
	HFP_API void setonconnected(CLIENT, void (*)(CLIENT));
	/**
	 * 设置连接失败事件
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		int 错误码
	 */
	HFP_API void setonconnectfail(CLIENT, void (*)(CLIENT, int));
	/**
	 * 设置通信握手成功事件
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		bool 是否握手成功
	 *		int errcode 错误码
	 *		const char * 错误描述
	 */
	HFP_API void setonhandshaked(CLIENT, void (*)(CLIENT, bool, int, const char*));
	/**
	 * 设置连接关闭事件
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		int close type
	 */
	HFP_API void setonclosed(CLIENT, void (*)(CLIENT, close_type));

	/**
	 * 请求验证码
	 * @param CLIENT 客户端
	 * @return SEQ 流水号
	 */
	HFP_API SEQ verifycode_request(CLIENT);
	/**
	 * 设置验证码应答事件
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response& 应答返回
	 *		const char * 图片字节
	 *		unsigned int 图片字节长度
	 */
	HFP_API void setonverifycode_response(CLIENT, void (*)(CLIENT, response&, const char*, unsigned int));
	/**
	 * 登录
	 * @param CLIENT 客户端
	 * @param const char* userId
	 * @param const char* passwrod
	 * @param const char* verify code
	 * @param const char* language
	 * @param const char* loginType : 客户端登入方式(3-手机客户端 4-第三方厂商客户端)
	 * @param const char* loginIp : 客户端登入IP，需填写外网IP
	 * @return SEQ 流水号
	 */
	HFP_API SEQ login_request(CLIENT, const char*, const char*, const char* =0, const char* = 0, const char* = 0,const char* = 0);
	/**
	 * 设置登录应答回调函数
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response& 应答返回
	 *		const char * 操作员编码
	 */
	HFP_API void setonlogin_response(CLIENT, void(*)(CLIENT, response&, const char*));
	/**
	 * 获取服务器时间(登录成功后调用)
	 * @param CLIENT 客户端
	 * @return long long 服务器时间
	 */
	HFP_API long long getservertime(CLIENT);
	/**
	 * 登出
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ logout_request(CLIENT);
	/**
	 * 登出推送
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		outtype& 退出类型
	 */
	HFP_API void setonlogout_push(CLIENT, void(*)(CLIENT, outtype&));
	/**
	 * 市场状态变化推送
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		const char* 交易所id
	 *		const char* 合约编码 IF == 0 表示交易所状态
	 *		market_state& 状态
	 */
	HFP_API void setonmarketstate_push(CLIENT, void(*)(CLIENT, const char*, const char*, market_state&));

	/**
	 * 市场状态变化推送：包含熔断状态，与setonmarketstate_push()设置的回调函数不重复回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		const char* 交易所id
	 *		const char* 合约编码 IF == 0 表示交易所状态
	 *		market_state& 市场状态
	 *		fuse_state& 熔断状态
	 */
	HFP_API void setonmarketfuse_push(CLIENT, void(*)(CLIENT, const char*, const char*, market_state&, fuse_state&));

	/**
	 * 修改密码
	 * @param CLIENT 客户端
	 * @param const char * 旧密码
	 * @param const char * 新密码
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ modifypassword_request(CLIENT, const char*, const char*);
	/**
	 * 设置修改密码应答回调函数
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 */
	HFP_API void setonmodifypassword_response(CLIENT, void(*)(CLIENT, response&));

	/**
	 * 请求会员信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ associator_request(CLIENT);
	/**
	 * 设置会员信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		associator & 会员信息
	 */
	HFP_API void setonassociator_response(CLIENT, void(*)(CLIENT, response&, associator&));
	
	/**
	 * 请求交易所信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ market_request(CLIENT);
	/**
	 * 设置交易所信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		market * 交易所信息
	 *     unsigned int 交易所信息记录数
	 */
	HFP_API void setonmarket_response(CLIENT, void(*)(CLIENT, response&, const market*, unsigned int));

	/**
	 * 请求合约信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ contract_request(CLIENT client);
	/**
	 * 设置合约信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		contract* 合约信息
	 *		unsigned int 合约信息数
	 */
	HFP_API void setoncontract_response(CLIENT, void(*)(CLIENT, response&, const contract*, unsigned int));

	/**
	 * 请求资金信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ account_request(CLIENT);
	/**
	 * 设置资金信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		associator* 资金信息
	 *		unsigned int 资金记录数
	 */
	HFP_API void setonaccount_response(CLIENT, void(*)(CLIENT, response&, const account*, unsigned int));

	/**
	 * 请求仓单汇总信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ receiptcollect_request(CLIENT);
	/**
	 * 设置仓单汇总信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		receipt_collect* 仓单汇总信息
	 *		unsigned int 仓单信息数
	 */
	HFP_API void setonreceiptcollect_response(CLIENT, void(*)(CLIENT, response&, const receipt_collect*, unsigned int));
	
	/**
	 * 下单
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 合约编码
	 * @param const char * 客户编码
	 * @param bool 是否买
	 * @param offset_falg 开平仓标记
	 * @param order_type 报单类型
	 * @param bool 是否定金方式
	 * @param long long 报单价
	 * @param long long 报单量
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ order_request(CLIENT, const char*, const char*, const char*, bool, offset_flag, order_type, bool, long long, long long);
	/**
	 * 设置报单应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		order_res & 报单应答信息
	 */
	HFP_API void setonorder_response(CLIENT, void(*)(CLIENT, response&, order_res&));

	/**
	 * 撤单
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 报单编码
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ cancelorder_request(CLIENT, const char*, const char*);
	/**
	 * 设置撤单应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		cancel_order_res & 撤单应答
	 */
	HFP_API void setoncancelorder_response(CLIENT, void (*)(CLIENT, response&, cancel_order_res&));
	/**
	 * 设置撤单回报回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		cancel_order_push & 撤单信息
	 */
	HFP_API void setoncancelorder_push(CLIENT, void(*)(CLIENT, cancel_order_push&));


	/**
	 * 设置成交回报回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		deal & 成交单信息
	 */
	HFP_API void setondeal_push(CLIENT, void(*)(CLIENT, deal&));

	/**
	 * 查询报单请求
	 * @param CLIENT 客户端
	 * @param const char * 交易所编码
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ queryorder_request(CLIENT, const char*);
	/**
	 * 设置查询报单应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		order* 报单列表指针
	 *		unsigned int 报单列表记录数
	 */
	HFP_API void setonqueryorder_response(CLIENT, void(*)(CLIENT, response&, order*, unsigned int));

	/**
	 * 查询成交单信息
	 * @param CLIENT 客户端
	 * @param const char * 交易所编码
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ querydeal_request(CLIENT, const char*);
	/**
	 * 设置成交单查询应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		deal* 成交单列表指针
	 *		unsigned int 成交单列表记录数
	 */
	HFP_API void setonquerydeal_response(CLIENT, void(*)(CLIENT, response&, deal*, unsigned int));
	
	/**
	 * 查询持仓汇总
	 * @param CLIENT 客户端
	 * @param const char * 交易所编码
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ querypositioncollect_request(CLIENT, const char*);
	/**
	 * 设置持仓汇总查询应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		position_collect* 持仓汇总列表指针
	 *		unsigned int 持仓汇总列表记录数
	 */
	HFP_API void setonquerypositioncollect_response(CLIENT, void(*)(CLIENT, response&, position_collect*, unsigned int));

	/**
	 * 查询持仓明细
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ querypositiondetail_request(CLIENT, const char*);
	/**
	 * 设置查询持仓明细应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		position_detail* 持仓明细列表指针
	 *		unsigned int 持仓明细列表记录数
	 */
	HFP_API void setonquerypositiondetail_response(CLIENT, void(*)(CLIENT, response&, position_detail*, unsigned int len));

	/**
	 * 银行签约
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 请求流水号
	 * @param const char * 客户签名
	 * @param const char * 客户名称
	 * @param const char * 市场名称
	 * @param const char * 是否需要双人符合标志 是-"2" 否-null
	 * @param const char * 是否验证客户名标志 是-"2" 否-null
	 * @return SEQ 流水号
	 */
	HFP_API SEQ signcontract_request(CLIENT,const char*,const char*,const char*,const char*, const char*, const char*, const char*);
	/**
	 * 设置银行签约回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		sign_break_rese& 银行签约应答
	 */
	HFP_API void setonsigncontract_response(CLIENT, void(*)(CLIENT, response&, sign_break_rese&));

	/**
	 * 银行解约
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 请求流水号
	 * @param const char * 客户签名
	 * @param const char * 客户名称
	 * @param const char * 市场名称
	 * @return SEQ 流水号
	 */
	HFP_API SEQ canclecontract_request(CLIENT,const char*,const char*,const char*,const char*, const char*);
	/**
	 * 设置银行解约回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		sign_break_rese& 银行解约应答
	 */
	HFP_API void setoncanclecontract_response(CLIENT, void(*)(CLIENT, response&, sign_break_rese&));

	/**
	 * 入金
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 请求流水号
	 * @param const char * 客户签名
	 * @param long long 金额
	 * @param const char * 备注
	 * @param const char * 币种
	 * @param const char * 出入金密码
	 * @param const char * 银行密码
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ fundIn_request(CLIENT,const char*,const char*,const char*,long long,const char*, const char*, const char* = 0, const char* = 0);
	/**
	 * 设置入金回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 */
	HFP_API void setonfundIn_response(CLIENT, void(*)(CLIENT, response&));

	/**
	 * 出金
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 请求流水号
	 * @param const char * 客户签名
	 * @param long long 金额
	 * @param const char * 备注
	 * @param const char * 币种
	 * @param const char * 出入金密码
	 * @param const char * 银行密码
	 * 
	 * @return SEQ 流水号
	 */
	HFP_API SEQ fundOut_request(CLIENT,const char*,const char*,const char*,long long,const char*, const char*, const char* = 0, const char* = 0);
	/**
	 * 设置出金回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 */
	HFP_API void setonfundOut_response(CLIENT, void(*)(CLIENT, response&));

	/**
	 * 出入金查询
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param const char * 类型
	 * @param long long 开始时间 没有限制则0
	 * @param long long 结束时间 没有限制则0
	 * @param const char* 出入金状态
	 * @return SEQ 流水号
	 */
	HFP_API SEQ fundInOutQuery_request(CLIENT,const char*,const char*,long long, long long,const char*);
	/**
	 * 设置出入金查询回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		fundinout_detail* 出入金明细列表指针
	 *		unsigned int 出入金明细记录数
	 */
	HFP_API void setonfundInOutQuery_response(CLIENT, void(*func)(CLIENT, response&,fundinout_detail*, unsigned int));

	/**
	 * 出金申请撤销
	 * @param CLIENT 客户端
	 * @param const char * 交易所id
	 * @param long long 出金申请id
	 * @return SEQ 流水号
	 */
	HFP_API SEQ canclefundout_request(CLIENT,const char*,long long);
	/**
	 * 设置出金申请撤销回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 */
	HFP_API void setoncanclefundout_response(CLIENT, void(*func)(CLIENT, response&));
	/**
	 * 获取报表校验串
	 * @param CLIENT 客户端
	 * @param 请求路径 相对于用户报表根目录
	 * @param 开始日期 (yyyyMMdd)
	 * @param 结束日期 (yyyyMMdd)
	 */
	HFP_API const char* getreportparamsvalue(CLIENT, const char*, const char*, const char*);

	/**
	 * 设置行情回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		quotation_data &行情信息
	 */
	HFP_API void setonquotation(CLIENT, void(*)(CLIENT, quotation_data&));

	/**
	 * 请求行情
	 * @param CLIENT 客户端
	 * @param const char **合约编码
	 * @param int 合约编码数量
	 */
	HFP_API void quotation_request(CLIENT, const char*[], int);
	/**
	 * 获取行情档数
	 * @param CLIENT 客户端
	 * @return 行情档数
	 */
	HFP_API int getquotationlevel(CLIENT);
		/**
	 * 第三方支付：绑定银行卡
	 * @param CLIENT 客户端
	 * @param const char * 银行卡号
	 * @param const char * 开户行名称
	 * @param const char * 银行编码
	 * @param const char * 银行账号归属地编码
	 * @param const char * 预留手机号
	 * @return SEQ 流水号
	 */
	HFP_API SEQ paybindcard_request(CLIENT,const char*,const char*,const char*,const char*,const char*);
	/**
	 * 第三方支付：设置绑定银行卡回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		const char* 银行返回错误码
	 *		const char* 银行返回错误信息
	 */
	HFP_API void setonpaybindcard_response(CLIENT, void(*)(CLIENT, response&,const char*,const char*));
	/**
	 * 第三方支付：解绑银行卡
	 * @param CLIENT 客户端
	 * @param const char * 银行卡号
	 * @return SEQ 流水号
	 */
	HFP_API SEQ payunbindcard_request(CLIENT,const char*);
	/**
	 * 第三方支付：设置解绑银行卡回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		const char* 银行返回错误码
	 *		const char* 银行返回错误信息
	 */
	HFP_API void setonpayunbindcard_response(CLIENT, void(*)(CLIENT, response&,const char*,const char*));
	/**
	 * 第三方支付：出金
	 * @param CLIENT 客户端
	 * @param const char * 银行卡号
	 * @param long long 金额
	 * @param const char * 备注
	 * @return SEQ 流水号
	 */
	HFP_API SEQ payfundout_request(CLIENT,const char*,long long,const char*);
	/**
	 * 第三方支付：设置出金回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		const char* 银行返回错误码
	 *		const char* 银行返回错误信息
	 */
	HFP_API void setonpayfundout_response(CLIENT, void(*)(CLIENT, response&,const char*,const char*));
	/**
	 * 第三方支付：入金
	 * @param CLIENT 客户端
	 * @param const char * 银行卡号
	 * @param long long 金额
	 * @param const char * 短信验证码
	 * @param const char * 备注
	 * @return SEQ 流水号
	 */
	HFP_API SEQ payfundin_request(CLIENT,const char*,long long,const char*,const char*);
	/**
	 * 第三方支付：设置入金回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		const char* 银行返回错误码
	 *		const char* 银行返回错误信息
	 */
	HFP_API void setonpayfundin_response(CLIENT, void(*)(CLIENT, response&,const char*,const char*));
	/**
	 * 第三方支付：发送短信验证码
	 * @param CLIENT 客户端
	 * @param const char * 银行卡号
	 * @return SEQ 流水号
	 */
	HFP_API SEQ paymobilecheckcode_request(CLIENT,const char*);
	/**
	 * 第三方支付：设置发送短信验证码回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		const char* 银行返回错误码
	 *		const char* 银行返回错误信息
	 */
	HFP_API void setonpaymobilecheckcode_response(CLIENT, void(*)(CLIENT, response&,const char*,const char*));
	/**
	 * 第三方支付：已绑银行卡查询
	 * @param CLIENT 客户端
	 * @return SEQ 流水号
	 */
	HFP_API SEQ paybindcardinfo_request(CLIENT);
	/**
	 * 第三方支付：设置已绑银行卡查询回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		pay_bindcardinfo * 绑定银行卡信息
	 *		unsigned int 绑定银行卡记录数
	 */
	HFP_API void setonpaybindcardinfo_response(CLIENT, void(*)(CLIENT, response&, pay_bindcardinfo *,unsigned int));
	/**
	 * 第三方支付：银行信息查询
	 * @param CLIENT 客户端
	 * @param const char* 银行名称
	 * @return SEQ 流水号
	 */
	HFP_API SEQ paybankinfo_request(CLIENT, const char*);
	/**
	 * 第三方支付：设置银行信息查询回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		pay_bankinfo * 绑定银行卡信息
	 *		unsigned int 绑定银行卡记录数
	 */
	HFP_API void setonpaybankinfo_response(CLIENT, void(*)(CLIENT, response&, pay_bankinfo *,unsigned int));
	/**
	 * 第三方支付：银行归属地信息查询
	 * @param CLIENT 客户端
	 * @param const char* 银行归属地名称(模糊查询)
	 * @return SEQ 流水号
	 */
	HFP_API SEQ payregioninfo_request(CLIENT, const char*);
	/**
	 * 第三方支付：设置银行归属地信息查询回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答
	 *		pay_regioninfo * 绑定银行卡信息
	 *		unsigned int 绑定银行卡记录数
	 */
	HFP_API void setonpayregioninfo_response(CLIENT, void(*)(CLIENT, response&, pay_regioninfo *,unsigned int));
		/**
	 * 请求保证金率信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ querydepositinfo_request(CLIENT);
	/**
	 * 设置保证金率信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		deposit_info* 保证金率信息
	 *		unsigned int 保证金率记录数
	 */
	HFP_API void setonquerydepositinfo_response(CLIENT, void(*)(CLIENT, response&, deposit_info*, unsigned int));
	/**
	 * 请求费率信息
	 * @param CLIENT 客户端
	 *
	 * @return SEQ 流水号
	 */
	HFP_API SEQ queryfeeinfo_request(CLIENT);
	/**
	 * 设置费率信息应答回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		response & 应答返回
	 *		deposit_info* 费率信息
	 *		unsigned int 费率记录数
	 */
	HFP_API void setonqueryfeeinfo_response(CLIENT, void(*)(CLIENT, response&, fee_info*, unsigned int));
	/**
	 * 设置心跳回调
	 * @param CLIENT 客户端
	 * @param 回调函数
	 *		CLIENT 客户端
	 *		long long 服务器时间:格林威治时间的毫秒数
	 */
	HFP_API void setonheartbeat_push(CLIENT, void(*)(CLIENT,long long));

};
#endif
