/*******************************************
*Copyrights   2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcThreadProcessReSyn.h
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
#ifndef _DC_THREAD_PROCESS_RESYN_H_
#define _DC_THREAD_PROCESS_RESYN_H_

#include <stdio.h>
#include <string.h>
#include <list>
#include <vector>
#include "TThread.h"


#include "dccType.h"
#include "dccDef.h"
#include "CValue.h"
#include "CDCCMsg.h"
#include "CDCCEntry.h"
#include "CPackage.h"

#include "DcParseXml.h"
#include "syn_struct.h"
#include "DcConfig.h"
#include "DcDefineCode.h"

#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"




#define XML_BUF_SZIE 1024

class CDCCEntry;
class DcThreadProcessReSyn : public TThread
{
public:

    /****************************************************************************************
    *@input
            pDCCEntry : DCC 实例

    *@output

    *@return

    *@description       构造函数
    ******************************************************************************************/

    DcThreadProcessReSyn(CDCCEntry *pDCCEntry, DcConfig* pConfig);

    ~DcThreadProcessReSyn();

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description       执行操作
    ******************************************************************************************/
    int routine();

    int Init();



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
            msgBody : 需路由的消息对象
            code : AVP 码

    *@output

    *@return
            AVP 值指针

    *@description       获取service_infomation组信息
    ******************************************************************************************/
    const DCC_BODY& GetMsgGroup(const DCC_BODY &msgBody ,unsigned int code) ;


    /****************************************************************************************
        *@input
                msgBody : 需路由的消息对象
                code : AVP 码

        *@output

        *@return
                AVP 值指针

        *@description       SetMsgAVP 设置AVP
        ******************************************************************************************/
    int SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code,char* str) ;

    int SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code, int i) ;
    
    int SetMsgFull(int x) ;
    
	  int GetMsgFull() ;

    /****************************************************************************************
    *@input
            msg:DCC消息

    *@output

    *@return

    *@description       CCR处理失败返回错误消息
    ******************************************************************************************/
    int SendResultCCA(CDCCMsg &msg, int error);

    /****************************************************************************************
    *@input
    msg:DCC消息

    *@output

    *@return

    *@description     处理HSS回复的CCA
    ******************************************************************************************/
    int DealWithCCA(CDCCMsg &msg);


    /****************************************************************************************
    *@input
            msg:DCC消息

    *@output

    *@return

    *@description       CCR处理失败返回错误消息
    ******************************************************************************************/
    int SendTransferCCR(syn_transfer_t& t,char* orgId) ;

    /****************************************************************************************
    *@input
            msg:DCC消息

    *@output

    *@return

    *@description   反向同步发送激活/上网卡/停复机
    ******************************************************************************************/
    int  SendReSynCCR(char* xml ,char* actionId,char* orgId,char* sessionId) ;

    /****************************************************************************************
    *@input
            msg:DCC消息

    *@output

    *@return

    *@description   设置会话ID
    ******************************************************************************************/
    int SetSessionID(const DCC_BODY &msgBody ,char* sessionId);

    /****************************************************************************************
    *@input
            msg:DCC消息

    *@output

    *@return

    *@description   反向同步,serv表 组装XML文件
    ******************************************************************************************/
    int ComposeXmlByServ(serv_t t,serv_state_attr_t s,serv_state_attr_t n,char* pState,char* nState,char* create_date,char* xml);

    /****************************************************************************************
    *@input
            msg:DCC消息

    *@output

    *@return

    *@description   反向同步,serv表 组装XML文件
    ******************************************************************************************/
    int ComposeXmlByServAttr(serv_attr_t t ,char* effDate,char* expDate,char* xml);

    /****************************************************************************************
        *@input
                msg:DCC消息

        *@output

        *@return

        *@description   反向同步函数
        ******************************************************************************************/
    int ReSynchronization();    
private:

    CDCCEntry*          m_pDCCEntry;            //DCC
    unsigned int        m_threadCount;      //线程序号
    DcConfig*           m_scConfig;             //配置信息
    char m_errBuf[48*XML_BUF_SZIE] ;
    //string m_errBuf ;
    char           m_session_id[64] ;
    DcParseXml*     m_pXml ;          //
};
#endif
