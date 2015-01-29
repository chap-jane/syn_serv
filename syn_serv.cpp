#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <list>
#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"
#include "pub_common/DCLogLevelCntrllerThr.h"
#include "ace/OS_NS_unistd.h"
#include "dbutil/ttPubFunc.h"
#include "DcParseXml.h"
#include "DcManager.h"

using namespace std;

#define VERSIONINFO "DIC-BIL-OCS-BAPP-SC-V7.17.0"

DCLogLevelCntrllerThr* g_logLevelCntrllerThr=NULL;

int  RegisterAceLog()
{
    printf("RegisterAceLog\n");
    ACELogMsgCallbackCmn *pLogMsgCallback = ACELogMsgCallbackCmn_SIG::instance();

    ACE_LOG_MSG->set_flags (ACE_Log_Msg::MSG_CALLBACK);
    ACE_LOG_MSG->msg_callback (pLogMsgCallback);
    pLogMsgCallback->SetLogLevel(4,0,0,0,4);
    pLogMsgCallback->ChangePMask(LM_DEBUG|LM_ERROR|LM_WARNING|LM_INFO);
    string strValue;

#ifdef WIN32
    strValue = "c:\\";
#else
    strValue = getenv("LOGPATH");
#endif /*WIN32*/

    // 启动日志代理
    DCAceLogAgentCmn::getInstance()->InitFileProperty((char*)strValue.c_str(),"SYN_SERV");
    DCAceLogAgentCmn::getInstance()->Start();


    //------------------------------------------------------------------------//
    //日志分级别
    DCLogLevel myLogLevel(4, 0,0, 0, 4);
    g_logLevelCntrllerThr = new DCLogLevelCntrllerThr(ACE_Thread_Manager::instance());

    int nRet = g_logLevelCntrllerThr->initialize(pLogMsgCallback, myLogLevel);
    if (nRet != 0)
    {
        ACE_ERROR(( MY_ERROR ACE_TEXT("LogLevelControllerThr Initialize Failed")));
        g_logLevelCntrllerThr->force_down();
        DCAceLogAgentCmn::getInstance()->Release();

        return -1;
    }
    return 0;
}



int main(int argc, char *argv[])
{
    if(2==argc)
    {
        if(0==strcmp(argv[1],"-v")||0==strcmp(argv[1],"-ver")||0==strcmp(argv[1],"-version"))
        {
            printf("%s:%s build date %ld \n", argv[0], VERSIONINFO,BUILD_TIME);
            return 0;
        }
    }

    RegisterAceLog();
    int nProcessType = 1 ;     //进程类型1:主  2:备  atoi(argv[1]);
    DcManager* manager = DcManager::GetInstance();

    switch(nProcessType)
    {
        case 1:
        {
            manager->Init();
            break;
        }
        case 2:
        {
            //备进程需要判断主进程是否存在
            break;
        }
        default:
            ;
    }

    //std::string str="<REGULATION_DOMAIN><PRODUCT FLAG=1><NEW><PRODUCT_ID>2</PRODUCT_ID><PRODUCT_PROVIDER_ID>3</PRODUCT_PROVIDER_ID><PRICING_PLAN_ID>4</PRICING_PLAN_ID><INTEGRAL_PRICING_PLAN_ID>5</INTEGRAL_PRICING_PLAN_ID><PRODUCT_FAMILY_ID>6</PRODUCT_FAMILY_ID><PRODUCT_NAME>tydic</PRODUCT_NAME><PRODUCT_COMMENTS>8</PRODUCT_COMMENTS><PRODUCT_TYPE>9</PRODUCT_TYPE><PRODUCT_CLASS>10</PRODUCT_CLASS><PRODUCT_CODE>11</PRODUCT_CODE><STATE>12</STATE><EFF_DATE>13</EFF_DATE><EXP_DATE>14</EXP_DATE></NEW></PRODUCT></REGULATION_DOMAIN>" ;
    //DcParseXml* xml = new  DcParseXml();
    //xml->parseXmlData(str) ;

    while(1)
    {
        sleep(1000000);
    }

    manager->Destroy();
    DCAceLogAgentCmn::getInstance()->Release();

    return 0;
}

