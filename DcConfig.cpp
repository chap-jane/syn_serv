/*******************************************
*Copyrights  2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcConfig.cpp
*Indentifier：
*
*Description：
*       配置文件类
*Version：
*       V1.0
*Author:
*       yancy
*Finished：
*       2012年2月20日
*History:
*       2012/2/20  V1.0 文件创建
********************************************/

#include "DcConfig.h"
#include "util/publiclib.h"
/*静态成员函数外部需要初始化*/
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
    /*获取配置threadCount项*/
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

    /*获取配置serviceNo项*/
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

    /*获取配置dbUser项*/
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

    /*获取配置logPath项*/
    if(!PublicLib::OCS_readConfig(t_str,"logPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("logPath is not set!!")));
    }
    else
    {
        strcpy(m_logFullPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("logPath = %s"),m_logFullPath));
    }

    /*获取配置bakPath项*/
    if(!PublicLib::OCS_readConfig(t_str,"bakPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("bakPath is not set!!")));
    }
    else
    {
        strcpy(m_bakPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("bakPath = %s"),m_bakPath));
    }

    /*获取配置errPath项*/
    if(!PublicLib::OCS_readConfig(t_str,"errPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("errPath is not set!!")));
    }
    else
    {
        strcpy(m_errPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("errPath = %s"),m_errPath));
    }

    /*获取配置invPath项*/
    if(!PublicLib::OCS_readConfig(t_str,"invPath","SCDataSyn"))
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("invPath is not set!!")));
    }
    else
    {
        strcpy(m_invPath,t_str.c_str()) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("invPath = %s"),m_invPath));
    }

    /*获取配置fileProFlag项*/
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

    /*获取配置UpdateFlag项*/
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

