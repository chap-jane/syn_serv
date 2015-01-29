#ifndef ___SYN_STRUCTUNI_H__
#define ___SYN_STRUCTUNI_H__
#include  <list>
#include <string>

typedef struct
{
    char table[30]; //表名
    char flag[2];   //DML标志
    char newandold[4] ;//新旧标识
} tTableAndFlag;

typedef struct
{

    char fieldName[50] ;  //字段名
    char fieldValue[200]; //字段值
} tFNameAndValue;

typedef struct
{
    char t_naof[4] ;
    tTableAndFlag  t_tf ;
    tFNameAndValue t_fnav ;
} xmlRecord;


typedef list<tFNameAndValue> lst_fnav ;


//TB_SYN_TASK_MANAGER
typedef struct
{
    char taskId[20]      ;    //任务标识
    char fileType[2]    ;    //文件类型 I：增量 A：全量 B：批量
    char tableName[30]  ;    //同步的表名
    char taskName[100]  ;    //文件名
    char taskPath[100]  ;    //文件所在路径
    char createTime[21] ;    //文件生成时间
    char dealTime[21]   ;    //文件处理时间
    char taskState[2]   ;    //任务状态：0：未处理 1：处理成功 8：部分成功 9：处理失败
} tSynTaskMgr;

typedef list<tSynTaskMgr> lst_tstm ;

typedef struct
{
    char    logId[8];                       //日志记录标识，序列生成
    char    taskId[8];                      //已处理任务ID
    char    taskName[30];                   //已处理文件名
    char    taskPath[100];                  //已处理文件所在路径
    char    totalRecords[8];                //已处理文件的总记录数
    char    normalRecords[8];       //成功处理记录数
    char    abnormalRecords[8];     //处理失败记录数
    char    invalidrecords[8];      //解析失败的记录数
    char    beginTime[21];          //开始时间
    char    endTime[21];            //结束时间
} tFileSynLog;


typedef struct
{

    char session_id[32];
    char sev_cnt_id[96];
    char cc_request_type[2];
    char cc_request_num[2] ;
    char ation_id[20] ;
    char xml[65536];
	char org_id[32];
    //string xml ;
} t_dcc;

typedef  struct
{
    char session_id[32]  ;
    char ser_cnt_id[96]  ;
    char  action_id[4]   ;
    char table_name[50]  ;
    char filds_name[60]  ;
    char filds_value[60] ;
    char  oper_type[2]    ;
    char  stat[2]        ;
    char  cc_number[2]   ;
} work_order_t ;


typedef struct
{
    char old_acct_id[21] ;
    char user_id[21] ;
    char new_acct_id[21] ;
    char session_id[51] ;
} syn_transfer_t;



////激活接口A表
typedef struct
{
    char serv_id[32] ;
    char pre_serv_state[4];
    char state[4];
    char create_date[21];
} tif_serv_activation_a_t;

////激活接口B表
typedef struct
{
    char serv_id[32] ;
    char pre_serv_state[4];
    char state[4];
} tif_serv_activation_b_t;

///停复机接口表
typedef struct
{
    char oper_id[32];
    char serv_id[32];
    char stop_type[15];
    char create_time[21];
} cdma_stop_open_interface_t;


////上网卡限制流量接口表
typedef struct
{
    char warn_record_id[32];
    char serv_id[32];
    char param_id[64];
    char create_date[21] ;
    char exp_date[21] ;
} tb_bil_trigger_record_t ;

typedef struct
{
    char session_id[64];
    char table_name[80];
    char primary_name[80];
    char primary_value[80];
} re_syn_interface_rel_t ;


typedef struct
{
    char serv_id[32]                   ;  //主产品实例标识
    char agreement_id[32]              ;  //客户协议标识
    char cust_id[32]                   ;  //客户标识
    char product_id[32]                ;  //产品标识
    char billing_cycle_type_id[32]     ;  //帐务周期类别标识
    char product_family_id[32]         ;  //产品家族标识
    char create_date[21]          ;  //建立日期
    char rent_date[21]            ;  //起租日期
    char completed_date[21]       ;  //竣工日期
    char acc_nbr[32]              ;  //号码
    char state[4]                 ;  //状态
    char state_date[21]           ;  //状态时间
} serv_t;

typedef struct
{
    char serv_id[32]          ;      //主产品实例标识
    char agreement_id[32]     ;      //客户协议标识
    char attr_id[32]          ;      //产品属性标识
    char attr_val[32]     ;      //属性取值
    char eff_date[21]     ;      //生效时间
    char exp_date[21]     ;      //失效时间
} serv_attr_t;

typedef struct
{
//  char    serv_id[20]         ;   //主产品实例标识
//  char    agreement_id[20]    ;   //客户协议标识
//  char    address_id[20]      ;   //地址标识
//  char    bureau_id[20]       ;   //分局标识
    char  exchange_id[32]     ;   //局向标识
//  char    stat_region_id[20]  ;   //统计区域标识
//  char    eff_date[20]    ;   //生效时间
//  char    exp_date[20]    ;   //失效时间
} serv_location_t;

typedef struct
{
    char org_id[32] ;
} org_t ;

typedef struct
{
    char serv_id[32];
    char agreement_id[32];
    char owe_business_type_id[10];
    char state[4];
    char eff_date[21];
    char exp_date[21] ;
} serv_state_attr_t ;



#endif
