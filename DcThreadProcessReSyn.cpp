#include "DcThreadProcessReSyn.h"
#include "syn_struct.h"


static long sIndex = 0 ;
static int m_fullflag = 0;
DcThreadProcessReSyn::DcThreadProcessReSyn(CDCCEntry *pDCCEntry, DcConfig* pConfig):m_errBuf()
{
    this->m_pDCCEntry = pDCCEntry ;
    m_scConfig  = pConfig   ;
    memset(m_session_id,0,sizeof(m_session_id)) ;
    m_pXml=NULL ;
    //m_pXml=new DcParseXml() ;
    m_pXml=DcParseXml::GetInstance();
    m_fullflag=0;

}
DcThreadProcessReSyn::~DcThreadProcessReSyn()
{

}


int DcThreadProcessReSyn::routine()
{

    int ret = -1 ;
    int nCode = 0;
    CDCCMsg *pMsg = NULL;

    ACELogMsgCallbackCmn *pLogMsgCallback = ACELogMsgCallbackCmn_SIG::instance();
    ACE_LOG_MSG->set_flags (ACE_Log_Msg::MSG_CALLBACK);
    ACE_LOG_MSG->msg_callback (pLogMsgCallback);
    pLogMsgCallback->SetLogLevel(4,0,0,0,4);
    pLogMsgCallback->ChangePMask(LM_DEBUG|LM_ERROR|LM_WARNING|LM_INFO);


    ACE_DEBUG((MY_DEBUG ACE_TEXT("*****************消息接口****************"))) ;
    while(1)
    {
        ret =ReSynchronization();
        if(ret)
        {
            sleep(5) ;
        }
    }

    return 0 ;
}

CValue* DcThreadProcessReSyn::GetMsgAVP(const DCC_BODY &msgBody ,unsigned int code)
{
    //ACE_DEBUG((MY_DEBUG ACE_TEXT("&msgBody is[%x], msgBody.size[%d], code[%d]!"),&msgBody,msgBody.size(),code));

    if(&msgBody == NULL)
    {
        return NULL;
    }
    for(int i=0; i<msgBody.size(); i++)
    {
    	ACE_DEBUG((MY_DEBUG ACE_TEXT("msgBody[i]->code[%d], msgBody[i]->type[%d]"),msgBody[i]->code(),msgBody[i]->type()));
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



int DcThreadProcessReSyn::SendTransferCCR(syn_transfer_t& t,char* orgId)
{
    int ret = 0;
    int i = 0;
    CDCCMsg* retMsg = new CDCCMsg(496) ;
    char buf[4096] = {0};
    //SDCCHead& head = retMsg.head();

    ret = m_pDCCEntry->prepare(*retMsg, 0);
    if (ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC Prepare Failed")));
        return -1;
    }


    char tStr[128]= {0} ;
    ret=SetSessionID(retMsg->getBody(),tStr);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC SetSessionID Failed")));
        return -1 ;
    }


    char systime[21]= {0} ;
    PublicLib::GetTime(systime,0) ;
    ///设置accpt_date
    ret=SetMsgAVP(retMsg->getBody(),82002,systime);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set accpt_date Failed")));
        return -1 ;
    }

    ///Complete-Date
    ret=SetMsgAVP(retMsg->getBody(),82003,systime);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set Complete-Date Failed")));
        return -1 ;
    }


    ///设置org_id
    ret=SetMsgAVP(retMsg->getBody(),82103,orgId);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set org_id Failed")));
        return -1 ;
    }

//      ret=SetMsgAVP(retMsg.getBody(),82007,"10h");
//     if(ret)
//     {
//      ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set action_id Failed")));
//      return -1 ;
//     }

    for (i = 0; i < retMsg->body().size(); i++)
    {

        //请求类型
        if(416 == retMsg->body()[i]->code())
        {
            int reqtype =4 ;
            retMsg->body()[i]->set(reqtype);
            continue ;
        }
        //serivce_infomation
        if(873 ==retMsg->body()[i]->code())
        {
            DCC_BODY* pSerInfo = (DCC_BODY*)retMsg->body()[i]->toPtr();
            for (int j = 0; j < pSerInfo->size(); j++)
            {
                //old_acct_id
                if(80107 == (*pSerInfo)[j]->code())
                {
                    long l_old =atol(t.old_acct_id) ;
                    long long ll_old=l_old ;
                    (*pSerInfo)[j]->set(ll_old);
                    continue ;
                }

                //Serv-Id
                if(5001 == (*pSerInfo)[j]->code())
                {
                    long l_servid =atol(t.user_id) ;
                    long long ll_servid=l_servid   ;
                    (*pSerInfo)[j]->set(ll_servid);
                    continue ;
                }

                //Trans-Id
                if(5100 == (*pSerInfo)[j]->code())
                {

                    char systime[21]= {0} ;
                    PublicLib::GetTime(systime,0) ;
                    long long l_tranId =atol(systime) ;

                    (*pSerInfo)[j]->set(l_tranId);
                    continue ;
                }

                //Acct-Id2 neew
                if(40000 == (*pSerInfo)[j]->code())
                {
                    long l_newacctId =atol(t.new_acct_id) ;
                    long long ll_newacctId=l_newacctId   ;
                    (*pSerInfo)[j]->set(ll_newacctId);
                    continue ;
                }
            }
        }

    }

    ret = m_pDCCEntry->send(*retMsg, 0, buf, 4096, 0, 5);
    char sendbuf[4096]= {0} ;
    retMsg->dump(sendbuf);
    ACE_DEBUG((MY_DEBUG ACE_TEXT("transfer ccr send : %s"),sendbuf));
    sendbuf[0]='\0' ;
    buf[0]='\0';
    if (ret)
    {
        delete retMsg ;
        retMsg =NULL ;
        return -1 ;
    }
    delete retMsg ;
    retMsg =NULL ;
    ACE_DEBUG((MY_DEBUG ACE_TEXT("过户CCR 发送成功")));
    return 0 ;
}

int DcThreadProcessReSyn::SetSessionID(const DCC_BODY &msgBody,char* sessionId)
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
    PublicLib::GetTime(systime,0) ;

    long index = sIndex++ ;
    char Session[64]= {0};
    sprintf(Session,"%s%s%ld",pOriginHost,systime,index);
    memset(m_session_id,0,sizeof(m_session_id)) ;
    strcpy(m_session_id,Session) ;
    strcpy(sessionId,Session) ;
    ///设置会话ID
    for (int i = 0; i < msgBody.size(); i++)
    {
        if (msgBody[i]->code() == 263) ///session_id
        {
            msgBody[i]->set(Session);
            return 0;
        }
    }
    return 1;
}



int  DcThreadProcessReSyn::SendReSynCCR(char* xml ,char* actionId,char* orgId,char* sessionId)
{
    int ret = 0;
    int i = 0;
    char buf[4096] = {0};
    //SDCCHead& head = retMsg.head();
    //head.version= DCC_VERSION;

   ret=GetMsgFull();
   ACE_DEBUG((MY_DEBUG ACE_TEXT("GetMsgFull retrun=%d"),ret));
	 if(ret)
     	{
     	ACE_DEBUG((MY_DEBUG ACE_TEXT("1006Deal")));
     	usleep(5000000);
		  this->SetMsgFull(0);
     	}
  CDCCMsg* retMsg =new CDCCMsg(496) ;
    ret = m_pDCCEntry->prepare(*retMsg, 0);
    if (ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC Prepare Failed")));
        return NULL;
    }

    //设置会话ID
    char tStr[128]= {0} ;
    ret=SetSessionID(retMsg->getBody(),tStr);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC SetSessionID Failed")));
        return 1 ;
    }

    //char tBuf[128]={0};
    strcpy(sessionId,tStr);

    //int iOrgId =atoi(orgId) ;
    ///设置org_id
    ret=SetMsgAVP(retMsg->getBody(),82103,orgId);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set org_id Failed")));
        return 1 ;
    }




    ///设置用户信息数据行 Data-Table-Simple-Row
    //char* testStr="<ACCT_DOMAIN ACCT_ID=116139613><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT></ACCT_DOMAIN><ACCT_DOMAIN ACCT_ID=116139613><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT><SERV_ACCT FLAG=1><NEW><ACCT_ID>116139613</ACCT_ID><ITEM_GROUP_ID>1</ITEM_GROUP_ID><PAYMENT_LIMIT>0</PAYMENT_LIMIT><PAYMENT_LIMIT_TYPE>5XA</PAYMENT_LIMIT_TYPE><PAYMENT_RULE_ID>803894848</PAYMENT_RULE_ID><PRIORITY>0</PRIORITY><SERV_ID>102018770626</SERV_ID><STATE>10A</STATE><STATE_DATE>20120521</STATE_DATE></NEW></SERV_ACCT></ACCT_DOMAIN>" ;

    ret=SetMsgAVP(retMsg->getBody(),80406,xml);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set Data-Table-Simple-Row Failed")));
        return 1 ;
    }

    ///设置ACTION_ID
    ret=SetMsgAVP(retMsg->getBody(),82007,actionId);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set action_id Failed")));
        return 1 ;
    }

    ///Service-Context-Id
    ret=SetMsgAVP(retMsg->getBody(),461,"InfoSynchro-ProductDomain.HSS@020.ChinaTelecom.com");
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set action_id Failed")));
        return 1 ;
    }


    char systime[21]= {0} ;
    PublicLib::GetTime(systime,0) ;

    ///设置accpt_date
    ret=SetMsgAVP(retMsg->getBody(),82002,systime);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set ccpt_date Failed")));
        return 1 ;
    }

    ///Complete-Date
    ret=SetMsgAVP(retMsg->getBody(),82003,systime);
    if(ret)
    {
        ACE_ERROR(( MY_DEBUG ACE_TEXT("DCC set Complete-Date Failed")));
        return 1 ;
    }

    char sendbuf[8096]= {0} ;
    retMsg->dump(sendbuf);
    ACE_DEBUG((MY_DEBUG ACE_TEXT("发送反向同步CCR : %s"),sendbuf));

    ret = m_pDCCEntry->send(*retMsg, 0, buf, 4096, 0, 5);
    if (ret)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("发送反向同步CCR 失败发送不成功,检查连接")));
        delete retMsg ;
        retMsg = NULL ;
        return 1 ;
    }

    if(retMsg != NULL)
    {
        delete retMsg ;
        retMsg = NULL ;
    }

    ACE_DEBUG((MY_DEBUG ACE_TEXT("发送反向同步CCR : 发送成功")));


    return 0 ;
}

////////设置AVP
int  DcThreadProcessReSyn::SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code,char* str)
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




int  DcThreadProcessReSyn::SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code, int i)
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


int DcThreadProcessReSyn::ComposeXmlByServ(serv_t t,serv_state_attr_t s,serv_state_attr_t n,char* pState,char* nState,char* create_date,char* xml)
{
    char tBuf[1024]= {0} ;
    char ABuf[2048]= {0} ;
    char sBuf[1024]= {0} ;

    if(s.serv_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<SERV_ID>%s</SERV_ID>",s.serv_id) ;
        strcat(sBuf,tmpBuf) ;

    }

    if(s.agreement_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<AGREEMENT_ID>%s</AGREEMENT_ID>",s.agreement_id) ;
        strcat(sBuf,tmpBuf) ;
    }

    if(s.owe_business_type_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<OWE_BUSINESS_TYPE_ID>%s</OWE_BUSINESS_TYPE_ID>",s.owe_business_type_id) ;
        strcat(sBuf,tmpBuf) ;
    }

//      if(s.eff_date[0] !='\0')
//      {
//          char tmpBuf[256]={0} ;
//          sprintf(tmpBuf,"<EFF_DATE>%s</EFF_DATE>",s.eff_date) ;
//          strcat(sBuf,tmpBuf) ;
//      }
//
//      if(s.exp_date[0] !='\0')
//      {
//          char tmpBuf[256]={0} ;
//          sprintf(tmpBuf,"<EXP_DATE>%s</EXP_DATE>",s.exp_date) ;
//          strcat(sBuf,tmpBuf) ;
//      }
//

    if(t.serv_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<SERV_ID>%s</SERV_ID>",t.serv_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.agreement_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<AGREEMENT_ID>%s</AGREEMENT_ID>",t.agreement_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.cust_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<CUST_ID>%s</CUST_ID>",t.cust_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.product_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<PRODUCT_ID>%s</PRODUCT_ID>",t.product_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    //  if(t.billing_cycle_type_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<BILLING_CYCLE_TYPE_ID>%s</BILLING_CYCLE_TYPE_ID>",t.billing_cycle_type_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    //  if(t.product_family_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<PRODUCT_FAMILY_ID>%s</PRODUCT_FAMILY_ID>",t.product_family_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.create_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<CREATE_DATE>%s</CREATE_DATE>",t.create_date) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.rent_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<RENT_DATE>%s</RENT_DATE>",t.rent_date) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.completed_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<COMPLETED_DATE>%s</COMPLETED_DATE>",t.completed_date) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.acc_nbr[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<ACC_NBR>%s</ACC_NBR>",t.acc_nbr) ;
        strcat(tBuf,tmpBuf) ;
    }

//      if(t.state[0] !='\0')
//      {
//          char tmpBuf[256]={0} ;
//          sprintf(tmpBuf,"<STATE>%s</STATE>",t.state) ;
//          strcat(tBuf,tmpBuf) ;
//      }

    if(t.state_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<STATE_DATE>%s</STATE_DATE>",t.state_date) ;
        strcat(tBuf,tmpBuf) ;
    }

    char newBuf[1024]= {0} ;
    if(n.serv_id[0]!='0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<SERV_ID>%s</SERV_ID>",n.serv_id) ;
        strcat(newBuf,tmpBuf) ;
    }



    if(n.agreement_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<AGREEMENT_ID>%s</AGREEMENT_ID>",n.agreement_id) ;
        strcat(newBuf,tmpBuf) ;
    }

    if(n.owe_business_type_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<OWE_BUSINESS_TYPE_ID>%s</OWE_BUSINESS_TYPE_ID>",n.owe_business_type_id) ;
        strcat(newBuf,tmpBuf) ;
    }

    if(n.eff_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<EFF_DATE>%s</EFF_DATE>",n.eff_date) ;
        strcat(newBuf,tmpBuf) ;
    }

    if(n.state[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<STATE>%s</STATE>",n.state) ;
        strcat(newBuf,tmpBuf) ;
    }

    if(n.exp_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<EXP_DATE>%s</EXP_DATE>",n.exp_date) ;
        strcat(newBuf,tmpBuf) ;
    }

    sprintf(ABuf,"<PRODUCT_DOMAIN SERV_ID=%s><SERV FLAG=3><OLD>%s<STATE>%s</STATE></OLD><NEW>%s<STATE>%s</STATE></NEW></SERV><SERV_STATE_ATTR FLAG=3><OLD>%s<STATE>%s</STATE><EFF_DATE>%s</EFF_DATE><EXP_DATE>%s</EXP_DATE></OLD><NEW>%s<STATE>%s</STATE><EFF_DATE>%s</EFF_DATE><EXP_DATE>%s</EXP_DATE></NEW></SERV_STATE_ATTR><SERV_STATE_ATTR FLAG=1><NEW>%s</NEW></SERV_STATE_ATTR>",t.serv_id,tBuf,pState,tBuf,nState,sBuf,pState,s.eff_date,s.exp_date,sBuf,pState,s.eff_date,n.eff_date,newBuf) ;
    //sprintf(ABuf,"<PRODUCT_DOMAIN SERV_ID=%s><SERV FLAG=3><OLD>%s<STATE>%s</STATE></OLD><NEW>%s<STATE>%s</STATE></NEW></SERV><SERV_STATE_ATTR FLAG=3><OLD>%s<STATE>%s</STATE><EFF_DATE>%s</EFF_DATE><EXP_DATE>%s</EXP_DATE></OLD><NEW>%s<STATE>%s</STATE><EFF_DATE>%s</EFF_DATE><EXP_DATE>%s</EXP_DATE></NEW></SERV_STATE_ATTR><SERV_STATE_ATTR FLAG=1><NEW>%s<STATE>%s</STATE><EFF_DATE>%s</EFF_DATE><EXP_DATE>30000101000000</EXP_DATE></NEW></SERV_STATE_ATTR>" ,t.serv_id,tBuf,pState,tBuf,nState,sBuf,pState,s.eff_date,s.exp_date,sBuf,pState,s.eff_date,create_date,sBuf,nState,create_date) ;

    strcpy(xml,ABuf) ;
    return  0 ;
}

int DcThreadProcessReSyn::ComposeXmlByServAttr(serv_attr_t t,char* effDate,char* expDate,char* xml)
{
    char tBuf[1024]= {0} ;
    char ABuf[2048]= {0} ;
    if(t.serv_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<SERV_ID>%s</SERV_ID>",t.serv_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.agreement_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<AGREEMENT_ID>%s</AGREEMENT_ID>",t.agreement_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.attr_id[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<ATTR_ID>%s</ATTR_ID>",t.attr_id) ;
        strcat(tBuf,tmpBuf) ;
    }

    if(t.eff_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<EFF_DATE>%s</EFF_DATE>",effDate) ;
        strcat(tBuf,tmpBuf) ;
    }

    char systime[21]= {0} ;
    PublicLib::GetTime(systime,0) ;

    if(t.exp_date[0] !='\0')
    {
        char tmpBuf[256]= {0} ;
        sprintf(tmpBuf,"<EXP_DATE>%s</EXP_DATE>",expDate) ;
        strcat(tBuf,tmpBuf) ;
    }

    sprintf(ABuf,"<PRODUCT_DOMAIN SERV_ID=%s><SERV_ATTR FLAG=1><NEW>%s<ATTR_VAL>0</ATTR_VAL></NEW></SERV_ATTR></PRODUCT_DOMAIN> " ,t.serv_id,tBuf) ;

    strcpy(xml,ABuf) ;
    return  0 ;

}

int DcThreadProcessReSyn::ReSynchronization()
{

    unsigned int ret =0 ;
    //扫描激活接口A表
    tif_serv_activation_a_t serv_a ;
    memset(&serv_a ,0x00,sizeof(serv_a)) ;
    ret =m_pXml->getTifServActivationA(serv_a) ;
    if(0 == ret && serv_a.serv_id[0] !='\0')
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("处理：tif_serv_activation.serv_id=%s 的记录"),serv_a.serv_id));
        serv_t tServ ;
        memset(&tServ,0x00,sizeof(tServ)) ;
        ret=m_pXml->getServ(tServ,serv_a.serv_id) ;

        serv_location_t tOrg ;
        memset(&tOrg,0x00,sizeof(tOrg)) ;
        m_pXml->getServLocation(tOrg,serv_a.serv_id) ;

        serv_state_attr_t state_t ;
        memset(&state_t ,0x00 ,sizeof(state_t)) ;

        //old
        m_pXml->getServStateAttr(state_t ,serv_a.serv_id,"2HN") ;
        //new
        serv_state_attr_t state_new_t ;
        memset(&state_new_t ,0x00 ,sizeof(state_new_t)) ;
        m_pXml->getServStateAttr(state_new_t ,serv_a.serv_id,"2HA") ;

        org_t or_t ;
        memset(&or_t,0x00,sizeof(or_t)) ;
        m_pXml->getOrg(or_t,tOrg.exchange_id) ;

        if(ret==0 && tServ.serv_id[0] !='\0' &&state_new_t.serv_id[0] !='\0' && tOrg.exchange_id[0] !='\0' && or_t.org_id[0] !='\0' && state_t.serv_id[0] !='\0')
        {
            char strxml[2048] = {0} ;
            ComposeXmlByServ(tServ,state_t,state_new_t,serv_a.pre_serv_state,serv_a.state,serv_a.create_date,strxml) ;
            if(strxml[0] !='\0')
            {
                char s[128] = {0} ;
                ret= SendReSynCCR(strxml,"10p",or_t.org_id,s) ;
                if(!ret)
                {
                    m_pXml->setResynInerfaceRel(m_session_id,"tif_serv_activation","serv_id",serv_a.serv_id) ;
                    m_pXml->setTifServActivationA(serv_a.serv_id,"9") ;
                    return 0 ;
                }
                m_pXml->setTifServActivationA(serv_a.serv_id,"9") ;
            }
        }
        else
        {
            m_pXml->setTifServActivationA(serv_a.serv_id,"9") ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("serv.serv_id =%s 数据为空 或者serv_location.exchange_id ,serv_state_attr,org_t为空 "),serv_a.serv_id));
        }
    }

    ///扫描停复机接口表
    cdma_stop_open_interface_t open_t ;
    memset(&open_t,0x00,sizeof(open_t)) ;
    ret =m_pXml->getCdmaStopOpenInterface(open_t) ;
    if(ret==0 && open_t.oper_id[0] !='\0')
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("处理：tb_bil_user_change_history.oper_id=%s 的记录"),open_t.oper_id));
        serv_t tServ ;
        memset(&tServ,0x00,sizeof(tServ)) ;
        ret=m_pXml->getServ(tServ,open_t.serv_id) ;

        serv_location_t tOrg ;
        memset(&tOrg,0x00,sizeof(tOrg)) ;
        m_pXml->getServLocation(tOrg,open_t.serv_id) ;


        serv_state_attr_t state_t ;
        memset(&state_t ,0x00 ,sizeof(state_t)) ;

        serv_state_attr_t state_new_t ;
        memset(&state_new_t ,0x00 ,sizeof(state_new_t)) ;
        if(strcmp(open_t.stop_type,"31")==0)
        {
            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HA") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HS") ;
        }
        else if(strcmp(open_t.stop_type,"32")==0)
        {
            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HS") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HD")  ;
        }
        else if(strcmp(open_t.stop_type,"33")==0)
        {
            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HS") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HA") ;
        }
        else if(strcmp(open_t.stop_type,"34")==0)
        {
            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HD") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HA") ;
        }
        else if(strcmp(open_t.stop_type,"36")==0)
        {
            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HD") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HA") ;
        }
        else if(strcmp(open_t.stop_type,"37")==0)
        {
            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HA") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HD") ;
        }
        else if(strcmp(open_t.stop_type,"38")==0)
        {

            m_pXml->getServStateAttr(state_t ,open_t.serv_id,"2HA") ;
            m_pXml->getServStateAttr(state_new_t ,open_t.serv_id,"2HD") ;
        }

        org_t or_t ;
        memset(&or_t,0x00,sizeof(or_t)) ;
        m_pXml->getOrg(or_t,tOrg.exchange_id) ;

        if(ret==0 && tServ.serv_id[0] !='\0'&& state_new_t.serv_id[0] !='\0' && tOrg.exchange_id[0] !='\0' && state_t.serv_id[0] !='\0')
        {
            char strxml[2048] = {0} ;
            char action[5]= {0};
            if(strcmp(open_t.stop_type,"31")==0)
            {
                ComposeXmlByServ(tServ,state_t,state_new_t,"2HA","2HS",open_t.create_time,strxml) ;
                strcpy(action,"10w") ;
            }
            else if(strcmp(open_t.stop_type,"32")==0)
            {
                ComposeXmlByServ(tServ,state_t,state_new_t,"2HS","2HD",open_t.create_time,strxml) ;
                strcpy(action,"10x") ;
            }
            else if(strcmp(open_t.stop_type,"33")==0)
            {

                ComposeXmlByServ(tServ,state_t,state_new_t,"2HS","2HA",open_t.create_time,strxml) ;
                strcpy(action,"10u") ;
            }
            else if(strcmp(open_t.stop_type,"34")==0)
            {

                ComposeXmlByServ(tServ,state_t,state_new_t,"2HD","2HA",open_t.create_time,strxml) ;
                strcpy(action,"10v") ;
            }
            else if(strcmp(open_t.stop_type,"36")==0)
            {
                ComposeXmlByServ(tServ,state_t,state_new_t,"2HD","2HA",open_t.create_time,strxml) ;
                strcpy(action,"10v") ;

            }
            else if(strcmp(open_t.stop_type,"37")==0)
            {
                ComposeXmlByServ(tServ,state_t,state_new_t,"2HA","2HD",open_t.create_time,strxml) ;
                strcpy(action,"10x") ;

            }
            else if(strcmp(open_t.stop_type,"38")==0)
            {
                ComposeXmlByServ(tServ,state_t,state_new_t,"2HA","2HD",open_t.create_time,strxml) ;
                strcpy(action,"10x") ;
            }

            if(strxml[0] !='\0')
            {
                char s[128] = {0} ;
                ret= SendReSynCCR(strxml,action,or_t.org_id,s) ;
                if(!ret)
                {
                    m_pXml->setResynInerfaceRel(m_session_id,"tb_bil_user_change_history","oper_id",open_t.oper_id) ;
                    m_pXml->setCdmaStopOpenInterface(open_t.oper_id,"9") ;
                    return 0 ;
                }
                m_pXml->setCdmaStopOpenInterface(open_t.oper_id,"9") ;
            }
        }
        else
        {
            m_pXml->setCdmaStopOpenInterface(open_t.oper_id,"9") ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("根据tb_bil_user_change_history.serv_id=%s 关联不到serv,serv_location,org,serv_state_attr表相关记录!!!"),open_t.serv_id));
        }
    }


    ////扫描上网卡限制表
    tb_bil_trigger_record_t red_t ;
    memset(&red_t,0x00,sizeof(red_t)) ;
    ret =m_pXml->getTbBilTriggerRecord(red_t) ;
    if(ret ==0 && red_t.warn_record_id[0] !='\0')
    {

        ACE_DEBUG((MY_DEBUG ACE_TEXT("处理：tb_bil_trigger_record.warn_record_id=%s 的记录"),red_t.warn_record_id));
        serv_attr_t attr_t ;
        memset(&attr_t,0x00,sizeof(attr_t)) ;
        if(strcmp(red_t.param_id ,"XZ1")==0)
        {
            ret =m_pXml->getServAttr(attr_t,red_t.serv_id,"521") ;
        }
        else if(strcmp(red_t.param_id ,"XZ2")==0)
        {
            ret =m_pXml->getServAttr(attr_t ,red_t.serv_id,"520") ;
        }
        else
        {
            m_pXml->setTbBilTriggerRecord(red_t.warn_record_id,"9") ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("tb_bil_trigger_record.param_id 不是 XZ1或XZ2")));
            return -1 ;
        }

        serv_location_t tOrg ;
        memset(&tOrg,0x00,sizeof(tOrg)) ;
        m_pXml->getServLocation(tOrg,red_t.serv_id) ;

        org_t or_t ;
        memset(&or_t,0x00,sizeof(or_t)) ;
        m_pXml->getOrg(or_t,tOrg.exchange_id) ;

        if(ret ==0 && attr_t.serv_id[0] !='\0' && tOrg.exchange_id[0] !='\0' )
        {
            char strxml[2048]= {0} ;

            ComposeXmlByServAttr(attr_t,red_t.create_date,red_t.exp_date,strxml) ;
            if(strxml[0] !='\0')
            {
                char s[128] = {0} ;
                ret= SendReSynCCR(strxml,"10t",or_t.org_id,s) ;
                if(!ret)
                {
                    m_pXml->setResynInerfaceRel(m_session_id,"tb_bil_trigger_record","warn_record_id",red_t.warn_record_id) ;
                    m_pXml->setTbBilTriggerRecord(red_t.warn_record_id,"9") ;
                    return 0 ;
                }
                m_pXml->setTbBilTriggerRecord(red_t.warn_record_id,"9") ;
            }
        }
        else
        {
            m_pXml->setTbBilTriggerRecord(red_t.warn_record_id,"9") ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("select * from serv_attr where serv_id =%s and attr_id =521数据为空 或者serv_location.exchange_id 为空"),red_t.serv_id));
        }
    }


    ////过户成功发CCR给HSS
    syn_transfer_t syn_t ;
    memset(&syn_t,0x00,sizeof(syn_t)) ;
    m_pXml->getTransfer(syn_t) ;
    if(syn_t.user_id[0]=='\0')
    {
        return 0 ;
    }
    else
    {
        m_pXml->updateTransfer("9",syn_t.user_id) ;
    }

    serv_location_t tOrg ;
    memset(&tOrg,0x00,sizeof(tOrg)) ;
    m_pXml->getServLocation(tOrg,syn_t.user_id) ;

    org_t or_t ;
    memset(&or_t,0x00,sizeof(or_t)) ;
    m_pXml->getOrg(or_t,tOrg.exchange_id) ;

    unsigned int r =0 ;
    if(syn_t.old_acct_id[0] != '\0' && syn_t.user_id[0] !='\0' )
    {
        r=SendTransferCCR(syn_t,or_t.org_id);
        if(!r)
        {
            m_pXml->updateTransfer("1",syn_t.user_id) ;
            return 0 ;

        }
        else
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("发送过户消息失败!")));
        }
    }


    return 1 ;
}

int DcThreadProcessReSyn::SetMsgFull(int x)
{
 m_fullflag=x;
 return 0;
}

int DcThreadProcessReSyn::GetMsgFull()
{
return m_fullflag;

}