/*******************************************
*Copyrights   2011��������Դ�ϿƼ�������޹�˾
*                   ����ƽ̨��Ŀ��
*All rights reserved.
*
*Filename��
*       DcThreadProcessReSyn.h
*Indentifier��
*
*Description��
*       ͬ���̴߳�����
*Version��
*       V1.0
*Author:
*       yancy
*Finished��
*       2012��2��20��
*History:
*       2012/2/20  V1.0 �ļ�����
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
            pDCCEntry : DCC ʵ��

    *@output

    *@return

    *@description       ���캯��
    ******************************************************************************************/

    DcThreadProcessReSyn(CDCCEntry *pDCCEntry, DcConfig* pConfig);

    ~DcThreadProcessReSyn();

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description       ִ�в���
    ******************************************************************************************/
    int routine();

    int Init();



    /****************************************************************************************
    *@input
        msgBody : ��·�ɵ���Ϣ����
        code : AVP ��

    *@output

    *@return
        AVP ֵָ��

    *@description        ��ȡ��Ϣ���е�AVP ֵ
    ******************************************************************************************/
    CValue* GetMsgAVP(const DCC_BODY &msgBody, unsigned int code);



    /****************************************************************************************
    *@input
            msgBody : ��·�ɵ���Ϣ����
            code : AVP ��

    *@output

    *@return
            AVP ֵָ��

    *@description       ��ȡservice_infomation����Ϣ
    ******************************************************************************************/
    const DCC_BODY& GetMsgGroup(const DCC_BODY &msgBody ,unsigned int code) ;


    /****************************************************************************************
        *@input
                msgBody : ��·�ɵ���Ϣ����
                code : AVP ��

        *@output

        *@return
                AVP ֵָ��

        *@description       SetMsgAVP ����AVP
        ******************************************************************************************/
    int SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code,char* str) ;

    int SetMsgAVP(const DCC_BODY &msgBody ,unsigned int code, int i) ;
    
    int SetMsgFull(int x) ;
    
	  int GetMsgFull() ;

    /****************************************************************************************
    *@input
            msg:DCC��Ϣ

    *@output

    *@return

    *@description       CCR����ʧ�ܷ��ش�����Ϣ
    ******************************************************************************************/
    int SendResultCCA(CDCCMsg &msg, int error);

    /****************************************************************************************
    *@input
    msg:DCC��Ϣ

    *@output

    *@return

    *@description     ����HSS�ظ���CCA
    ******************************************************************************************/
    int DealWithCCA(CDCCMsg &msg);


    /****************************************************************************************
    *@input
            msg:DCC��Ϣ

    *@output

    *@return

    *@description       CCR����ʧ�ܷ��ش�����Ϣ
    ******************************************************************************************/
    int SendTransferCCR(syn_transfer_t& t,char* orgId) ;

    /****************************************************************************************
    *@input
            msg:DCC��Ϣ

    *@output

    *@return

    *@description   ����ͬ�����ͼ���/������/ͣ����
    ******************************************************************************************/
    int  SendReSynCCR(char* xml ,char* actionId,char* orgId,char* sessionId) ;

    /****************************************************************************************
    *@input
            msg:DCC��Ϣ

    *@output

    *@return

    *@description   ���ûỰID
    ******************************************************************************************/
    int SetSessionID(const DCC_BODY &msgBody ,char* sessionId);

    /****************************************************************************************
    *@input
            msg:DCC��Ϣ

    *@output

    *@return

    *@description   ����ͬ��,serv�� ��װXML�ļ�
    ******************************************************************************************/
    int ComposeXmlByServ(serv_t t,serv_state_attr_t s,serv_state_attr_t n,char* pState,char* nState,char* create_date,char* xml);

    /****************************************************************************************
    *@input
            msg:DCC��Ϣ

    *@output

    *@return

    *@description   ����ͬ��,serv�� ��װXML�ļ�
    ******************************************************************************************/
    int ComposeXmlByServAttr(serv_attr_t t ,char* effDate,char* expDate,char* xml);

    /****************************************************************************************
        *@input
                msg:DCC��Ϣ

        *@output

        *@return

        *@description   ����ͬ������
        ******************************************************************************************/
    int ReSynchronization();    
private:

    CDCCEntry*          m_pDCCEntry;            //DCC
    unsigned int        m_threadCount;      //�߳����
    DcConfig*           m_scConfig;             //������Ϣ
    char m_errBuf[48*XML_BUF_SZIE] ;
    //string m_errBuf ;
    char           m_session_id[64] ;
    DcParseXml*     m_pXml ;          //
};
#endif
