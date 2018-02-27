# encoding: UTF-8

defineDict = {}
typedefDict = {}




# 账号类型
# 个人客户
defineDict["TAPI_ACCOUNT_TYPE_PERSON"] =  'P'# 机构客户
defineDict["TAPI_ACCOUNT_TYPE_ORGANIZATION"] =  'O'
# 权限编码类型
# 系统删单
defineDict["TAPI_RIGHT_ORDER_DEL"] =  30001# 订单审核
defineDict["TAPI_RIGHT_ORDER_CHECK"] =  30002# 只可查询
defineDict["TAPI_RIGHT_ONLY_QRY"] =  31000# 只可开仓
defineDict["TAPI_RIGHT_ONLY_OPEN"] =  31001# 上期查挂单
defineDict["TAPI_RIGHT_SHFE_QUOTE"] =  31002
# 登录用户身份类型
# 投资者用户
defineDict["TAPI_USERTYPE_CLIENT"] = 10000# 经纪人
defineDict["TAPI_USERTYPE_BROKER"] = 20000# 交易员
defineDict["TAPI_USERTYPE_TRADER"] = 30000# 风控
defineDict["TAPI_USERTYPE_RISK"] = 40000# 管理员
defineDict["TAPI_USERTYPE_MANAGER"] = 50000# 行情
defineDict["TAPI_USERTYPE_QUOTE"] = 60000
# 账号状态
# 正常
defineDict["TAPI_ACCOUNT_STATE_NORMAL"] =  'N'# 销户
defineDict["TAPI_ACCOUNT_STATE_CANCEL"] =  'C'# 休眠
defineDict["TAPI_ACCOUNT_STATE_SLEEP"] =  'S'
# 父子账户类型
# 正常账号
defineDict["TAPI_ACCOUNT_FAMILYTYPE_NOMAL"] = 'N'# 子账户
defineDict["TAPI_ACCOUNT_FAMILYTYPE_CHILD"] = 'C'
# 委托类型
# 市价
defineDict["TAPI_ORDER_TYPE_MARKET"] =  '1'# 限价
defineDict["TAPI_ORDER_TYPE_LIMIT"] =  '2'# 市价止损
defineDict["TAPI_ORDER_TYPE_STOP_MARKET"] =  '3'# 限价止损
defineDict["TAPI_ORDER_TYPE_STOP_LIMIT"] =  '4'# 期权行权
defineDict["TAPI_ORDER_TYPE_OPT_EXEC"] =  '5'# 期权弃权
defineDict["TAPI_ORDER_TYPE_OPT_ABANDON"] =  '6'# 询价
defineDict["TAPI_ORDER_TYPE_REQQUOT"] =  '7'# 应价
defineDict["TAPI_ORDER_TYPE_RSPQUOT"] =  '8'# 互换
defineDict["TAPI_ORDER_TYPE_SWAP"] =  '9'
# 委托来源
# 自助电子单
defineDict["TAPI_ORDER_SOURCE_SELF_ETRADER"] =  '1'# 代理电子单
defineDict["TAPI_ORDER_SOURCE_PROXY_ETRADER"] =  '2'# 外部电子单(外部电子系统下单，本系统录入)
defineDict["TAPI_ORDER_SOURCE_JTRADER"] =  '3'# 人工录入单(外部其他方式下单，本系统录入)
defineDict["TAPI_ORDER_SOURCE_MANUAL"] =  '4'# carry单
defineDict["TAPI_ORDER_SOURCE_CARRY"] =  '5'# 程式化报单
defineDict["TAPI_ORDER_SOURCE_PROGRAM"] =  '6'# 交割行权
defineDict["TAPI_ORDER_SOURCE_DELIVERY"] =  '7'# 期权放弃
defineDict["TAPI_ORDER_SOURCE_ABANDON"] =  '8'# 通道费
defineDict["TAPI_ORDER_SOURCE_CHANNEL"] =  '9'# 易盛API
defineDict["TAPI_ORDER_SOURCE_ESUNNY_API"] =  'A'
# 委托有效类型
# 当日有效
defineDict["TAPI_ORDER_TIMEINFORCE_GFD"] =  '0'# 取消前有效
defineDict["TAPI_ORDER_TIMEINFORCE_GTC"] =  '1'# 指定日期前有效
defineDict["TAPI_ORDER_TIMEINFORCE_GTD"] =  '2'# FAK或IOC
defineDict["TAPI_ORDER_TIMEINFORCE_FAK"] =  '3'# FOK
defineDict["TAPI_ORDER_TIMEINFORCE_FOK"] =  '4'
# 买卖类型
# 无
defineDict["TAPI_SIDE_NONE"] =  'N'# 买入
defineDict["TAPI_SIDE_BUY"] =  'B'# 卖出
defineDict["TAPI_SIDE_SELL"] =  'S'# 双边
defineDict["TAPI_SIDE_ALL"] =  'A'
# 开平类型
# 不分开平
defineDict["TAPI_PositionEffect_NONE"] =  'N'# 开仓
defineDict["TAPI_PositionEffect_OPEN"] =  'O'# 平仓
defineDict["TAPI_PositionEffect_COVER"] =  'C'# 平当日
defineDict["TAPI_PositionEffect_COVER_TODAY"] =  'T'
# 投机保值类型
# 无
defineDict["TAPI_HEDGEFLAG_NONE"] =  'N'# 投机
defineDict["TAPI_HEDGEFLAG_T"] =  'T'# 保值
defineDict["TAPI_HEDGEFLAG_B"] =  'B'# 套利
defineDict["TAPI_HEDGEFLAG_L"] =  'L'
# 委托状态类型
# 终端提交
defineDict["TAPI_ORDER_STATE_SUBMIT"] =  '0'# 已受理
defineDict["TAPI_ORDER_STATE_ACCEPT"] =  '1'# 策略待触发
defineDict["TAPI_ORDER_STATE_TRIGGERING"] =  '2'# 交易所待触发
defineDict["TAPI_ORDER_STATE_EXCTRIGGERING"] =  '3'# 已排队
defineDict["TAPI_ORDER_STATE_QUEUED"] =  '4'# 部分成交
defineDict["TAPI_ORDER_STATE_PARTFINISHED"] =  '5'# 完全成交
defineDict["TAPI_ORDER_STATE_FINISHED"] =  '6'# 待撤消(排队临时状态)
defineDict["TAPI_ORDER_STATE_CANCELING"] =  '7'# 待修改(排队临时状态)
defineDict["TAPI_ORDER_STATE_MODIFYING"] =  '8'# 完全撤单
defineDict["TAPI_ORDER_STATE_CANCELED"] =  '9'# 已撤余单
defineDict["TAPI_ORDER_STATE_LEFTDELETED"] =  'A'# 指令失败
defineDict["TAPI_ORDER_STATE_FAIL"] =  'B'# 策略删除
defineDict["TAPI_ORDER_STATE_DELETED"] =  'C'# 已挂起
defineDict["TAPI_ORDER_STATE_SUPPENDED"] =  'D'# 到期删除
defineDict["TAPI_ORDER_STATE_DELETEDFOREXPIRE"] =  'E'# 已生效——询价成功
defineDict["TAPI_ORDER_STATE_EFFECT"] =  'F'# 已申请——行权、弃权、套利等申请成功
defineDict["TAPI_ORDER_STATE_APPLY"] =  'G'
# 计算方式
# 比例
defineDict["TAPI_CALCULATE_MODE_PERCENTAGE"] =  '1'# 定额
defineDict["TAPI_CALCULATE_MODE_QUOTA"] =  '2'# 绝对方式
defineDict["TAPI_CALCULATE_MODE_ABSOLUTE"] =  '7'
# 成交来源
# 全部
defineDict["TAPI_MATCH_SOURCE_ALL"] =  '0'# 自助电子单
defineDict["TAPI_MATCH_SOURCE_SELF_ETRADER"] =  '1'# 代理电子单
defineDict["TAPI_MATCH_SOURCE_PROXY_ETRADER"] =  '2'# 外部电子单
defineDict["TAPI_MATCH_SOURCE_JTRADER"] =  '3'# 人工录入单
defineDict["TAPI_MATCH_SOURCE_MANUAL"] =  '4'# carry单
defineDict["TAPI_MATCH_SOURCE_CARRY"] =  '5'# 程式化单
defineDict["TAPI_MATCH_SOURCE_PROGRAM"] =  '6'# 交割行权
defineDict["TAPI_MATCH_SOURCE_DELIVERY"] =  '7'# 期权放弃
defineDict["TAPI_MATCH_SOURCE_ABANDON"] =  '8'# 通道费
defineDict["TAPI_MATCH_SOURCE_CHANNEL"] =  '9'# 易盛API
defineDict["TAPI_MATCH_SOURCE_ESUNNY_API"] =  'A'
# 开平方式
# 不区分开平
defineDict["TAPI_CLOSE_MODE_NONE"] =  'N'# 平仓未了结
defineDict["TAPI_CLOSE_MODE_UNFINISHED"] =  'U'# 区分开仓和平仓
defineDict["TAPI_CLOSE_MODE_OPENCOVER"] =  'C'# 区分开仓、平仓和平今
defineDict["TAPI_CLOSE_MODE_CLOSETODAY"] =  'T'
# 期货算法
# 逐笔
defineDict["TAPI_FUTURES_ALG_ZHUBI"] =  '1'# 盯市
defineDict["TAPI_FUTURES_ALG_DINGSHI"] =  '2'
# 期权算法
# 期货方式
defineDict["TAPI_OPTION_ALG_FUTURES"] =  '1'# 期权方式
defineDict["TAPI_OPTION_ALG_OPTION"] =  '2'
# 本外币标识
# 境内人民币账户
defineDict["TAPI_LWFlag_L"] =  'L'# 客户境内外币账户
defineDict["TAPI_LWFlag_W"] =  'W'

# 期货保证金方式
# 分笔
defineDict["TAPI_DEPOSITCALCULATE_MODE_FEN"] =  '1'# 锁仓
defineDict["TAPI_DEPOSITCALCULATE_MODE_SUO"] =  '2'
# 期权保证金公式,据此判断该品种期权采用何种内置计算公式计算保证金

# 组合方向,品种两腿组合合约的买卖方向和第几腿相同
# 和第一腿一致
defineDict["TAPI_CMB_DIRECT_FIRST"] =  '1'# 和第二腿一致
defineDict["TAPI_CMB_DIRECT_SECOND"] =  '2'
# 交割行权方式,期货和期权了结的方式
# 实物交割
defineDict["TAPI_DELIVERY_MODE_GOODS"] =  'G'# 现金交割
defineDict["TAPI_DELIVERY_MODE_CASH"] =  'C'# 期权行权
defineDict["TAPI_DELIVERY_MODE_EXECUTE"] =  'E'# 期权放弃
defineDict["TAPI_DELIVERY_MODE_ABANDON"] =  'A'# 港交所行权
defineDict["TAPI_DELIVERY_MODE_HKF"] =  'H'
# 合约类型
# 交易行情合约
defineDict["TAPI_CONTRACT_TYPE_TRADEQUOTE"] = '1'# 行情合约
defineDict["TAPI_CONTRACT_TYPE_QUOTE"] = '2'
# 策略单类型
# 无
defineDict["TAPI_TACTICS_TYPE_NONE"] =  'N'# 预备单(埋单)
defineDict["TAPI_TACTICS_TYPE_READY"] =  'M'# 自动单
defineDict["TAPI_TACTICS_TYPE_ATUO"] =  'A'# 条件单
defineDict["TAPI_TACTICS_TYPE_CONDITION"] =  'C'
# 订单操作类型
# 报单
defineDict["APIORDER_INSERT"] =  '1'# 改单
defineDict["APIORDER_MODIFY"] =  '2'# 撤单
defineDict["APIORDER_DELETE"] =  '3'# 挂起
defineDict["APIORDER_SUSPEND"] =  '4'# 激活
defineDict["APIORDER_ACTIVATE"] =  '5'# 删除
defineDict["APIORDER_SYSTEM_DELETE"] =  '6'
# 触发条件类型
# 无
defineDict["TAPI_TRIGGER_CONDITION_NONE"] =  'N'# 大于等于
defineDict["TAPI_TRIGGER_CONDITION_GREAT"] =  'G'# 小于等于
defineDict["TAPI_TRIGGER_CONDITION_LITTLE"] =  'L'
# 触发价格类型
# 无
defineDict["TAPI_TRIGGER_PRICE_NONE"] =  'N'# 买价
defineDict["TAPI_TRIGGER_PRICE_BUY"] =  'B'# 卖价
defineDict["TAPI_TRIGGER_PRICE_SELL"] =  'S'# 最新价
defineDict["TAPI_TRIGGER_PRICE_LAST"] =  'L'
# 市价撮合深度
# 任意价
defineDict["TAPI_MARKET_LEVEL_0"] =  0# 1档最优价
defineDict["TAPI_MARKET_LEVEL_1"] =  1# 5档
defineDict["TAPI_MARKET_LEVEL_5"] =  5# 10档
defineDict["TAPI_MARKET_LEVEL_10"] =  10

# 交易状态
# 集合竞价
defineDict["TAPI_TRADE_STATE_BID"] =  '1'# 集合竞价撮合
defineDict["TAPI_TRADE_STATE_MATCH"] =  '2'# 连续交易
defineDict["TAPI_TRADE_STATE_CONTINUOUS"] =  '3'# 交易暂停
defineDict["TAPI_TRADE_STATE_PAUSED"] =  '4'# 闭市
defineDict["TAPI_TRADE_STATE_CLOSE"] =  '5'# 闭市处理时间
defineDict["TAPI_TRADE_STATE_DEALLAST"] =  '6'# 网关未连
defineDict["TAPI_TRADE_STATE_GWDISCONNECT"] =  '0'# 未知状态
defineDict["TAPI_TRADE_STATE_UNKNOWN"] =  'N'# 正初始化
defineDict["TAPI_TRADE_STATE_INITIALIZE"] =  'I'# 准备就绪
defineDict["TAPI_TRADE_STATE_READY"] =  'R'


# 忽略后台推送通知标记
# 推送所有信息
defineDict["TAPI_NOTICE_IGNORE_NONE"] =  0x00000000# 忽略所有推送
defineDict["TAPI_NOTICE_IGNORE_ALL"] =  0xFFFFFFFF# 忽略资金推送:OnRtnFund
defineDict["TAPI_NOTICE_IGNORE_FUND"] =  0x00000001# 忽略委托推送:OnRtnOrder
defineDict["TAPI_NOTICE_IGNORE_ORDER"] =  0x00000002# 忽略成交推送:OnRtnFill
defineDict["TAPI_NOTICE_IGNORE_FILL"] =  0x00000004# 忽略持仓推送:OnRtnPosition
defineDict["TAPI_NOTICE_IGNORE_POSITION"] =  0x00000008# 忽略平仓推送:OnRtnClose
defineDict["TAPI_NOTICE_IGNORE_CLOSE"] =  0x00000010# 忽略持仓盈亏推送:OnRtnPositionProfit
defineDict["TAPI_NOTICE_IGNORE_POSITIONPROFIT"] =  0x00000020

# 委托查询类型
# 返回所有委托
defineDict["TAPI_ORDER_QRY_TYPE_ALL"] =  'A'# 只返回未结束的委托
defineDict["TAPI_ORDER_QRY_TYPE_UNENDED"] =  'U'

# ------------------------------------------------------------------------------------------

# 登录认证信息

# 登录反馈信息

# 账号相关信息查询请求

# 资金账号信息

# 客户下单请求结构

# 委托类型查询应答

# 委托有效类型查询应答


# 委托完整信息











# 报单通知结构

# 报单操作应答结构


# 客户改单请求

# 客户撤单请求结构

# 挂起委托请求结构

# 激活委托请求结构

# 删除委托请求结构

# 委托查询请求结构

# 委托流程查询

# 成交查询请求结构

# 成交信息

# 平仓查询请求结构

# 持仓查询请求结构

# 持仓信息

# 客户持仓盈亏

# 客户持仓盈亏通知

# 平仓信息
# 资金查询请求
# 资金账号资金信息

# 交易品种信息

# 交易合约信息


# 深度行情信息

# 深度行情查询应答

# 交易所时间状态信息查询请求结构

# 交易所时间状态信息

# 交易所时间状态信息通知结构

# 询价通知结构

# 上手通道信息查询请求结构

# 上手通道信息结构
# 客户最终费率查询请求结构
# 客户最终费率信息结构





# 长度为10的字符串
# 长度为20的字符串
# 长度为30的字符串
# 长度为40的字符串
# 长度为50的字符串
# 长度为70的字符串
# 长度为100的字符串
# 长度为300的字符串
# 长度为500的字符串
# 长度为2000的字符串

# Authorization Code


# 单字符定义，可用于定义其他类型



# int 32
# unsigned 32
# int 64
# unsigned 64
# unsigned 16
# unsigned 8
# real 64


# 是否标示
# 是
defineDict["APIYNFLAG_YES"] = 'Y'# 否
defineDict["APIYNFLAG_NO"] = 'N'




# 时间戳类型(格式 yyyy-MM-dd hh:nn:ss.xxx)
# 日期和时间类型(格式 yyyy-MM-dd hh:nn:ss)
# 日期类型(格式 yyyy-MM-dd)
# 时间类型(格式 hh:nn:ss)

# 日志级别
# 无
defineDict["APILOGLEVEL_NONE"] = 'N'# Error
defineDict["APILOGLEVEL_ERROR"] = 'E'# Warning
defineDict["APILOGLEVEL_WARNING"] = 'W'# Debug
defineDict["APILOGLEVEL_DEBUG"] = 'D'


# 品种类型
# 无
defineDict["TAPI_COMMODITY_TYPE_NONE"] =  'N'# 现货
defineDict["TAPI_COMMODITY_TYPE_SPOT"] =  'P'# 期货
defineDict["TAPI_COMMODITY_TYPE_FUTURES"] =  'F'# 期权
defineDict["TAPI_COMMODITY_TYPE_OPTION"] =  'O'# 跨期套利
defineDict["TAPI_COMMODITY_TYPE_SPREAD_MONTH"] =  'S'# 跨品种套利
defineDict["TAPI_COMMODITY_TYPE_SPREAD_COMMODITY"] =  'M'# 看涨垂直套利
defineDict["TAPI_COMMODITY_TYPE_BUL"] =  'U'# 看跌垂直套利
defineDict["TAPI_COMMODITY_TYPE_BER"] =  'E'# 跨式套利
defineDict["TAPI_COMMODITY_TYPE_STD"] =  'D'# 宽跨式套利
defineDict["TAPI_COMMODITY_TYPE_STG"] =  'G'# 备兑组合
defineDict["TAPI_COMMODITY_TYPE_PRT"] =  'R'# 外汇——直接汇率
defineDict["TAPI_COMMODITY_TYPE_DIRECTFOREX"] =  'X'# 外汇——间接汇率
defineDict["TAPI_COMMODITY_TYPE_INDIRECTFOREX"] =  'I'# 外汇——交叉汇率
defineDict["TAPI_COMMODITY_TYPE_CROSSFOREX"] =  'C'# 指数
defineDict["TAPI_COMMODITY_TYPE_INDEX"] =  'Z'# 股票
defineDict["TAPI_COMMODITY_TYPE_STOCK"] =  'T'# 现货延期
defineDict["TAPI_COMMODITY_TYPE_SPOT_TRADINGDEFER"] =  'Y'



# 看涨看跌标示
# 买权
defineDict["TAPI_CALLPUT_FLAG_CALL"] =  'C'# 卖权
defineDict["TAPI_CALLPUT_FLAG_PUT"] =  'P'# 无
defineDict["TAPI_CALLPUT_FLAG_NONE"] =  'N'


# Application信息


# 品种编码结构

# 合约编码结构

# 交易所信息

# 修改密码请求


