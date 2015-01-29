#ifndef ___SYN_STRUCTUNI_H__
#define ___SYN_STRUCTUNI_H__
#include  <list>
#include <string>

typedef struct
{
    char table[30]; //����
    char flag[2];   //DML��־
    char newandold[4] ;//�¾ɱ�ʶ
} tTableAndFlag;

typedef struct
{

    char fieldName[50] ;  //�ֶ���
    char fieldValue[200]; //�ֶ�ֵ
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
    char taskId[20]      ;    //�����ʶ
    char fileType[2]    ;    //�ļ����� I������ A��ȫ�� B������
    char tableName[30]  ;    //ͬ���ı���
    char taskName[100]  ;    //�ļ���
    char taskPath[100]  ;    //�ļ�����·��
    char createTime[21] ;    //�ļ�����ʱ��
    char dealTime[21]   ;    //�ļ�����ʱ��
    char taskState[2]   ;    //����״̬��0��δ���� 1������ɹ� 8�����ֳɹ� 9������ʧ��
} tSynTaskMgr;

typedef list<tSynTaskMgr> lst_tstm ;

typedef struct
{
    char    logId[8];                       //��־��¼��ʶ����������
    char    taskId[8];                      //�Ѵ�������ID
    char    taskName[30];                   //�Ѵ����ļ���
    char    taskPath[100];                  //�Ѵ����ļ�����·��
    char    totalRecords[8];                //�Ѵ����ļ����ܼ�¼��
    char    normalRecords[8];       //�ɹ������¼��
    char    abnormalRecords[8];     //����ʧ�ܼ�¼��
    char    invalidrecords[8];      //����ʧ�ܵļ�¼��
    char    beginTime[21];          //��ʼʱ��
    char    endTime[21];            //����ʱ��
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



////����ӿ�A��
typedef struct
{
    char serv_id[32] ;
    char pre_serv_state[4];
    char state[4];
    char create_date[21];
} tif_serv_activation_a_t;

////����ӿ�B��
typedef struct
{
    char serv_id[32] ;
    char pre_serv_state[4];
    char state[4];
} tif_serv_activation_b_t;

///ͣ�����ӿڱ�
typedef struct
{
    char oper_id[32];
    char serv_id[32];
    char stop_type[15];
    char create_time[21];
} cdma_stop_open_interface_t;


////���������������ӿڱ�
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
    char serv_id[32]                   ;  //����Ʒʵ����ʶ
    char agreement_id[32]              ;  //�ͻ�Э���ʶ
    char cust_id[32]                   ;  //�ͻ���ʶ
    char product_id[32]                ;  //��Ʒ��ʶ
    char billing_cycle_type_id[32]     ;  //������������ʶ
    char product_family_id[32]         ;  //��Ʒ�����ʶ
    char create_date[21]          ;  //��������
    char rent_date[21]            ;  //��������
    char completed_date[21]       ;  //��������
    char acc_nbr[32]              ;  //����
    char state[4]                 ;  //״̬
    char state_date[21]           ;  //״̬ʱ��
} serv_t;

typedef struct
{
    char serv_id[32]          ;      //����Ʒʵ����ʶ
    char agreement_id[32]     ;      //�ͻ�Э���ʶ
    char attr_id[32]          ;      //��Ʒ���Ա�ʶ
    char attr_val[32]     ;      //����ȡֵ
    char eff_date[21]     ;      //��Чʱ��
    char exp_date[21]     ;      //ʧЧʱ��
} serv_attr_t;

typedef struct
{
//  char    serv_id[20]         ;   //����Ʒʵ����ʶ
//  char    agreement_id[20]    ;   //�ͻ�Э���ʶ
//  char    address_id[20]      ;   //��ַ��ʶ
//  char    bureau_id[20]       ;   //�־ֱ�ʶ
    char  exchange_id[32]     ;   //�����ʶ
//  char    stat_region_id[20]  ;   //ͳ�������ʶ
//  char    eff_date[20]    ;   //��Чʱ��
//  char    exp_date[20]    ;   //ʧЧʱ��
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
