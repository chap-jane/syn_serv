#include <iostream>
#include <iconv.h>
#include "DcThreadProcess.h"
#include "DcManager.h"

const int MAX_RECV_TIME = 10000000; //recv超时时间

using namespace std ;

static long sIndex = 0 ;
DcThreadProcess::DcThreadProcess(DcManager *pDcManager, CDCCEntry *pDCCEntry, DcConfig* pConfig,unsigned int i):m_errBuf()
{
    this->m_pDcManager = pDcManager ;
    this->m_threadCount=i ;
    this->m_scConfig  = pConfig   ;
	this->m_pDCCEntry = pDCCEntry;
	m_nReserveOCSAndHBData = 0;
	m_nServNeedCombine = 0;


    //m_pXml=DcParseXml::GetInstance();
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
    for(int i =0 ; i<10 ; i++)
    {
        ptfServDML[i]=NULL ;
    }
    //memset(m_sql,0,2048) ;
    memset(&m_t,0x00,sizeof(m_t)) ;
    m_info[0]='\0' ;



}
DcThreadProcess::~DcThreadProcess()
{

    for(int i = 0 ; i<9 ; i++)
    {
        if(ptfDML[i] != NULL)
        {
            delete ptfDML[i] ;
            ptfDML[i] =NULL ;
        }
    }

    for(int i = 0 ; i<10 ; i++)
    {
        if(ptfServDML[i] != NULL)
        {
            delete ptfServDML[i] ;
            ptfServDML[i] =NULL ;
        }
    }
	
    for(int j = 0 ; j<2 ; j++)
    {
        if(ptfDQL[j] !=NULL)
        {
            delete ptfDQL[j] ;
            ptfDQL[j] =NULL  ;
        }
    }




}
int DcThreadProcess::Init()
{

    try
    {
        char buf[1024]= {0} ;
        char login[128]= {0} ;
        DcConfig* iconfig = DcConfig::GetInstance();
        strcpy(login,iconfig->OCSOracleString) ;
		m_nReserveOCSAndHBData = iconfig->m_nReserveOCSAndHBData;
		m_nServNeedCombine = iconfig->m_nServNeedCombine;
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
        for(int i =0 ; i<2 ; i++)
        {
            ptfDQL[i] = new DCRMDBDQL(pCon);
        }
        //ptfDML = new DCRMDBDQL(pCon);
        for(int i =0 ; i<9 ; i++)
        {
            ptfDML[i] = new DCRMDBDML(pCon);
        }
		
        for(int i =0 ; i<10 ; i++)
        {
            ptfServDML[i] = new DCRMDBDML(pCon);
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
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDQL[0] sql[%s] success"),buf));

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select SERV_ID from SERV_ATTR where SERV_ID = :1 and ATTR_ID = :2 and ATTR_VAL = :3 and sysdate between to_date(eff_date,'YYYYMMDDHH24MISS') and to_date(exp_date,'YYYYMMDDHH24MISS')");
        ret=ptfDQL[1]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[1]->SetLongParam(1);
        ptfDQL[1]->SetLongParam(2);
        ptfDQL[1]->SetCharParam(3);
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDQL[1] sql[%s] success"),buf));


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
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[1] sql[%s] success"),buf));

        /*memset(buf,0,1024) ;
        strcat(buf,"delete from SERV where SERV_ID=:1 and PARTITION_ID=:2") ;
        ret= ptfDML[2]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[2]->SetLongParam(1);
        ptfDML[2]->SetIntParam(2);
        //ptfDML[2]->UnBindParam();
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[2] sql[%s] success"),buf));*/

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
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[3] sql[%s] success"),buf));

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
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[4] sql[%s] success"),buf));

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
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[5] sql[%s] success"),buf));


        memset(buf,0,1024) ;
        strcat(buf,"insert into syn_session_t(session_id,stat,deal_time,org_id) values(:1 ,:2,to_date(:3,'yyyymmddhh24miss'),:4)") ;
        ret= ptfDML[6]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[6]->SetCharParam(1);
        ptfDML[6]->SetCharParam(2);
        ptfDML[6]->SetCharParam(3);
		ptfDML[6]->SetCharParam(4);
        ptfDML[6]->UnBindParam();
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[6] sql[%s] success"),buf));


        memset(buf,0,1024) ;
        strcat(buf,"insert into work_order_t(session_id,ser_cnt_id,action_id,table_name,filds_name,filds_value,oper_type,session_req,indb_time) values(:1 ,:2 ,:3 ,:4 ,:5 ,:6, :7, :8, :9) ") ;
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
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[7] sql[%s] success"),buf));

        memset(buf,0,1024) ;
        strcat(buf,"insert into hss_ocs_errrecord(session_id,state,insert_time) values(:1,'0',sysdate)") ;
        ret = ptfDML[8]->Prepare(buf) ;
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[8]->SetCharParam(1) ;
		ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfDML[8] sql[%s] success"),buf));

		for(int j=0; j<10;)
		{
			memset(buf,0,1024) ;
			int nPartitionId = j + 1;
			
			sprintf(buf,"delete from SERV partition(t_partition_p%d) %s",nPartitionId,"where SERV_ID=:1");
			
	        ret= ptfServDML[j]->Prepare(buf);
			
	        if(ret != 0)
	        {
	            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
	            return -1 ;
	        }
			
	        ptfServDML[j]->SetLongParam(1);
			ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare ptfServDML[%d] sql[%s] success"),j,buf));
			j++;
    	}
		

		m_mapServKey.insert(map<string, int>::value_type(string("SERV_ID"),0));
		
		m_mapServParam.insert(map<string, int>::value_type(string("SERV_ID"),0));
		m_mapServParam.insert(map<string, int>::value_type(string("AGREEMENT_ID"),0));
		m_mapServParam.insert(map<string, int>::value_type(string("CUST_ID"),0));
		m_mapServParam.insert(map<string, int>::value_type(string("PRODUCT_ID"),0));
		m_mapServParam.insert(map<string, int>::value_type(string("BILLING_CYCLE_TYPE_ID"),0));
		m_mapServParam.insert(map<string, int>::value_type(string("PRODUCT_FAMILY_ID"),0));
		m_mapServParam.insert(map<string, int>::value_type(string("CREATE_DATE"),1));
		m_mapServParam.insert(map<string, int>::value_type(string("RENT_DATE"),1));
		m_mapServParam.insert(map<string, int>::value_type(string("COMPLETED_DATE"),1));
		m_mapServParam.insert(map<string, int>::value_type(string("ACC_NBR"),1));
		m_mapServParam.insert(map<string, int>::value_type(string("STATE"),1));
		m_mapServParam.insert(map<string, int>::value_type(string("STATE_DATE"),1));
		m_mapServParam.insert(map<string, int>::value_type(string("PARTITION_ID"),0));

		m_mapServAttrKey.insert(map<string, int>::value_type(string("SERV_ID"),0));
		m_mapServAttrKey.insert(map<string, int>::value_type(string("AGREEMENT_ID"),0));
		m_mapServAttrKey.insert(map<string, int>::value_type(string("ATTR_ID"),0));
		m_mapServAttrKey.insert(map<string, int>::value_type(string("ATTR_VAL"),1));
		m_mapServAttrKey.insert(map<string, int>::value_type(string("EFF_DATE"),1));
		m_mapServAttrKey.insert(map<string, int>::value_type(string("EXP_DATE"),1));

		m_mapServLocationKey.insert(map<string, int>::value_type(string("SERV_ID"),0));
		m_mapServLocationKey.insert(map<string, int>::value_type(string("AGREEMENT_ID"),0));

		m_mapAServExKey.insert(map<string, int>::value_type(string("ACC_NBR"),1));
		m_mapAServExKey.insert(map<string, int>::value_type(string("SERV_ID"),0));
		m_mapAServExKey.insert(map<string, int>::value_type(string("AGREEMENT_ID"),0));

    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        return -1 ;
    }

    return 0 ;
}

int DcThreadProcess::routine()
{
    int ret = -1 ;
    int nCode = 0;
    list<t_dcc> *lst_dcc = new list<t_dcc> ;
    CDCCMsg *pMsg = NULL;
    string xmlData("") ;
    char buf[49152]= {0} ;

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
    ret=getDataType();
    if(ret)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("get syn_data_type failds!"))) ;
        return -1 ;
    }
    ACE_DEBUG((MY_DEBUG ACE_TEXT("*****************消息接口****************"))) ;
    char wBuf[48][1024]= {0} ;
    t_dcc t ;
    long lnReceiveOriMsgTime = 0;
    long lnReciveMsgTime  = 0;
    long lnParseMsgTime   = 0;//在DCC中消息解析的时间
    long lnProcessMsgTime = 0;
    long lnHssDateSize    = 0;
    long lnQueueSize     = 0;
    HssMsgDumpFlag = DcConfig::GetInstance()->m_nHssDumpFlag;
    while(1)
    {

/*
		ret = m_pDCCEntry->recv(&pMsg,MAX_RECV_TIME);
        if(ret<0 || NULL ==pMsg)
        {
            ACE_DEBUG((MY_DEBUG_01 ACE_TEXT("DcThreadProcess::svc receive  DCCMsg timeout.\n")));
            continue ;
        }
        lnQueueSize = m_pDCCEntry->getQueueSize();
        lnReciveMsgTime = PublicLib::getTimeUsec();
        lnReceiveOriMsgTime =  pMsg->getMsgBin().GetReceiveTime();
        lnParseMsgTime =lnReciveMsgTime - lnReceiveOriMsgTime;
        lnHssDateSize = pMsg->getHead().len;
        if(HssMsgDumpFlag)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("receive a HSS Msg:")));
            pMsg->dump(buf);
            memset(wBuf,0x00,sizeof(wBuf));
            for(int i=0 ; i<32 ; i++)
            {
                int j=i*2048 ;
                memcpy(wBuf[i] ,buf+j,1024) ;
                if(wBuf[i][0] !='\0')
                {
                    ACE_DEBUG((MY_DEBUG ACE_TEXT("%s"),wBuf[i]));
                }
                else
                {
                    break ;
                }
            }
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Msg end")));
        }
        char t_systime[21]= {0} ;
        PublicLib::GetTime(t_systime,6) ;
        memset(&t,0x00,sizeof(t)) ;
        SDCCHead& head = pMsg->head();
        ///接受到的是CCA
        if(head.sign<128)
        {

            DealWithCCA(*pMsg);

            if(pMsg != NULL)
            {
                delete pMsg;
                pMsg = NULL;
            }
            continue ;
        }
       
		*/

		pMsg = m_pDcManager->GetDCCMsgFromQueue();
		if (pMsg == NULL)
		{
			usleep(5000);
			continue;
		}
		
		ACE_DEBUG((MY_DEBUG ACE_TEXT("start process the dcc msg in the oper thread ")));	
		
        lnQueueSize = m_pDCCEntry->getQueueSize();
        lnReciveMsgTime = PublicLib::getTimeUsec();
        lnReceiveOriMsgTime =  pMsg->getMsgBin().GetReceiveTime();
        lnParseMsgTime =lnReciveMsgTime - lnReceiveOriMsgTime;
        lnHssDateSize = pMsg->getHead().len;

        ///接受到CCR
        vector<CValue*>& pBody = pMsg->getBody();
        for(int i=0 ; i<pBody.size() ; i++)
        {
            if( 263 == pBody[i]->code()) //session_id
            {
                strcpy(t.session_id, pBody[i]->toString()) ;
                continue ;
            }

            if( 461 == pBody[i]->code())//Service-Context-Id
            {
                strcpy(t.sev_cnt_id, pBody[i]->toString()) ;
                continue ;
            }

            if(416 == pBody[i]->code())//CC-Request-Type
            {
                sprintf(t.cc_request_type,"%d",pBody[i]->toInt4()) ;
                continue ;
            }

            if(415 == pBody[i]->code())//CC-Request-number
            {
                sprintf(t.cc_request_num,"%d",pBody[i]->toInt4()) ;
                continue ;
            }
			if(82103 == pBody[i]->code())//Org-Id
            {
                strcpy(t.org_id, pBody[i]->toString()) ;
				ACE_DEBUG((MY_DEBUG ACE_TEXT("Get org_id[%s] from avp 82103"),t.org_id));
                continue ;
            }

            if(873==pBody[i]->code())//Service-Information
            {
                DCC_BODY* pSerInfo = (DCC_BODY*)(pBody[i]->toPtr());
                for(int j=0 ; j<pSerInfo->size() ; j++ )
                {
                    if(80400 != (*pSerInfo)[j]->code())//
                    {
                        continue ;
                    }
                    DCC_BODY* dataInfo = (DCC_BODY*)((*pSerInfo)[j]->toPtr());
                    for(int k=0 ; k<dataInfo->size(); k++)
                    {
                        //action_id
                        if(82007==(*dataInfo)[k]->code())
                        {
                            strcpy(t.ation_id, (*dataInfo)[k]->toString()) ;

                        }

                        //Data-Table-Simple-Row 资料规格域
                        if(80406==(*dataInfo)[k]->code())
                        {
                            const char* xmlBuf=(*dataInfo)[k]->toString();
                            if(xmlBuf[0]!='\0')
                            {
                                strcpy(t.xml,xmlBuf) ;
                                lst_dcc->push_back(t) ;
                            }
                        }

                        if(82008 ==(*dataInfo)[k]->code()) //Customer-Informatio
                        {
                            DCC_BODY* CInfo =(DCC_BODY*)((*dataInfo)[k]->toPtr());
                            for(int n=0 ; n<CInfo->size() ; n++)
                            {
                                if(80406 ==(*CInfo)[n]->code())
                                {

                                    const  char* xmlBuf=(*CInfo)[n]->toString();
                                    if(xmlBuf[0]!='\0')
                                    {
                                        strcpy(t.xml,xmlBuf) ;
                                        lst_dcc->push_back(t) ;
                                    }
                                }
                            }
                        }
                        else if(82009 ==(*dataInfo)[k]->code()) //Account-Information
                        {
                            DCC_BODY* aInfo =(DCC_BODY*)((*dataInfo)[k]->toPtr());
                            for(int n=0 ; n<aInfo->size() ; n++)
                            {
                                if(80406 ==(*aInfo)[n]->code())
                                {

                                    const  char* xmlBuf=(*aInfo)[n]->toString() ;
                                    if(xmlBuf[0] != '\0')
                                    {
                                        strcpy(t.xml,xmlBuf) ;
                                        lst_dcc->push_back(t) ;
                                    }
                                }
                            }
                        }
                        else if(82010 ==(*dataInfo)[k]->code()) //Serv-Information
                        {
                            DCC_BODY* sInfo =(DCC_BODY*)((*dataInfo)[k]->toPtr());
                            for(int n=0 ; n<sInfo->size() ; n++)
                            {
                                if(80406 ==(*sInfo)[n]->code())
                                {

                                    const  char* xmlBuf=(*sInfo)[n]->toString();
                                    if(xmlBuf[0] != '\0')
                                    {
                                        strcpy(t.xml,xmlBuf) ;
                                        lst_dcc->push_back(t) ;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
		ACE_DEBUG((MY_DEBUG ACE_TEXT("process parse dcc msg end ")));
		
        m_ExecHssSQLTime  = 0;
        m_ExecHssSQLTimes = 0;
        if((strcmp(t.cc_request_type,"1" ) ==0) || (strcmp(t.cc_request_type,"2" ) ==0) )
        {
            delete pMsg;
            pMsg = NULL;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("receive a init or update msg and wait for the term msg ...")));

            ACE_DEBUG((MY_DEBUG ACE_TEXT("PER<Receive Msg Time:%Q Parse Msg Time:%Q Process Msg Time:%Q HssDataSize:%Q DCCQueueSize:%Q SQLExecTime:%Q SQLExecTimes:%Q  Session:%s>"),
                       lnReceiveOriMsgTime,lnParseMsgTime, PublicLib::getTimeUsec() - lnReciveMsgTime,lnHssDateSize,lnQueueSize,m_ExecHssSQLTime,m_ExecHssSQLTimes,t.session_id));
            continue ;
        }

        if((strcmp(t.cc_request_type,"3" ) ==0) || (strcmp(t.cc_request_type,"4" ) ==0) )
        {
            if(lst_dcc->size()==0)
            {
                SendResultCCA(*pMsg, 1);
                if(pMsg !=NULL)
                {
                    delete pMsg;
                    pMsg = NULL;
                }
                ACE_DEBUG((MY_DEBUG ACE_TEXT("PER<Receive Msg Time:%Q Parse Msg Time:%Q Process Msg Time:%Q HssDataSize:%Q DCCQueueSize:%Q SQLExecTime:%Q SQLExecTimes:%Q  Session:%s>"),
                           lnReceiveOriMsgTime,lnParseMsgTime, PublicLib::getTimeUsec() - lnReciveMsgTime,lnHssDateSize,lnQueueSize,m_ExecHssSQLTime,m_ExecHssSQLTimes,t.session_id));
                continue ;
            }
            int i =2 ;
			bool bIsOcsUser = true;
			bool bNewHaveServAttr = false;  //10a有403 3的SERV_ATTR或者有10o工单
			for(list<t_dcc>::iterator it_dcc = lst_dcc->begin(); it_dcc != lst_dcc->end() ; it_dcc++)
			{
				if(strcmp(it_dcc->ation_id,"10a") == 0)
				{
					char utf8Buf[24576];
					char gbkBuf[24576];
					memset(utf8Buf,0,sizeof(utf8Buf)) ;
					memset(gbkBuf,0,sizeof(gbkBuf)) ;
					strcpy(utf8Buf,it_dcc->xml) ;
					ret=UTF82GBK(utf8Buf,gbkBuf);
					if(ret == 0)
					{
						ACE_DEBUG((MY_DEBUG ACE_TEXT("utf-8 to gdk fails!")));
					}
					TiXmlDocument*	docDataTableRow   = new TiXmlDocument() ;
					
					docDataTableRow->Parse(gbkBuf,NULL, TIXML_ENCODING_UNKNOWN) ;
					TiXmlElement *tElement =  docDataTableRow->RootElement() ;
					
					if(tElement == NULL)
					{
						ACE_DEBUG((MY_ERROR ACE_TEXT("get FirstChildElement false!!")));
						delete docDataTableRow ;
						docDataTableRow =NULL ;
						//return -1 ;//遇到错误工单应该继续处理下一个工单
						continue;
					}
					
					char table_name[30] = {0};
					for(TiXmlElement* tElement5 =tElement->FirstChildElement() ; tElement5 ; tElement5=tElement5->NextSiblingElement())
					{
						memset(table_name,0x00,sizeof(table_name));
						strcpy(table_name,tElement5->Value());
						ACE_DEBUG((MY_DEBUG ACE_TEXT("get table name[%s] from tElement5->Value[%s]"),table_name,tElement5->Value()));
						if(strcmp(table_name,"SERV_ATTR") == 0)
						{
							bool bRightAttrId = false;
							bool bRightAttrValue = false;
							for(TiXmlElement* tElement0 =tElement5->FirstChildElement() ; tElement0 ; tElement0=tElement0->NextSiblingElement())
							{
								for(TiXmlElement* tElement1 =tElement0->FirstChildElement() ; tElement1 ; tElement1=tElement1->NextSiblingElement())
								{
									memset(&t_fn,0x00,sizeof(t_fn)) ;
									strcpy(t_fn.fieldName,tElement1->Value()) ;    //取得叶子节点的名称
									strcpy(t_fn.fieldValue,tElement1->GetText()) ; //取得叶子节点的值
									if((strcmp(t_fn.fieldName,"ATTR_ID") == 0) && (strcmp(t_fn.fieldValue,"403") == 0))
									{
										ACE_DEBUG((MY_DEBUG ACE_TEXT("get fieldName[%s] from tElement1->Value[%s]"),t_fn.fieldName,tElement1->Value()));
										bRightAttrId = true;
									}
									else if((strcmp(t_fn.fieldName,"ATTR_VAL") == 0) && (strcmp(t_fn.fieldValue,"3") == 0))
									{
										ACE_DEBUG((MY_DEBUG ACE_TEXT("get fieldValue[%s] from tElement1->GetText[%s]"),t_fn.fieldValue,tElement1->GetText()));
										bRightAttrValue = true;
									}
								}
							}

							//消息自带了SERV_ATTR的403,3的属性才认为是OCS用户，才需要处理
							if(bRightAttrId && bRightAttrValue)
							{
								ACE_DEBUG((MY_DEBUG ACE_TEXT("session has action_id=10a with serv_attr 403,3 need deal")));
								bNewHaveServAttr = true; //10a的自带了serv_attr则需要处理
								break;
							}
						}
					}
					
					if(docDataTableRow !=NULL)
					{
						delete docDataTableRow ;
						docDataTableRow =NULL ;
					}
				}
				else if(strcmp(it_dcc->ation_id,"10o") == 0)
				{
					ACE_DEBUG((MY_DEBUG ACE_TEXT("session has action_id=10o need deal")));
					bNewHaveServAttr = true; //有10o工单的认为是OCS用户,直接处理
				}

				if(bNewHaveServAttr)
				{
					break;
				}
			}
			
            for(list<t_dcc>::iterator it = lst_dcc->begin(); it != lst_dcc->end() ; it++)
            {
                i = parseXmlData(it->xml,*it,bIsOcsUser,bNewHaveServAttr);
                if(i != 0)
                {
                	//if(!bIsOcsUser)
                	//{
					//	break;
                	//}
					//else
					//{
                    Rollback();
                    ACE_DEBUG((MY_ERROR ACE_TEXT("session_id[%s] Error Code[%d]"),t.session_id,i));
                    memset(m_errBuf,0,sizeof(m_errBuf)) ;
                    strcpy(m_errBuf,it->xml) ;
                    SendResultCCA(*pMsg, 1);

                    if(pMsg != NULL)
                    {
                        delete pMsg;
                        pMsg = NULL;
                        lst_dcc->clear() ;
                    }
                    break ;
					//}
                }
            }

            //if((i == 0) || (!bIsOcsUser))
			if(i == 0)
            {
                string s=t.session_id ;
				string org_id = t.org_id;
				ACE_DEBUG((MY_DEBUG ACE_TEXT("Begin insert into syn_sesssion_t by session_id[%s], org_id[%s]"),s,org_id));
				if(bNewHaveServAttr || bIsOcsUser)
				{
                	InsertSession(s,org_id);
            	}
				else
				{
					ACE_DEBUG((MY_DEBUG ACE_TEXT("This session_id[%s] is not an ocs session, need not deal"),t.session_id));
				}

				int nResultCode = 0;
				if(!bIsOcsUser) //非OCS返回专门的错误码
				{
					nResultCode = 2;
				}
				
				Commit();
                SendResultCCA(*pMsg, nResultCode);
            }
            else
            {
                insertErrRecord(t.session_id) ;
            }
            ACE_DEBUG((MY_DEBUG ACE_TEXT("PER<Receive Msg Time:%Q Parse Msg Time:%Q Process Msg Time:%Q HssDataSize:%Q DCCQueueSize:%Q SQLExecTime:%Q SQLExecTimes:%Q  Session:%s>"),
                       lnReceiveOriMsgTime,lnParseMsgTime, PublicLib::getTimeUsec() - lnReciveMsgTime,lnHssDateSize,lnQueueSize,m_ExecHssSQLTime,m_ExecHssSQLTimes,t.session_id));
        }

        lst_dcc->clear() ;
        if(pMsg != NULL)
        {
            delete pMsg;
            pMsg = NULL;
        }
    }

    return 0 ;
}

CValue* DcThreadProcess::GetMsgAVP(const DCC_BODY &msgBody ,unsigned int code)
{
	//ACE_DEBUG((MY_DEBUG ACE_TEXT("&msgBody is[%x], msgBody.size[%d], code[%d]!"),&msgBody,msgBody.size(),code));

    if(&msgBody == NULL)
    {
        return NULL;
    }
    for(int i=0; i<msgBody.size(); i++)
    {
    	//ACE_DEBUG((MY_DEBUG ACE_TEXT("msgBody[i]->code[%d], msgBody[i]->type[%d]"),msgBody[i]->code(),msgBody[i]->type()));
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


int DcThreadProcess::SendResultCCA(CDCCMsg &msg, int code)
{
    int ret = 0;
    CDCCMsg retCcaMsg(493) ;

    ACE_DEBUG(( MY_DEBUG ACE_TEXT("deal with CCA ")));
    ret = m_pDCCEntry->prepare(retCcaMsg, m_threadCount);
    if (ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC Prepare Failed")));
        return -1;
    }
    //修改消息头
    SDCCHead& head = retCcaMsg.head();
    head.code =272;
    head.app = 4;
    retCcaMsg.serial(msg.serial());
    char t_session_id[64]= {0} ;

    //修改返回值
    for (int i = 0; i < retCcaMsg.body().size(); i++)
    {

        if(82103 ==  retCcaMsg.body()[i]->code())
        {
            CValue *value = NULL;
            value = GetMsgAVP(msg.body(), 82103);
            if(value)
            {
                retCcaMsg.body()[i]->set(value->toString());
            }
            continue ;
        }

        //Session ID
        if(263 ==  retCcaMsg.body()[i]->code())
        {
            CValue *value = NULL;
            value = GetMsgAVP(msg.body(), 263);
            if(value)
            {
                strcpy(t_session_id,value->toString()) ;
                retCcaMsg.body()[i]->set(value->toString());

            }
            continue ;
        }

        //修改返回值
        if (268 == retCcaMsg.body()[i]->code())
        {
            retCcaMsg.body()[i]->set(2001);
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Result-Code := %d"),code));
            continue ;
        }

        //请求类型
        if(416 == retCcaMsg.body()[i]->code())
        {
            CValue *value = NULL;
            value = GetMsgAVP(msg.body(), 416);
            if(value)
            {
                retCcaMsg.body()[i]->set(value->toUInt4());
            }
            continue ;
        }

        //请求消息序号
        if(415 == retCcaMsg.body()[i]->code())
        {
            CValue *value = NULL;
            value = GetMsgAVP(msg.body(), 415);
            if(value)
            {
                retCcaMsg.body()[i]->set(value->toUInt4());
            }
            continue ;
        }
        //serivce_infomation
        if(873 ==retCcaMsg.body()[i]->code())
        {
            DCC_BODY* pSerInfo = (DCC_BODY*)retCcaMsg.body()[i]->toPtr();
            for (int j = 0; j < pSerInfo->size(); j++)
            {
                //Service-Result-Code
                if(20631 == (*pSerInfo)[j]->code())
                {
                    (*pSerInfo)[j]->set(code);
                    continue ;
                }
                //Para-Field-Result
                if((80407 ==(*pSerInfo)[j]->code()) && (code != 0))
                {
                    if(m_errBuf[0]=='\0')
                    {
                        (*pSerInfo)[j]->set("0");
                    }
                    else
                    {
                        (*pSerInfo)[j]->set(m_errBuf);
                        memset(m_errBuf,0,sizeof(m_errBuf)) ;
                    }
                }
            }
        }
    }
    //sleep(7);
    char buf[49152] = {0};
    ret = m_pDCCEntry->send(retCcaMsg, m_threadCount, buf, 49152, -1, 5);

    if(HssMsgDumpFlag)
    {
        char sendbuf[49152]= {0} ;
        retCcaMsg.dump(sendbuf);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("DcThreadProcess send : %s"),sendbuf));
    }
    if(ret)
    {
        return -1 ;
    }

    return 0 ;
}



char* DcThreadProcess::SetSessionID(const DCC_BODY &msgBody)
{
    CValue *value = NULL;
    const char *pOriginHost = NULL;
    //获取Origin_host;系统当前时间 4字节（time）循环递增
    value = GetMsgAVP(msgBody, 264);
    if (value == NULL)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("获取 Origin_host 失败")));
        return NULL;
    }
    pOriginHost = value->toString();
    time_t tmp = time(NULL);
    char systime[21]= {0} ;
    PublicLib::GetTime(systime,6) ;

    long index = sIndex++ ;
    char Session[64]= {0};
    sprintf(Session,"%s%s%ld",pOriginHost,systime,index);

    ///设置会话ID
    for (int i = 0; i < msgBody.size(); i++)
    {
        if (msgBody[i]->code() == 263) ///session_id
        {
            msgBody[i]->set(Session);
            return Session;
        }
    }
    return NULL;
}




////////设置AVP
int  DcThreadProcess::SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code,char* str)
{
    if(&msgBody == NULL)
    {
        return -1;
    }
    for(int i=0; i<msgBody.size(); i++)
    {
        if(msgBody[i]->code() == code)
        {
            msgBody[i]->set(str) ;
            return 0 ;
        }
        if(msgBody[i]->type() == DCC_TYPE_Group)
        {
            const DCC_BODY *pAVPArray= (const DCC_BODY *)(msgBody[i]->toPtr()) ;
            //CValue *element = SetMsgAVP(*pAVPArray, code) ;
            int ret =SetMsgAVP(*pAVPArray, code, str) ;
            if(ret ==0 )
            {
                return 0 ;
            }

        }
    }
    return -1 ;
}




int  DcThreadProcess::SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code, int i)
{
    if(&msgBody == NULL)
    {
        return -1;
    }
    for(int i=0; i<msgBody.size(); i++)
    {
        if(msgBody[i]->code() == code)
        {
            msgBody[i]->set(i) ;
            return 0 ;
        }
        if(msgBody[i]->type() == DCC_TYPE_Group)
        {
            const DCC_BODY *pAVPArray= (const DCC_BODY *)(msgBody[i]->toPtr()) ;
            //CValue *element = SetMsgAVP(*pAVPArray, code) ;
            int ret =SetMsgAVP(*pAVPArray, code, i) ;
            if(ret ==0 )
            {
                return 0 ;
            }

        }
    }
    return -1 ;
}


int DcThreadProcess::DealWithCCA(CDCCMsg &msg)
{
    unsigned int ret =0 ;
    char seId[64]= {0} ;
    int retCode =-1 ;
    CValue *value = NULL;
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
        strcpy(flag,"9") ;
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

    return 0 ;
}



int DcThreadProcess::parseXmlData(const std::string& xmlDAta,t_dcc &t_info,bool &m_bIsOcsUser, bool &bNewHaveServAttr)
{
    int ret =0 ;
    ACE_DEBUG((MY_DEBUG ACE_TEXT("parse  xml buf strart.......xmlDAta[%s]"),xmlDAta));
    char utf8Buf[24576];
    char gbkBuf[24576];
    memset(utf8Buf,0,sizeof(utf8Buf)) ;
    memset(gbkBuf,0,sizeof(gbkBuf)) ;
    strcpy(utf8Buf,xmlDAta.c_str()) ;
    ret=UTF82GBK(utf8Buf,gbkBuf);
    if(ret == 0)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("utf-8 to gdk fails!")));
    }
    TiXmlDocument*  docDataTableRow   = new TiXmlDocument() ;

    docDataTableRow->Parse(gbkBuf,NULL, TIXML_ENCODING_UNKNOWN) ;
    TiXmlElement *tElement =  docDataTableRow->RootElement() ;

    if(tElement == NULL)
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("get FirstChildElement false!!")));
        delete docDataTableRow ;
        docDataTableRow =NULL ;
        return -1 ;//可以返回错误码
    }

	ACE_DEBUG((MY_DEBUG ACE_TEXT("parse  xml end")));

    char buf[100] = {0} ;
	//ACE_DEBUG((MY_DEBUG ACE_TEXT("tElement->Value[%s]"),tElement->Value()));
    strcpy(buf,tElement->Value()) ;
	//char szTableKey[1024] = {0};  //表主键信息，例如serv_id = 124 and agreement_id = 232131 and acc_nbr = '456321'
	string szTableKey = "";
    char szSqlBuf[2048]= {0} ;

	//bool bNewHaveServAttr = false;
	/*if(strcmp(t_info.ation_id,"10a") == 0)
	{
		char table_name[30] = {0};
		for(TiXmlElement* tElement5 =tElement->FirstChildElement() ; tElement5 ; tElement5=tElement5->NextSiblingElement())
		{
			memset(table_name,0x00,sizeof(table_name));
			strcpy(table_name,tElement5->Value());
			ACE_DEBUG((MY_DEBUG ACE_TEXT("get table name[%s] from tElement5->Value[%s]"),table_name,tElement5->Value()));
			if(strcmp(table_name,"SERV_ATTR") == 0)
			{
				bool bRightAttrId = false;
				bool bRightAttrValue = false;
				for(TiXmlElement* tElement0 =tElement5->FirstChildElement() ; tElement0 ; tElement0=tElement0->NextSiblingElement())
				{
					for(TiXmlElement* tElement1 =tElement0->FirstChildElement() ; tElement1 ; tElement1=tElement1->NextSiblingElement())
					{
						memset(&t_fn,0x00,sizeof(t_fn)) ;
						strcpy(t_fn.fieldName,tElement1->Value()) ;    //取得叶子节点的名称
						strcpy(t_fn.fieldValue,tElement1->GetText()) ; //取得叶子节点的值
						if((strcmp(t_fn.fieldName,"ATTR_ID") == 0) && (strcmp(t_fn.fieldValue,"403") == 0))
						{
							ACE_DEBUG((MY_DEBUG ACE_TEXT("get fieldName[%s] from tElement1->Value[%s]"),t_fn.fieldName,tElement1->Value()));
							bRightAttrId = true;
						}
						else if((strcmp(t_fn.fieldName,"ATTR_VAL") == 0) && (strcmp(t_fn.fieldValue,"3") == 0))
						{
							ACE_DEBUG((MY_DEBUG ACE_TEXT("get fieldValue[%s] from tElement1->GetText[%s]"),t_fn.fieldValue,tElement1->GetText()));
							bRightAttrValue = true;
						}
					}
				}

				//消息自带了SERV_ATTR的403,3的属性才认为是OCS用户，才需要处理
				if(bRightAttrId && bRightAttrValue)
				{
					bNewHaveServAttr = true; //10a的自带了serv_attr则需要处理
					break;
				}
			}
		}
	}*/
	
    for(TiXmlElement* tElement2 =tElement->FirstChildElement() ; tElement2 ; tElement2=tElement2->NextSiblingElement())
    {
        memset(&t_st,0x00,sizeof(t_st)) ;
        strcpy(t_st.table,tElement2->Value()) ;//取得表名
        TiXmlAttribute* AttrFlag = tElement2->FirstAttribute() ;
        strcpy(t_st.flag,AttrFlag->Value()) ; //取得flag标识
        bool bHaveServOld = false;
        for(TiXmlElement* tElement3 =tElement2->FirstChildElement() ; tElement3 ; tElement3=tElement3->NextSiblingElement())
        {
			//bool bIsOcsUser = true;
			long lnServId = -1;
            char bufNewOrOld[4]= {0} ;
            strcpy(t_st.newandold,tElement3->Value()) ;//新旧标识

			szTableKey = "";
			l_list.clear();
            for(TiXmlElement* tElement4 =tElement3->FirstChildElement() ; tElement4 ; tElement4=tElement4->NextSiblingElement())
            {
                memset(&t_fn,0x00,sizeof(t_fn)) ;
                strcpy(t_fn.fieldName,tElement4->Value()) ;    //取得叶子节点的名称
                strcpy(t_fn.fieldValue,tElement4->GetText()) ; //取得叶子节点的值
                if(t_fn.fieldValue[0] !='\0')
                {
					//ACE_DEBUG((MY_DEBUG ACE_TEXT("get fieldName[%s] from fieldValue[%s]"),t_fn.fieldName,t_fn.fieldValue));
                    l_list.push_back(t_fn) ;
                }
				
				if(strcmp("NEW",t_st.newandold)==0) //add
				{
					getTablePrimaryKeyString(t_st.table,t_fn.fieldName,t_fn.fieldValue,szTableKey);
            	}

				//取出serv_id，查询SERV_ATTR，判断是否OCS用户
				if(strcmp(t_fn.fieldName, "SERV_ID") == 0)
				{
					lnServId = atol(t_fn.fieldValue);
				}
            }
			
			//if((strcmp(t_info.ation_id,"10o") == 0) || (strcmp(t_info.ation_id,"10a") == 0) 
			//	|| (strcmp(t_info.ation_id,"10d") == 0) || (strcmp(t_info.ation_id,"10s") == 0) || (lnServId == -1))
			//不存后付费开关ReserveOCSAndHBData为开，才需要判断是否预后
			if(m_nReserveOCSAndHBData == 1)
			{
				//if((strcmp(t_info.ation_id,"10o") == 0) || (lnServId == -1)
				//	|| (strcmp("ACCT",t_st.table)==0) || (strcmp("CUST",t_st.table)==0))
				if(bNewHaveServAttr || (lnServId == -1)
					|| (strcmp("ACCT",t_st.table)==0) || (strcmp("CUST",t_st.table)==0))
				{
					//没有SERV_ID、预后互转10o、ACCT和CUST表不判断是否OCS直接处理
					ACE_DEBUG((MY_DEBUG ACE_TEXT("action_id is 10o, or action_id is 10a with serv_attr 403,3, or table_name is ACCT or CUST or message do not have serv_id, need deal")));
				}
				else if(lnServId != -1)
				{
					/*if(!bNewHaveServAttr) 
					{
						//无需判断返回值。查询异常仍为true，按OCS用户处理
						ret = getServAttr(lnServId,403,"3",m_bIsOcsUser);
						/*if(!bIsOcsUser)
						{
							//return 9999;
							m_bIsOcsUser = false;
							continue;
						}
					}
					else
					{
						//10a的消息中自带SERV_ATTR的需要直接处理，不判断
						ACE_DEBUG((MY_DEBUG ACE_TEXT("action_id is 10a and there is serv_attr in message, it's OCS user, need deal")));
					}*/
					ret = getServAttr(lnServId,403,"3",m_bIsOcsUser);
					if(!m_bIsOcsUser)
					{
						break;
					}
	        	}
			}
			
			if((strcmp("NEW",t_st.newandold)==0) && (strcmp("SERV",t_st.table)==0))
			{
				tFNameAndValue t_fn;
				strcpy(t_fn.fieldName,"PARTITION_ID");
				strcpy(t_fn.fieldValue,"-1");
				l_list.push_back(t_fn);
        	}
            //////组装SQL语句
            ACE_DEBUG((MY_DEBUG ACE_TEXT("start package sql")));

			//控制是否合并成update的开关为开才需要检查是否需要合并
			if((strcmp(t_st.flag,"3") == 0) && (strcmp(t_st.table, "SERV") == 0) && (strcmp("OLD",t_st.newandold) == 0))
			{
				ACE_DEBUG((MY_DEBUG ACE_TEXT("this is delete from SERV, delete + insert -> update")));
				bHaveServOld = true;
				//delete from SERV的直接写work_order_t
			}
			else   //insert into SERV的，改成update
			{
				//bool bIsDeleteServ = false;
				string strServId = "";
				string strPartitionId = "";
				bool bIsUpdateServ = false;
				string strSql = "";
	            ret = packageSql(t_st,l_list,strPartitionId,strServId,strSql,bHaveServOld) ;
				if(ret == 1)
				{
					bIsUpdateServ= true;
				}
	            if(strSql.size() != 0)
	                ACE_DEBUG((MY_DEBUG ACE_TEXT("packageSql-sql :%s"),strSql.c_str()));

	            int iExe =0 ;
	            /////////执行SQL语句

				ACE_DEBUG((MY_DEBUG ACE_TEXT("start exectute sql:%s"),strSql.c_str()));	

	            long lnTemp = PublicLib::getTimeUsec();
	            //if((iExe=executeSql()) != 0)
	            iExe=executeSql(strPartitionId,strServId,bIsUpdateServ,strSql);
				if(iExe < 0)
	            {
	                l_list.clear() ;
	                if(docDataTableRow !=NULL)
	                {
	                    delete docDataTableRow ;
	                    docDataTableRow =NULL ;
	                }
	                return iExe ;
	            }
				else if((iExe == 1) && (szTableKey.size() != 0))  //主键冲突的先delete再insert
				{
					ACE_DEBUG((MY_DEBUG ACE_TEXT("begin to delete then insert, table name %s, key info %s"), t_st.table, szTableKey.c_str()));
					memset(szSqlBuf,0x00,sizeof(szSqlBuf)) ;
					if(strcmp(t_st.table, "SERV") == 0)
					{
						char szServId[32] = {0};
						char partition_id[2] = {0};
						int  nPartitionID = -1;
						
						strncpy(szServId,szTableKey.c_str()+10, strlen(szTableKey.c_str()) - 10);
						partition_id[0] = szServId[strlen(szServId)-1];
						partition_id[1] = '\0';
						nPartitionID = atoi(partition_id) + 1;
						
						sprintf(szSqlBuf,"DELETE FROM %s partition(t_partition_p%d) WHERE %s",t_st.table,nPartitionID,szTableKey.c_str());
					}
					else
					{
						sprintf(szSqlBuf,"DELETE FROM %s WHERE %s",t_st.table,szTableKey.c_str());
					}
					iExe = deleteRecordByKey(szSqlBuf);
					if(iExe)
					{
						l_list.clear() ;
						if(docDataTableRow !=NULL)
						{
							delete docDataTableRow ;
							docDataTableRow =NULL ;
						}
						return iExe ;
					}
					else
					{
						ACE_DEBUG((MY_DEBUG ACE_TEXT("start package sql")));
						packageSql(t_st,l_list,strPartitionId,strSql,strServId) ;
						if(strSql.size() != 0)
							ACE_DEBUG((MY_DEBUG ACE_TEXT("packageSql-sql :%s"),strSql.c_str()));
						
						int iExe =0 ;
						/////////执行SQL语句
						
						ACE_DEBUG((MY_DEBUG ACE_TEXT("start exectute sql:%s"),strSql.c_str()));	
						
						long lnTemp = PublicLib::getTimeUsec();
						iExe = executeSql(strPartitionId,strServId,false,strSql);
						if(iExe != 0)
						{
							l_list.clear() ;
							if(docDataTableRow !=NULL)
							{
								delete docDataTableRow ;
								docDataTableRow =NULL ;
							}
							return iExe ;
						}
						else
						{
							ACE_DEBUG((MY_DEBUG ACE_TEXT("end delete then insert successfully")));
						}
					}
				}
				m_ExecHssSQLTimes ++;
				m_ExecHssSQLTime += PublicLib::getTimeUsec() - lnTemp;
        	}
            //插入工单表
            ACE_DEBUG((MY_DEBUG ACE_TEXT("start exectute end")));
            memset(&work_t,0x00,sizeof(work_t)) ;
            strcpy(work_t.session_id ,t_info.session_id);
            strcpy(work_t.ser_cnt_id ,t_info.sev_cnt_id);
            strcpy(work_t.action_id  ,t_info.ation_id);
            strcpy(work_t.table_name ,t_st.table);
            if(strcmp("NEW",t_st.newandold)==0)
            {
                strcpy(work_t.oper_type  ,"1");
            }
            else if(strcmp("OLD",t_st.newandold)==0)
            {
                strcpy(work_t.oper_type  ,"2");
            }

            strcpy(work_t.stat       ,"0");
            strcpy(work_t.cc_number  ,t_info.cc_request_num);
            char t_name[60]= {0};
            char t_value[60]= {0};
            for(list<tFNameAndValue>::iterator iter = l_list.begin(); iter != l_list.end() ; iter++)
            {
                char key[96]= {0} ;
                sprintf(key,"%s|%s",t_st.table,iter->fieldName) ;
                map<string, string>::iterator it =m_dataType.find(key) ;
                if(it !=m_dataType.end())
                {
                    //找到主键和值
                    if((strcmp("4",it->second.c_str())==0) || (strcmp("5",it->second.c_str())==0))
                    {
                    	ACE_DEBUG((MY_DEBUG ACE_TEXT("find key field[%s] value[%s]"),iter->fieldName,iter->fieldValue));
                        strcat(t_name,iter->fieldName)   ;
                        strcat(t_name,",")   ;
                        strcat(t_value,iter->fieldValue) ;
                        strcat(t_value,",") ;
                    }
                }
            }
            if(t_name[0] !='\0')
            {
                memcpy(work_t.filds_name,t_name,strlen(t_name)-1);
                memcpy(work_t.filds_value,t_value,strlen(t_value)-1);
            }
            //插入工单表
            ACE_DEBUG((MY_DEBUG ACE_TEXT("start insert into work order table")));
            InsertWorkOrder(work_t) ;
            memset(&work_t,0x00,sizeof(work_t)) ;
            l_list.clear() ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Execute successful........... ")));
        }
    }

    if(docDataTableRow !=NULL)
    {
        delete docDataTableRow ;
        docDataTableRow =NULL ;
    }
    return 0 ;
}


int DcThreadProcess::executeSql(string strPartitionId,string strServId, bool bIsUpdateServ,string strSql)
{

    unsigned int ret =0 ;
    DCRMDBDML*  tpDML =NULL;
    try
    {
        tpDML = new DCRMDBDML(pCon);
        //if(m_sql != NULL)
        if(strSql.size() != 0)
            ret= tpDML->Prepare(strSql.c_str());
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),strSql.c_str()));
            if(tpDML !=NULL)
            {
                delete tpDML ;
                tpDML =NULL ;
            }
            return 2002 ;
        }
		
		if(bIsUpdateServ)
		{
            ACE_DEBUG((MY_DEBUG ACE_TEXT("execute-sql: %s"),strSql.c_str()));
            tpDML->Execute();
            if(tpDML != NULL)
            {
                delete tpDML ;
                tpDML = NULL ;
            }
            return 0 ;
		}
		
        if(strcmp("OLD",t_st.newandold)==0)
        {
        	if(strServId.size() > 0)
        	{
				ret = deleteServByServId(atoi(strPartitionId.c_str()),atol(strServId.c_str()));
				if(tpDML !=NULL)
				{
					delete tpDML ;
					tpDML =NULL ;
				}
				return ret;
        	}
			
            ACE_DEBUG((MY_DEBUG ACE_TEXT("execute-sql: %s"),strSql.c_str()));
            tpDML->Execute();
            //pCon->Commit() ;
            if(tpDML !=NULL)
            {
                delete tpDML ;
                tpDML =NULL ;
            }
            return 0 ;
        }
        int i= 0 ;
        for(list<tFNameAndValue>::iterator iter = l_list.begin(); iter != l_list.end() ; iter++)
        {
            char key[96]= {0} ;
            sprintf(key,"%s|%s",t_st.table,iter->fieldName) ;
            //DcParseXml* m_data =DcParseXml::GetInstance();
            map<string, string>::iterator miter =m_dataType.find(key) ;
            if(miter != m_dataType.end())
            {
                i++ ;
                if((strcmp("1",miter->second.c_str())==0)||(strcmp("4",miter->second.c_str())==0))
                {
                    tpDML->SetCharParam(i);
                }
                if((strcmp("2",miter->second.c_str())==0)||(strcmp("5",miter->second.c_str())==0))
                {
                    //pDML->SetLongParam(i);
                    tpDML->SetCharParam(i);
                }
            }
            else
            {

                ACE_DEBUG((MY_DEBUG ACE_TEXT("%s undefine in syn_data_type table"),key));
                if(tpDML !=NULL)
                {
                    delete tpDML ;
                    tpDML =NULL ;
                }
                return 2004 ;
            }
        }

        tpDML->UnBindParam();
        int j=0 ;
        char sqlInfo[1024]= {0} ;
        strcat(sqlInfo,"values(") ;
        int k =l_list.size() ;
		char partition_id[2] = {0};
		char szServId[32] = {0};
        for(list<tFNameAndValue>::iterator iter = l_list.begin(); iter != l_list.end() ; iter++)
        {
        	if(strcmp(iter->fieldName,"SERV_ID") == 0)
        	{
        		strcpy(szServId,iter->fieldValue);
				partition_id[0] = szServId[strlen(szServId)-1];
				partition_id[1] = '\0';
        	}
            j++ ;
			if(strcmp(iter->fieldName,"PARTITION_ID") == 0)
			{
				if(strlen(partition_id) == 0)
				{
					ACE_DEBUG((MY_DEBUG ACE_TEXT("partition_id is null!")));
					return -1;
				}
				tpDML->BindParam(j, partition_id);
				strcat(sqlInfo,partition_id) ;
        	}
			else
			{
				tpDML->BindParam(j, iter->fieldValue);
				strcat(sqlInfo,iter->fieldValue) ;
			}
            if(j<k)
            {
                strcat(sqlInfo,",") ;
            }
        }
        strcat(sqlInfo,")") ;
        strcat(m_info,sqlInfo) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execute-sql: %s"),m_info));
        m_info[0]='\0' ;

        tpDML->Execute();
        //pCon->Commit();
    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        if(tpDML !=NULL)
        {
            delete tpDML ;
            tpDML =NULL ;
        }

		//判断是否主键冲突错误，是的话需要delete再insert
		if(checkInsertError(e.GetSqlErrorInfo()))
		{
			return 1;
		}
		else
		{
        	return -1 ;
		}
    }

    if(tpDML !=NULL)
    {
        delete tpDML ;
        tpDML =NULL ;
    }
    return 0 ;
}

void DcThreadProcess::Commit()
{
    pCon->Commit();
}

void DcThreadProcess::Rollback()
{
    pCon->Rollback(0);
}

int DcThreadProcess::InsertSession(string& t, string& org_id)
{
    try
    {

        char szDealTime[21]= {0} ;
        PublicLib::GetTime(szDealTime,1) ;
        unsigned int  ret =0 ;
        char buf[1024] ;
        if(t[0]=='\0')
        {
            return -1 ;
        }
        ptfDML[6]->BindParam(1,t.c_str());
        ptfDML[6]->BindParam(2,"0");
        ptfDML[6]->BindParam(3,szDealTime);
		ptfDML[6]->BindParam(4,org_id.c_str());
        ACE_DEBUG((MY_DEBUG ACE_TEXT("insert into syn_session_t values(%s,%s,to_date(%s,'yyyymmddhh24miss'),%s)"),t.c_str(),"0",szDealTime,org_id.c_str()));
        ptfDML[6]->Execute();
        //pCon->Commit();
    }
    catch(DCRMDBException& e)
    {

        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));

    }

    return 0 ;
}

int DcThreadProcess::code_convert(const char *fromcode, const char* tocode, char* inbuf, char* poutbuf)
{
    int ret=0;
    iconv_t cd ;

    if((cd = iconv_open(tocode, fromcode)) == (iconv_t)-1)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("convert error[%d][%s]\n"),errno,strerror(errno)));
        return 0;
    }


    unsigned long inbytesleft = strlen(inbuf);
    unsigned long outbytesleft = 24576;

    ret = iconv(cd, &inbuf, &inbytesleft, &poutbuf, &outbytesleft);

    iconv_close(cd);

    return 24576-outbytesleft;
}

int DcThreadProcess::UTF82GBK(char * buf, char *outbuf)
{
    return code_convert("utf8", "gb18030", buf, outbuf);
};

int DcThreadProcess::packageSql(tTableAndFlag t,lst_fnav l,string &strPartitioinId,string &strServId,string &strSql,bool bHaveServOld)
{
    char sqlBuf[2048]= {0} ;
	
    if(strcmp("NEW",t.newandold)==0) //add
    {
        char fields[500]= {0} ;
        char bindParam[200] = {0};
        int i =1 ;
		if((m_nServNeedCombine == 1) && bHaveServOld && (strcmp(t.flag,"3") == 0) && (strcmp(t.table, "SERV") == 0) && (strcmp("NEW",t.newandold) == 0))
		{
			char szServId[50] = {0};
	        for(list<tFNameAndValue>::iterator iter = l.begin(); iter != l.end() ; iter++)
	        {
	        	if(strcmp(iter->fieldName,"PARTITION_ID") == 0)
	        	{
	        		continue;
	        	}
	        	map<string, int>::iterator itParam = m_mapServParam.find(string(iter->fieldName));
				if(itParam == m_mapServParam.end())
				{
					ACE_DEBUG((MY_ERROR ACE_TEXT("fieldName[%s] is not a parameter of table SERV!"),iter->fieldName));
					return -1;
				}
	        	if(strcmp(iter->fieldName,"SERV_ID") == 0)
	        	{
	        		strcpy(szServId,iter->fieldValue);
					continue;
	        	}
				
	        	if(fields[0] == '\0')
	        	{
	        		strcpy(fields,iter->fieldName);
					if(itParam->second == 0)
					{
						sprintf(fields,"%s=%s,",fields,iter->fieldValue);
					}
					else
					{
						sprintf(fields,"%s='%s',",fields,iter->fieldValue);
					}
	        	}
				else
				{
					if(itParam->second == 0)
					{
						sprintf(fields,"%s%s=%s,",fields,iter->fieldName,iter->fieldValue);
					}
					else
					{
						sprintf(fields,"%s%s='%s',",fields,iter->fieldName,iter->fieldValue);
					}
				}
	        }

			if(fields[0] == '\0')
			{
				ACE_DEBUG((MY_ERROR ACE_TEXT("Can not find any parameter from XML!")));
				return -1;
			}
			else
			{
				fields[strlen(fields)-1] = '\0';
			}

			if(szServId[0] == '\0')
			{
				ACE_DEBUG((MY_ERROR ACE_TEXT("Can not find SERV_ID from XML!")));
				return -1;
			}
			
			char partition_id[2] = {0};
			partition_id[0] = szServId[strlen(szServId)-1];
			partition_id[1] = '\0';
			int nPartitionId = atoi(partition_id)+1;

			sprintf(sqlBuf,"update %s partition(t_partition_p%d) set %s where SERV_ID=%s",t.table,nPartitionId,fields,szServId);

	        //memset(m_sql,0,2048) ;
	        //strcpy(m_sql,sqlBuf) ;
	        strSql = string(sqlBuf);
			//ACE_DEBUG((MY_DEBUG ACE_TEXT("update serv sql is[%s]!"),m_sql));
			return 1;
		}
		else
		{
			char szServId[32] = {0};
			char partition_id[2] = {0};
			int nPartitionId = 0;
	        for(list<tFNameAndValue>::iterator iter = l.begin(); iter != l.end() ; iter++)
	        {
	        	if((strcmp(t.table, "SERV") == 0) && (strcmp(iter->fieldName,"SERV_ID") == 0))
	        	{
	        		strcpy(szServId,iter->fieldValue);
					partition_id[0] = szServId[strlen(szServId)-1];
					partition_id[1] = '\0';
					nPartitionId = atoi(partition_id)+1;
	        	}
	            strcat(fields,iter->fieldName);
	            strcat(fields,",");

	            char c[5] = {0} ;
	            sprintf(c,":%d , ",i++) ;
	            strcat(bindParam,c);
	        }

	        string str1 = fields ;
	        string str2 = bindParam ;
	        strcat(sqlBuf,"insert into ") ;
	        strcat(sqlBuf,t.table) ;
	        strcat(sqlBuf,"(") ;

	        unsigned int n =str1.find_last_of(",") ;
	        if(n == string::npos )
	        {
	            return NULL ;
	        }
	        string tmp1= str1.substr(0,n) ;
	        strcat(sqlBuf,tmp1.c_str()) ;

	        strcat(sqlBuf,") ") ;
			memset(m_info,0x0,2048);
	        strcat(m_info,sqlBuf) ;
	        strcat(sqlBuf,"values(") ;
	        unsigned int j =str2.find_last_of(",") ;
	        if(j == string::npos )
	        {
	            return NULL ;
	        }
	        string tmp2= str2.substr(0,j-1) ;


	        strcat(sqlBuf,tmp2.c_str()) ;
	        strcat(sqlBuf, " )") ;

	        //cout<<"ADD-SQL:"<<sqlBuf<<endl ;
	        //memset(m_sql,0,2048) ;
	        //strcpy(m_sql,sqlBuf) ;
	        strSql = string(sqlBuf);
			return 0 ;
    	}
    }

    if(strcmp("OLD",t.newandold)==0)//delete
    {
        //delete table
        char dBuf[200]= {0} ;
		char szServId[50] = {0};
		char partition_id[2] = {0};
		int nPartitionId = 0;
        for(list<tFNameAndValue>::iterator iter = l.begin(); iter != l.end() ; iter++)
        {
        	if((strcmp(t.table, "SERV") == 0) && (strcmp(iter->fieldName,"SERV_ID") == 0))
        	{
        		strcpy(szServId,iter->fieldValue);
				partition_id[0] = szServId[strlen(szServId)-1];
				partition_id[1] = '\0';
				nPartitionId = atoi(partition_id) + 1;
        	}

            char key[96]= {0} ;
            sprintf(key,"%s|%s",t.table,iter->fieldName) ;
            //DcParseXml* m_data =DcParseXml::GetInstance();
            map<string, string>::iterator it = m_dataType.find(key) ;
            if(it !=m_dataType.end())
            {
                //根据主键删除记录
                if((strcmp("4",it->second.c_str())==0) || (strcmp("5",it->second.c_str())==0))
                {
                    strcat(dBuf,iter->fieldName);
                    strcat(dBuf,"=");
                    strcat(dBuf,iter->fieldValue) ;
                    strcat(dBuf," and ") ;
                }
            }
        }

        string st = dBuf ;
        unsigned int ipos =st.find_last_of("and") ;
        if(ipos == string::npos )
        {
            return NULL ;
        }
        string stbuf =st.substr(0,ipos-3) ;

        strcat(sqlBuf,"delete from ") ;
        strcat(sqlBuf,t.table) ;
		if(strcmp(t.table, "SERV") == 0)
		{
			sprintf(sqlBuf,"%s partition(t_partition_p%d)",sqlBuf,nPartitionId);
		}
        strcat(sqlBuf," where ");
        strcat(sqlBuf,stbuf.c_str()) ;

        //cout<<"DEL-SQL:"<<sqlBuf<<endl ;
        //memset(m_sql,0,2048) ;
        //strcpy(m_sql,sqlBuf) ;
        strSql = string(sqlBuf);
		
        string stDel = sqlBuf ;
        unsigned int inpos =stDel.find_last_of("=") ; //找到最后一个等号位置，值换成:1判断是否是提前prepare的SQL
        if(inpos != string::npos )
        {
			string strbufdel = stDel.substr(0,inpos+1);
			strbufdel = strbufdel + string(":1"); //拼接成=:1
			char buf[2048]= {0} ;
			//ptfDML[2]->GetSqlString(buf, 1024);

			sprintf(buf,"delete from SERV partition(t_partition_p%d) where SERV_ID=:1",nPartitionId);
			ACE_DEBUG((MY_DEBUG ACE_TEXT("this delete sql is[%s], ptfServDML[%d] sql is[%s]"), strbufdel.c_str(),nPartitionId,buf));
			
			//是这个SQL，则执行之前绑定值delete from SERV where SERV_ID=:1		
			if(strcmp(strbufdel.c_str(),buf) == 0)
			{
				ACE_DEBUG((MY_DEBUG ACE_TEXT("sql[%s] has been prepared"),strbufdel.c_str()));
				strServId = string(szServId); //如果是提前prepare的SQL，则需要用绑定值来删除
				strPartitioinId = string(partition_id);
			}
        }
		
        return 0 ;
    }

    return 0 ;
}


int DcThreadProcess::InsertWorkOrder(work_order_t &t)
{

    try
    {
        if(strcmp(t.table_name ,"ACCT")==0 && strcmp(t.table_name ,"CUST")==0)
        {
            return 0 ;
        }

        //pDML = new DCRMDBDML(pCon);
        char szStartTime[21]= {0} ;
        char szDealTime[21]= {0} ;
        PublicLib::GetTime(szStartTime,1) ;
        PublicLib::GetTime(szDealTime,1) ;
        unsigned int    ret =0 ;
        char sql[1024] ;

        ptfDML[7]->UnBindParam();
        ptfDML[7]->BindParam(1,t.session_id);
        ptfDML[7]->BindParam(2,t.ser_cnt_id);
        ptfDML[7]->BindParam(3,t.action_id);
        ptfDML[7]->BindParam(4,t.table_name);
        ptfDML[7]->BindParam(5,t.filds_name);
        ptfDML[7]->BindParam(6,t.filds_value);
        ptfDML[7]->BindParam(7,t.oper_type);
        ptfDML[7]->BindParam(8,t.cc_number);
        ptfDML[7]->BindParam(9,szStartTime);

        ptfDML[7]->Execute();
        //pCon->Commit();

    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        return -1 ;
    }


    return 0 ;

}


int DcThreadProcess::insertErrRecord(char * sessId)
{

    try
    {
        ptfDML[8]->UnBindParam();
        ptfDML[8]->BindParam(1,sessId);
        ptfDML[8]->Execute();
        Commit();
    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        return -1 ;
    }

    return 0 ;
}


int DcThreadProcess::setTifServActivationA(char* pKey,char* flag)
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


int DcThreadProcess::setCdmaStopOpenInterface(char* pKey,char* flag)
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

int DcThreadProcess::setTbBilTriggerRecord(char* pKey,char* flag)
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

int DcThreadProcess::setResynInerfaceRel(char* sessionId,char* tabName ,char* pFiles,char* pValues)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[5]->UnBindParam();
        ptfDML[5]->BindParam(1,sessionId) ;
        ptfDML[5]->BindParam(2,tabName) ;
        ptfDML[5]->BindParam(3,pFiles) ;
        ptfDML[5]->BindParam(4,pValues) ;

        ptfDML[5]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[5]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}


int DcThreadProcess::getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId)
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

int DcThreadProcess::getDataType()
{

    int ret =0 ;
    char buf[1024] = {0};
    //DCRMDBConnection* pConn  =NULL;
    DCRMDBDQL*        pDQLL = NULL;

    try
    {

        char iBuf[128]= {0};
//   DcConfig* iconfig = DcConfig::GetInstance();
//   strcpy(iBuf,iconfig->OCSOracleString) ;
//
//   unsigned int ret = DCRMDBPool::Instance()->Init(iBuf, 8);
//   if (ret)
//   {
//
//          ACE_DEBUG((MY_ERROR ACE_TEXT("init RMDB failed!!")));
//          return -1;
//   }
//
//       pConn = DCRMDBPool::Instance()->GetConnection();
//       if (NULL == pConn)
//       {
//              ACE_DEBUG((MY_ERROR ACE_TEXT("get connection failed!!")));
//              return -2;
//       }

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

int DcThreadProcess::getTablePrimaryKeyString(char *szTableName, char *szFieldName, char *szFieldValue, string &strKeyInfo)
{
	//ACE_DEBUG((MY_DEBUG ACE_TEXT("getTablePrimaryKeyString szTableName[%s],szFieldName[%s],szFieldValue[%s]"), szTableName,szFieldName,szFieldValue));
	int nTableIndex = 0;
	char szCurrField[256] = {0};
	string strAnd = " AND ";
	bool bIsKey = false;
	int nIsChar = 0;  //字段是否字符型，1是；0非
	map<string, int>::iterator itKey;
	if(strcmp(szTableName, "SERV") == 0)
	{
		itKey = m_mapServKey.find(string(szFieldName));
		if(itKey != m_mapServKey.end())
		{
			bIsKey = true;
			nIsChar = itKey->second;
		}
	}
	else if(strcmp(szTableName, "SERV_ATTR") == 0)
	{
		itKey = m_mapServAttrKey.find(string(szFieldName));
		if(itKey != m_mapServAttrKey.end())
		{
			bIsKey = true;
			nIsChar = itKey->second;
		}
	}
	else if(strcmp(szTableName, "SERV_LOCATION") == 0)
	{
		itKey = m_mapServLocationKey.find(string(szFieldName));
		if(itKey != m_mapServLocationKey.end())
		{
			bIsKey = true;
			nIsChar = itKey->second;
		}
	}
	else if(strcmp(szTableName, "A_SERV_EX") == 0)
	{
		itKey = m_mapAServExKey.find(string(szFieldName));
		if(itKey != m_mapAServExKey.end())
		{
			bIsKey = true;
			nIsChar = itKey->second;
		}
	}
	else 
	{
	}

	if(bIsKey)
	{
		ACE_DEBUG((MY_DEBUG ACE_TEXT("find key info szFieldName[%s]"), szFieldName));
		if(nIsChar == 1)
		{
			sprintf(szCurrField, "%s = '%s'",szFieldName,szFieldValue);
		}
		else
		{
			sprintf(szCurrField, "%s = %s",szFieldName,szFieldValue);
		}
		
		ACE_DEBUG((MY_DEBUG ACE_TEXT("find key info szCurrField[%s]"), szCurrField));

		if(strKeyInfo.size() == 0)
		{
			strKeyInfo = string(szCurrField);
		}
		else
		{
			strKeyInfo += strAnd;
			strKeyInfo += string(szCurrField);
		}
	}
	//ACE_DEBUG((MY_DEBUG ACE_TEXT("get strKeyInfo[%s]"), strKeyInfo.c_str()));

	return 0;
}

//传入捕获到的错误信息，判断是否主键冲突，是返回true，否则返回false
bool DcThreadProcess::checkInsertError(const char* pszErrorInfo)
{
	char szTmpError[2048] = {0};
	char szError[20] = {0};
	char *pszFind = NULL;
	
	strcpy(szError,"unique constraint");
	strcpy(szTmpError, pszErrorInfo);
	strcpy(szTmpError, PublicLib::strtolower(pszErrorInfo,szTmpError));

	pszFind = PublicLib::FindSubStr(szTmpError,szError);
	if(pszFind == NULL)
	{
		return false;
	}
	else
	{
        ACE_DEBUG((MY_ERROR ACE_TEXT("insert error info[%s] is unique constraint error!"),pszErrorInfo));
		return true;
	}
}

int DcThreadProcess::deleteRecordByKey(char* pszSql)
{
    unsigned int ret =0 ;
    DCRMDBDML*  tpDML =NULL;
    try
    {
        tpDML = new DCRMDBDML(pCon);
		if(strlen(pszSql) == 0)
		{
			ACE_DEBUG((MY_DEBUG ACE_TEXT("delete sql is null!")));
			return 0;
		}
       
        ret= tpDML->Prepare(pszSql);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] failed!"), pszSql));
            if(tpDML !=NULL)
            {
                delete tpDML ;
                tpDML =NULL ;
            }
            return -1;
        }
		
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execute-sql: %s"), pszSql));
        tpDML->Execute();
        if(tpDML !=NULL)
        {
            delete tpDML;
            tpDML =NULL;
        }
    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        if(tpDML !=NULL)
        {
            delete tpDML ;
            tpDML =NULL ;
        }
		
		return -1;
    }

    return 0 ;
}

int DcThreadProcess::deleteServByServId(int nPartitionId,long lnServId)
{
	//nPartitionId = nPartitionId + 1;
    try
    {
        ptfServDML[nPartitionId]->UnBindParam();
		ACE_DEBUG((MY_DEBUG ACE_TEXT("DcThreadProcess::deleteServByServId execption-sql: delete from SERV partition(t_partition_p%d) where SERV_ID=%q begin"), nPartitionId+1,lnServId));

        ptfServDML[nPartitionId]->BindParam(1,lnServId);
        ptfServDML[nPartitionId]->Execute();
        //Commit();
    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"), e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"), e.GetSqlErrorInfo()));
        return -1 ;
    }

	ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: delete from SERV partition(t_partition_p%d) where SERV_ID=%q finished"), nPartitionId+1,lnServId));
    return 0 ;
}

int DcThreadProcess::getServAttr(long lnServId, long lnAttrId, char *szAttrVal, bool &bIsOCSUser)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDQL[1]->UnBindParam();
        ptfDQL[1]->BindParam(1,lnServId);
        ptfDQL[1]->BindParam(2,lnAttrId);
        ptfDQL[1]->BindParam(3,szAttrVal);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("getServAttr sql[select SERV_ID from SERV_ATTR where SERV_ID = :1 and ATTR_ID = :2 and ATTR_VAL = :3], BindValue: serv_id[%q],attr_id[%q],attr_val[%s]"),
			lnServId,lnAttrId,szAttrVal));
        ptfDQL[1]->Query();
        if(ptfDQL[1]->Next())
        {
			ACE_DEBUG((MY_DEBUG ACE_TEXT("This serv_id[%q] is an ocs user"),lnServId));
			bIsOCSUser = true;
        }
		else
		{
			ACE_DEBUG((MY_DEBUG ACE_TEXT("This serv_id[%q] is not an ocs user, need not deal"),lnServId));
			bIsOCSUser = false;
		}
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[1]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0;
}


