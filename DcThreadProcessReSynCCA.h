/*******************************************
*Copyrights   2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcThreadProcess.h
*Indentifier：
*
*Description：
*       同步线程处理类
*Version：
*       V1.0
*Author:
*       yancy
*Finished：
*       2012年2月20日
*History:
*       2012/2/20  V1.0 文件创建
********************************************/
#ifndef _DC_THREAD_PROCESS_RE_SYN_CCA_H_
#define _DC_THREAD_PROCESS_RE_SYN_CCA_H_

#include "tinyxml.h"
#include "tinystr.h"



#include "TThread.h"
#include "dccType.h"
#include "dccDef.h"
#include "CValue.h"
#include "CDCCMsg.h"
#include "CDCCEntry.h"
#include "CPackage.h"

#include "syn_struct.h"
#include "DcConfig.h"
#include "DcDefineCode.h"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <list>
#include <map>
#include <vector>

#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"


#include <rmdb.h>


#define XML_BUF_SZIE 1024

class DcManager;



class DcThreadProcessReSynCCA : public TThread
{
public:

    /****************************************************************************************
    *@input
            pDCCEntry : DCC 实例

    *@output

    *@return

    *@description       构造函数
    ******************************************************************************************/

     DcThreadProcessReSynCCA(CDCCEntry *pDccEntry,DcManager *pDcManager, DcConfig* pConfig);

    ~DcThreadProcessReSynCCA();

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description       执行操作
    ******************************************************************************************/
    int routine();





    /****************************************************************************************
    *@input
        msgBody : 需路由的消息对象
        code : AVP 码

    *@output

    *@return
        AVP 值指针

    *@description        获取消息体中的AVP 值
    ******************************************************************************************/
    CValue* GetMsgAVP(const DCC_BODY &msgBody, unsigned int code);





    /****************************************************************************************
    *@input
    msg:DCC消息

    *@output

    *@return

    *@description     处理HSS回复的CCA
    ******************************************************************************************/
    int DealWithCCA(CDCCMsg &msg);


public :




    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   获取类型定义
    ******************************************************************************************/
    //int getDataType() ; //获取表的数据类型



    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   初始化数据库连接
    ******************************************************************************************/
    int Init() ;


    /****************************************************************************************
    *@input
             pkey
    *@output

    *@return

    *@description   更新激活接口A表
    ******************************************************************************************/
    int setTifServActivationA(char* pKey,char* flag) ;

    /****************************************************************************************
    *@input
             pkey
    *@output

    *@return

    *@description   更新停复机接口表
    ******************************************************************************************/
    int setCdmaStopOpenInterface(char* pKey,char* flag) ;

    /****************************************************************************************
    *@input
             pkey
    *@output

    *@return

    *@description   更新上网卡接口表
    ******************************************************************************************/
    int setTbBilTriggerRecord(char* pKey,char* flag) ;



    /****************************************************************************************
    *@input
             sessionId
    *@output
    re_syn_interface_rel_t
    *@return

    *@description   查询反向同步的接口表和SESSION对应关系表
    ******************************************************************************************/
    int getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId) ;

private:
    DcManager*          m_pDcManager;            //DCC
    DcConfig*           m_scConfig;             //配置信息
private :

    //map<string,string> m_dataType ;
    char m_sql[2048]  ;   ///执行的SQL
    char m_info[2048] ;   ///打印日志用

public :
    DCRMDBConnection* pCon ;
    CDCCEntry*   m_DccEntry;
    DCRMDBDML*        pDML ;
    DCRMDBDQL*        pDQL ;

    DCRMDBDQL*        ptfDQL[1] ;
    DCRMDBDML*        ptfDML[9] ;

};
#endif


