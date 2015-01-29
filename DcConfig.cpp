/*******************************************
*Copyrights  2011��������Դ�ϿƼ�������޹�˾
*                   ����ƽ̨��Ŀ��
*All rights reserved.
*
*Filename��
*       DcConfig.cpp
*Indentifier��
*
*Description��
*       �����ļ���
*Version��
*       V1.0
*Author:
*       yancy
*Finished��
*       2012��2��20��
*History:
*       2012/2/20  V1.0 �ļ�����
********************************************/

#include "DcConfig.h"
#include "util/publiclib.h"
/*��̬��Ա�����ⲿ��Ҫ��ʼ��*/
DcConfig* DcConfig::m_instance = NULL ;


DcConfig::DcConfig()
{
    memset(m_logFullPath,0,sizeof(m_logFullPath)) ;
    memset(OCSOracleString,0,128) ;
    memset(m_fileProFlag,0,4) ;
    memset(m_bakPath,0,100) ;
    memset(m_errPath,0,100) ;
    memset(m_invPath,0,100) ;

}


DcConfig::~DcConfig()
{
    ;
}


DcConfig* DcConfig::GetInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new DcConfig() ;
    }

    return m_instance ;
}

DcConfig::Init()
{
    string  t_str="" ;
    /*��ȡ����threadCount��*/
    if(!PublicLib::OCS_readConfig(t_str,"threadCount","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("threadCount not config.set default value 1")));
        m_threadCount = 1;
    }
    else
    {
        m_threadCount = atoi(t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("threadCount = %d"),m_threadCount));
    }

    if(!PublicLib::OCS_readConfig(t_str,"reConnTime","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("reConnTime not config.set default value 60")));
        m_reConIntervalTime = 60;
    }
    else
    {
        m_reConIntervalTime = atoi(t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("reConnTime = %d"),m_reConIntervalTime));
    }

    /*��ȡ����serviceNo��*/
    if(!PublicLib::OCS_readConfig(t_str,"serviceNo","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("ServiceNo not config.set default value 1")));
        m_serviceNo = 1;
    }
    else
    {
        m_serviceNo = atoi(t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("m_serviceNo = %d"),m_serviceNo));
    }

    /*��ȡ����dbUser��*/
    if(!PublicLib::OCS_readConfig(t_str,"OCSOracleString","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("dbusr is not set!!")));
    }
    else
    {
        strcpy(OCSOracleString,t_str.c_str()) ;
        //OCSOracleString =t_str ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("OCSOracleString = %s"),OCSOracleString));
    }

    /*��ȡ����logPath��*/
    if(!PublicLib::OCS_readConfig(t_str,"logPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("logPath is not set!!")));
    }
    else
    {
        strcpy(m_logFullPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("logPath = %s"),m_logFullPath));
    }

    /*��ȡ����bakPath��*/
    if(!PublicLib::OCS_readConfig(t_str,"bakPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("bakPath is not set!!")));
    }
    else
    {
        strcpy(m_bakPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("bakPath = %s"),m_bakPath));
    }

    /*��ȡ����errPath��*/
    if(!PublicLib::OCS_readConfig(t_str,"errPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("errPath is not set!!")));
    }
    else
    {
        strcpy(m_errPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("errPath = %s"),m_errPath));
    }

    /*��ȡ����invPath��*/
    if(!PublicLib::OCS_readConfig(t_str,"invPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("invPath is not set!!")));
    }
    else
    {
        strcpy(m_invPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("invPath = %s"),m_invPath));
    }

    /*��ȡ����fileProFlag��*/
    if(!PublicLib::OCS_readConfig(t_str,"HssMsgDumpFlag","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("HssMsgDumpFlag is not set!!")));
        m_nHssDumpFlag = 0;
    }
    else
    {
        m_nHssDumpFlag = atoi(t_str.c_str());
        ACE_DEBUG((MY_DEBUG ACE_TEXT("HssMsgDumpFlag = %d"),m_nHssDumpFlag));
    }

    /*��ȡ����UpdateFlag��*/
    if(!PublicLib::OCS_readConfig(t_str,"ReserveOCSAndHBData","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("ReserveOCSAndHBData is not set!!")));
        m_nReserveOCSAndHBData = 0;
    }
    else
    {
        m_nReserveOCSAndHBData = atoi(t_str.c_str());
        ACE_DEBUG((MY_DEBUG ACE_TEXT("ReserveOCSAndHBData = %d"),m_nHssDumpFlag));
    }

    if(!PublicLib::OCS_readConfig(t_str,"ServNeedCombine","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("ServNeedCombine is not set!!")));
        m_nServNeedCombine = 0;
    }
    else
    {
        m_nServNeedCombine = atoi(t_str.c_str());
        ACE_DEBUG((MY_DEBUG ACE_TEXT("ServNeedCombine = %d"),m_nHssDumpFlag));
    }

    return 0 ;
}

DcConfig::Destroy()
{
    if (m_instance)
    {
        delete m_instance ;
        m_instance =NULL ;
    }

//  if (m_sqlPrepare){
//      delete m_sqlPrepare;
//      m_sqlPrepare =NULL ;
//  }

    return 0 ;
}

