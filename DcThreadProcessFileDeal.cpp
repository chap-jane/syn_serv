#include "DcThreadProcessFileDeal.h"
#include "syn_struct.h"


DcThreadProcessFileDeal::DcThreadProcessFileDeal()
{
    m_pIndb =NULL ;
}

DcThreadProcessFileDeal::~DcThreadProcessFileDeal()
{

    if(m_pIndb !=NULL)
    {
        delete m_pIndb ;
        m_pIndb =NULL ;
    }

}

int DcThreadProcessFileDeal::routine()
{

    ACELogMsgCallbackCmn *pLogMsgCallback = ACELogMsgCallbackCmn_SIG::instance();
    ACE_LOG_MSG->set_flags (ACE_Log_Msg::MSG_CALLBACK);
    ACE_LOG_MSG->msg_callback (pLogMsgCallback);
    pLogMsgCallback->SetLogLevel(4,0,0,0,4);
    pLogMsgCallback->ChangePMask(LM_DEBUG|LM_ERROR|LM_WARNING|LM_INFO);

    //此线程负责，文件处理和重连

    ACE_DEBUG((MY_DEBUG ACE_TEXT("*****************文件/重连线程****************"))) ;
    CDCCEntry*  m_pDCCEntry = CDCCEntry::instance();

    while(1)
    {

        char iBuf[4]= {0};
        int ret=0 ;
        DcConfig* iconfig = DcConfig::GetInstance();
        strcpy(iBuf,iconfig->m_fileProFlag) ;
        int sleepTime=iconfig->m_reConIntervalTime ;
        if(strcmp("yes",iBuf)== 0 || strcmp("YES",iBuf)== 0 ||strcmp("Yes",iBuf)== 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("##############开始文件接口###########")));
            m_pIndb=Indb::GetInstance();
        label2:
            if((ret=m_pIndb->runProcess())== 0)//文件接口
            {
                goto label2 ;
            }
        }

        ret = m_pDCCEntry->status() ;
        if(0 != ret)
        {
            sleep(sleepTime) ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("the Hss disconnected and re connect to Hss!"))) ;
            m_pDCCEntry->reset(0);
        }


    }

    return 0 ;
} ;
