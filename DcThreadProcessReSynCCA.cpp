#include <iostream>
#include <iconv.h>
#include "DcThreadProcessReSynCCA.h"
#include "DcManager.h"





using namespace std ;

static long sIndex = 0 ;
DcThreadProcessReSynCCA::DcThreadProcessReSynCCA(CDCCEntry *pDccEntry,DcManager *pDcManager, DcConfig* pConfig)
{
    this->m_pDcManager = pDcManager ;
    this->m_scConfig  = pConfig   ;
    this->m_DccEntry= pDccEntry;
    pCon  =NULL;
    pDML = NULL;
    pDQL = NULL;
    for(int i =0 ; i<9 ; i++)
    {
        ptfDML[i]=NULL ;
    }
    for(int j =0 ; j<1 ; j++)
    {
        ptfDQL[j] = NULL ;
    }
    memset(m_sql,0,2048) ;
    m_info[0]='\0' ;



}
DcThreadProcessReSynCCA::~DcThreadProcessReSynCCA()
{

    for(int i = 0 ; i<9 ; i++)
    {
        if(ptfDML[i] != NULL)
        {
            delete ptfDML[i] ;
            ptfDML[i] =NULL ;
        }
    }

    for(int j = 0 ; j<1 ; j++)
    {
        if(ptfDQL[j] !=NULL)
        {
            delete ptfDQL[j] ;
            ptfDQL[j] =NULL  ;
        }
    }




}
int DcThreadProcessReSynCCA::Init()
{

    try
    {
        char buf[1024]= {0} ;
        char login[128]= {0} ;
        DcConfig* iconfig = DcConfig::GetInstance();
        strcpy(login,iconfig->OCSOracleString) ;

        unsigned int ret = DCRMDBPool::Instance()->Init(login,16);
        if (ret)
        {
            printf("init RMDB failed\n");
            return -1;
        }

        pCon = DCRMDBPool::Instance()->GetConnection();
        if (NULL == pCon)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("get connection failed!")));
            return -2;
        }
        //ptfDQL = new DCRMDBDQL(pCon);
        for(int i =0 ; i<1 ; i++)
        {
            ptfDQL[i] = new DCRMDBDQL(pCon);
        }
        //ptfDML = new DCRMDBDQL(pCon);
        for(int i =0 ; i<9 ; i++)
        {
            ptfDML[i] = new DCRMDBDML(pCon);
        }


        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select session_id ,table_name ,primary_name ,primary_value from re_syn_interface_rel  where session_id = :1 ");
        ret=ptfDQL[0]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[0]->SetCharParam(1);


        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update TIF_SERV_ACTIVATION set syn_deal_state =:1 where serv_id=:2 ");
        ret=ptfDML[1]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[1]->SetCharParam(1);
        ptfDML[1]->SetCharParam(2);

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update tb_bil_user_change_history set syn_deal_state =:1 where oper_id =:2");
        ptfDML[3]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[3]->SetLongParam(1);
        ptfDML[3]->SetLongParam(2);

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update tb_bil_trigger_record set SYN_STATE = :1 where warn_record_id =:2  ");
        ret=ptfDML[4]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[4]->SetCharParam(1);
        ptfDML[4]->SetCharParam(2);

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "insert into  re_syn_interface_rel(session_id ,table_name ,primary_name ,primary_value,deal_date) values(:1,:2,:3,:4,sysdate)");
        ret=ptfDML[5]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[5]->SetCharParam(1);
        ptfDML[5]->SetCharParam(2);
        ptfDML[5]->SetCharParam(3);
        ptfDML[5]->SetCharParam(4);


        memset(buf,0,1024) ;
        strcat(buf,"insert into syn_session_t(session_id,stat,deal_time) values(:1 ,:2,to_date(:3,'yyyymmddhh24miss'))") ;
        ret= ptfDML[6]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[6]->SetCharParam(1);
        ptfDML[6]->SetCharParam(2);
        ptfDML[6]->SetCharParam(3);
        ptfDML[6]->UnBindParam();


        memset(buf,0,1024) ;
        strcat(buf,"insert into work_order_t(session_id,ser_cnt_id,action_id,table_name,filds_name,filds_value,oper_type,session_req,indb_time) values(:1 ,:2 ,:3 ,:4 ,:5 ,:6, :7, :8, to_date(:9,'yyyymmddhh24miss')) ") ;
        ret= ptfDML[7]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[7]->SetCharParam(1);
        ptfDML[7]->SetCharParam(2);
        ptfDML[7]->SetCharParam(3);
        ptfDML[7]->SetCharParam(4);
        ptfDML[7]->SetCharParam(5);
        ptfDML[7]->SetCharParam(6);
        ptfDML[7]->SetCharParam(7);
        ptfDML[7]->SetCharParam(8);
        ptfDML[7]->SetCharParam(9);

        memset(buf,0,1024) ;
        strcat(buf,"insert into hss_ocs_errrecord(session_id,state,insert_time) values(:1,'0',sysdate)") ;
        ret = ptfDML[8]->Prepare(buf) ;
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[8]->SetCharParam(1) ;

    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        return -1 ;
    }

    return 0 ;
}

int DcThreadProcessReSynCCA::routine()
{
    int ret = -1 ;
    CDCCMsg *pMsg = NULL;

    ACELogMsgCallbackCmn *pLogMsgCallback = ACELogMsgCallbackCmn_SIG::instance();
    ACE_LOG_MSG->set_flags (ACE_Log_Msg::MSG_CALLBACK);
    ACE_LOG_MSG->msg_callback (pLogMsgCallback);
    pLogMsgCallback->SetLogLevel(4,0,0,0,4);
    pLogMsgCallback->ChangePMask(LM_DEBUG|LM_ERROR|LM_WARNING|LM_INFO);
    ret=Init();
    if(ret)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("初始化失败"))) ;
        return -1 ;
    }

    ACE_DEBUG((MY_DEBUG ACE_TEXT("*****************消息接口****************"))) ;
    while(1)
    {
		pMsg = m_pDcManager->GetDCCMsgFromBackQueue();
        
        if(pMsg != NULL)
        {
        	DealWithCCA(*pMsg);
            delete pMsg;
            pMsg = NULL;
        }
		else
		{
			sleep(1);
		}			
    }

    return 0 ;
}

CValue* DcThreadProcessReSynCCA::GetMsgAVP(const DCC_BODY &msgBody ,unsigned int code)
{
    if(&msgBody == NULL)
    {
        return NULL;
    }
    for(int i=0; i<msgBody.size(); i++)
    {
        if(msgBody[i]->code() == code)
        {
            return msgBody[i] ;
        }
        if(msgBody[i]->type() == DCC_TYPE_Group)
        {
            const DCC_BODY *pAVPArray= (const DCC_BODY *)(msgBody[i]->toPtr()) ;
            CValue *element = GetMsgAVP(*pAVPArray, code) ;
            if(element != NULL)
            {

                return element;
            }
        }
    }
    return NULL ;
}



int DcThreadProcessReSynCCA::DealWithCCA(CDCCMsg &msg)
{
    unsigned int ret =0 ;
    char seId[64]= {0} ;
    int retCode =-1 ;
    CValue *value = NULL;
    DcThreadProcessReSyn *pThreadReSyn=NULL;
    value = GetMsgAVP(msg.body(), 263);
    if(value != NULL)
    {
        strcpy(seId,value->toString()) ;
    }
    ACE_DEBUG((MY_DEBUG ACE_TEXT("DealWithCCA: session_id=%s"),seId));
    value = GetMsgAVP(msg.body(), 20631);
    if(value != NULL)
    {
        retCode= value->toUInt4() ;
    }

    ACE_DEBUG((MY_DEBUG ACE_TEXT("DealWithCCA: Service-Result-Code=%d"),retCode));
    char flag[2]= {0} ;
    if(retCode==0)
    {
        strcpy(flag,"1") ;
    }
    else
    {
        strcpy(flag,"19") ;
    }
    if(retCode==1006)
    {
    	 pThreadReSyn = new DcThreadProcessReSyn(m_DccEntry,m_scConfig);
       pThreadReSyn->SetMsgFull(1);  
       
    }
    re_syn_interface_rel_t rel_t ;
    memset(&rel_t,0x00,sizeof(rel_t)) ;
    ret=getResynInerfaceRel(rel_t,seId) ;
    if(ret==0 && rel_t.session_id[0] !='\0')
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("get re_syn_interface_rel.table_name=%s,re_syn_interface_rel.primary_value=%s,state=%s"),rel_t.table_name,rel_t.primary_value,flag));
        if(strcmp(rel_t.table_name,"tif_serv_activation")==0)
        {
            setTifServActivationA(rel_t.primary_value,flag) ;
        }
        else if(strcmp(rel_t.table_name,"tb_bil_user_change_history")==0)
        {
            setCdmaStopOpenInterface(rel_t.primary_value,flag) ;
        }
        else if(strcmp(rel_t.table_name,"tb_bil_trigger_record")==0)
        {
            setTbBilTriggerRecord(rel_t.primary_value,flag) ;
        }
    }
    else
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("select session_id ,table_name ,primary_name ,primary_value from re_syn_interface_rel  where session_id = %s is null ,false!!!"),seId));
    }
   if(pThreadReSyn != NULL)
    {
        delete pThreadReSyn ;
       pThreadReSyn = NULL ;
    }
      return 0 ;
}





int DcThreadProcessReSynCCA::setTifServActivationA(char* pKey,char* flag)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[1]->UnBindParam();
        ptfDML[1]->BindParam(1,flag) ;
        ptfDML[1]->BindParam(2,pKey) ;
        ptfDML[1]->Execute();
        pCon->Commit() ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("update TIF_SERV_ACTIVATION set syn_deal_state =%s where serv_id=%s ok."),flag,pKey));
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[1]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}


int DcThreadProcessReSynCCA::setCdmaStopOpenInterface(char* pKey,char* flag)
{

    char buf[1024]= {0} ;
    try
    {
        ptfDML[3]->UnBindParam();
        ptfDML[3]->BindParam(1,atol(flag)) ;
        ptfDML[3]->BindParam(2,atol(pKey)) ;
        ptfDML[3]->Execute();
        pCon->Commit() ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("update tb_bil_user_change_history set syn_deal_state =%s where oper_id =%s ok."),flag,pKey));
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[3]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcThreadProcessReSynCCA::setTbBilTriggerRecord(char* pKey,char* flag)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[4]->UnBindParam();
        ptfDML[4]->BindParam(1,flag) ;
        ptfDML[4]->BindParam(2,pKey) ;
        ptfDML[4]->Execute();
        pCon->Commit() ;

        ACE_DEBUG((MY_DEBUG ACE_TEXT("update tb_bil_trigger_record set SYN_STATE = %s where warn_record_id =%s ok."),flag,pKey));
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[4]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}



int DcThreadProcessReSynCCA::getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDQL[0]->UnBindParam();
        ptfDQL[0]->BindParam(1,sessionId);
        ptfDQL[0]->Query();
        while (ptfDQL[0]->Next())
        {
            ptfDQL[0]->GetValue(1, t.session_id);
            ptfDQL[0]->GetValue(2, t.table_name);
            ptfDQL[0]->GetValue(3, t.primary_name);
            ptfDQL[0]->GetValue(4, t.primary_value);
            break;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[0]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}
/*
int DcThreadProcessReSynCCA::getDataType()
{

    int ret =0 ;
    char buf[1024] = {0};
    DCRMDBDQL*        pDQLL = NULL;

    try
    {
        char iBuf[128]= {0};
        pDQLL = new DCRMDBDQL(pCon);
        strcpy(buf, "select upper(trim(table_name)),upper(trim(files_name)),upper(trim(data_type)) from syn_data_type");

        pDQLL->Prepare(buf);
        pDQLL->UnBindParam();
        pDQLL->Query();
        while (pDQLL->Next())
        {
            char buf1[32]= {0};
            char buf2[64]= {0} ;
            char buf3[2]= {0} ;
            pDQLL->GetValue(1, buf1);
            pDQLL->GetValue(2, buf2);
            pDQLL->GetValue(3, buf3);
            if(buf1[0]=='0' ||buf2[0]=='\0'|| buf3[0]=='\0')
            {
                ACE_DEBUG((MY_ERROR ACE_TEXT("syn_date_type table maybe have unll value !!")));
                return -1 ;
            }

            char key[96]= {0};
            sprintf(key,"%s|%s",buf1,buf2);
            m_dataType.insert(map<string,string>::value_type(key,buf3)) ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDQLL->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        if(pDQLL != NULL)
        {
            delete pDQLL ;
            pDQLL = NULL ;
        }
        return -1 ;
    }
    if(pDQLL != NULL)
    {
        delete pDQLL ;
        pDQLL = NULL ;
    }
    return ret ;
}

*/


