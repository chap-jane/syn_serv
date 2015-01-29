/*******************************************
*Copyrights  2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcConfig.h
*Indentifier：
*
*Description：
*       配置文件类
*Version：
*       V1.0
*Author:
*       yancy
*Finished：
*       2011年12月20日
*History:
*       2011/12/20  V1.0 文件创建
********************************************/

#ifndef _DC_CONFIG_H_
#define _DC_CONFIG_H_

#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"
#include "pub_common/DCPrepareSql.h"
#include <string>

class DcConfig
{

public:

    /*构造及析构 函数 */
    DcConfig();
    ~DcConfig();

public:

    static DcConfig* GetInstance(); /*得到一个实例 提供给外部接口*/

    /*初始化 获取配置信息*/
    int Init();

    /*释放资源*/
    int Destroy();

    //int LoadAVPDef();

public:

    static DcConfig* m_instance;
    int   m_threadCount;                /*线程数*/
    int   m_reConIntervalTime ;       /*重连间隔时间*/
    int   m_serviceNo;                /*DCC端口号*/
    char m_dbUser[20] ;                  /*数据用户名*/
    char m_dbPass[30] ;                  /*数据密码*/
    char m_dbSerName[20] ;               /*数据服务名*/
    char OCSOracleString[128] ;                         //ods30_dx/ods30_dx6@tydic_5
    char m_logFullPath[100] ;             /*日志路径*/
    char m_fileProFlag[4]   ;             //文件处理标识
    char m_bakPath[100] ;                //文件处理的时备份目录
    char m_errPath[100] ;                //文件处理时错误记录目录
    char m_invPath[100] ;                //文件处理时的无效目录
    int  m_nHssDumpFlag;//答应日志的开关
    int  m_nReserveOCSAndHBData;   //0-存全网数据；1-ACCT、CUST存全网数据，其他表只存OCS所有数据；非0非1默认0
    int  m_nServNeedCombine;        //是否需要合并SERV的delete和insert为update:0-不合并；1-合并；默认不合并

    //DCPrepareSql *m_sqlPrepare;

};
#endif
