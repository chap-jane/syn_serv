/*******************************************
*Copyrights  2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcDefineCode.h
*Indentifier：
*
*Description：
*       同步配置类
*Version：
*       V1.0
*Author:
*       yancy
*Finished：
*       2012年2月20日
*History:
*       2012/2/20  V1.0 文件创建
********************************************/
#ifndef _DC_DEFINECODE_H_
#define _DC_DEFINECODE_H_
const unsigned int CODE_SESSION_ID = 263;
const unsigned int CODE_SERVICE_CONTEXT_ID = 461;
const unsigned int CODE_SERV_ID = 5001;
const unsigned int CODE_LATN_ID = 5055;
const unsigned int CODE_SIGN_AVP = 2701;
const unsigned int CODE_REQUEST_RATABLE_QUERY = 5041;
const unsigned int CODE_OWNER_TYPE = 5011;
const unsigned int CODE_OWNER_ID = 5012;
const unsigned int CODE_RATABLE_RESOURCE_ID = 5013;
const unsigned int CODE_RATABLE_CYCLE_ID = 5014;
const unsigned int CODE_OFR_INST_ID = 2806;
const unsigned int CODE_REF_TYPE = 2807;
const unsigned int CODE_REF_OFFSET = 2808;
const unsigned int CODE_LIFE_TYPE = 2809;

const unsigned int CODE_REQUEST_RATABLE_UPDATE = 5010;
const unsigned int CODE_REQUEST_RATABLE_RESERVED = 5056;
const unsigned int CODE_BALANCE = 5015;
const unsigned int CODE_BALANCE_HB = 6015;
const unsigned int CODE_REQUEST_ACCT_ITEM = 5002;
const unsigned int CODE_AOC_BALANCE = 20330;
const unsigned int CODE_ACCT_ITEM_TYPE_ID = 5003;
const unsigned int CODE_REQUESTED_SERVICE_UNIT = 437;
const unsigned int CODE_CC_MONEY = 413;
const unsigned int CODE_CC_TOTAL_OCTETS = 421;
const unsigned int CODE_CC_INPUT_OCTETS = 412;
const unsigned int CODE_CC_OUTPUT_OCTETS = 414;
const unsigned int CODE_CC_SERVICE_SPECIFIC_UNITS = 417;
const unsigned int CODE_UNIT_VALUE = 445;
const unsigned int CODE_CURRENCY_CODE = 425;
const unsigned int CODE_VALUE_DIGITS = 447;
const unsigned int CODE_EXPONENT = 429;

const unsigned int CODE_USED_ACCT_ITEM = 5008;
//const unsigned int CODE_REQUEST_BALANCE_PRESENT = 33;//5016;
//const unsigned int CODE_REQUEST_BALANCE_UPDATE =  34;//5010;
//const unsigned int CODE_RATABLE_UPDATE = 35;//5010;
//const unsigned int CODE_RATABLE_PRESENT = 36;//5016;

const unsigned int CODE_REPORTING_REASON = 872;
const unsigned int CODE_TARIFF_CHANGE_USAGE = 452;
const unsigned int CODE_CC_TIME = 420;
const unsigned int CODE_USED_SERVICE_UNIT = 446;

const unsigned int CODE_BALANCE_TYPE_ID = 5019;
const unsigned int CODE_BALANCE_TYPE_ID_HB = 6019;
const unsigned int CODE_EFF_DATE = 5021;
const unsigned int CODE_EXP_DATE = 5022;
//const unsigned int CODE_SERVICE_SPECIFIC_UNITS = 45;//417; //没变


const unsigned int CODE_RESULT_CODE = 268;
const unsigned int CODE_RESPOND_RATABLE_QUERY =  5042;
const unsigned int CODE_RESPOND_RATABLE_UPDATE = 3803;
const unsigned int CODE_GRANTED_ACCT_BALANCE = 5040;
const unsigned int CODE_AOC_BALANCE_RSP = 3816;
const unsigned int CODE_GRANTED_SERVICE_UNIT = 431;
const unsigned int CODE_TARIFF_TIME_CHANGE = 451;
const unsigned int CODE_TIME_QUOTA_THRESHOLD =868;
const unsigned int CODE_VOLUME_QUOTA_THRESHOLD = 869;

//const unsigned int CODE_ACCT_BALANCE_ID = 2813;
//const unsigned int CODE_UNIT_TYPE_ID = 2814;
const unsigned int CODE_REPONSE_DEBIT_ACCT_ITEM = 5025;
const unsigned int CODE_REQUEST_REFUND_ITEM  = 3815;
const unsigned int CODE_REQUEST_DEBIT_ACCT_ITEM = 5005;

const unsigned int CODE_REQUEST_BALANCE_QUERY_FLAG = 5050;
const unsigned int CODE_RESPOND_BALANCE_QUERY = 5051;
const unsigned int CODE_RESPOND_BALANCE_QUERY_HB = 6051;
const unsigned int CODE_ACCT_BALANCE_ID = 5052;
const unsigned int CODE_UNIT_TYPE_ID = 5053;
const unsigned int CODE_UNIT_TYPE_ID_HB = 6053;
const unsigned int CODE_DEBIT_TYPE = 5054;

const unsigned int CODE_AOC_INFORMATION = 20329;
const unsigned int CODE_AOC_CONFIRMATION = 20340;
const unsigned int CODE_CC_REQUEST_NUMBER = 415;

const unsigned int CODE_ACCT_ID = 40000;
const unsigned int CODE_SERVICE_INFORMATION = 873;
const unsigned int CODE_ORDER_INFORMATION = 82000;
const unsigned int CODE_ORDER_ID = 82001;
const unsigned int CODE_ACCEPT_DATE = 82002;
const unsigned int CODE_COMPELE_DATE = 82003;
const unsigned int CODE_ORDER_TYPE = 82004;
const unsigned int CODE_MSG_ID = 82005;
const unsigned int CODE_SEND_TIMES = 82006;

const unsigned int CODE_DATA_INFORMATION = 80400;
const unsigned int CODE_ACTION_ID = 82007;
const unsigned int CODE_CUSTOM_INFORMATION = 82008;
const unsigned int CODE_SIMPLE_ROW = 80406;
const unsigned int CODE_ACCOUNT_INFORMATION = 82009;
const unsigned int CODE_SERV_INFORMATION = 82010;

//const unsigned int CODE_DATA_INFORMATION = 71;//80400
//const unsigned int CODE_ACTION_ID = 72;//
#define ORDER_DATA              "order_data"
#define SPECIFICATION_DATA      "specification data"
#define INVOLVEMENT_DATA        "Involvement data"
#define ACCOUNT_DATA            "account data"
#define PRD_DATA                "prd data"
#endif
