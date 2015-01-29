/*******************************************
*Copyrights   2011��������Դ�ϿƼ�������޹�˾
*                   ����ƽ̨��Ŀ��
*All rights reserved.
*
*Filename��
*       DcThreadProcess.h
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
            pDCCEntry : DCC ʵ��

    *@output

    *@return

    *@description       ���캯��
    ******************************************************************************************/

     DcThreadProcessReSynCCA(CDCCEntry *pDccEntry,DcManager *pDcManager, DcConfig* pConfig);

    ~DcThreadProcessReSynCCA();

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description       ִ�в���
    ******************************************************************************************/
    int routine();





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
    msg:DCC��Ϣ

    *@output

    *@return

    *@description     ����HSS�ظ���CCA
    ******************************************************************************************/
    int DealWithCCA(CDCCMsg &msg);


public :




    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   ��ȡ���Ͷ���
    ******************************************************************************************/
    //int getDataType() ; //��ȡ�����������



    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   ��ʼ�����ݿ�����
    ******************************************************************************************/
    int Init() ;


    /****************************************************************************************
    *@input
             pkey
    *@output

    *@return

    *@description   ���¼���ӿ�A��
    ******************************************************************************************/
    int setTifServActivationA(char* pKey,char* flag) ;

    /****************************************************************************************
    *@input
             pkey
    *@output

    *@return

    *@description   ����ͣ�����ӿڱ�
    ******************************************************************************************/
    int setCdmaStopOpenInterface(char* pKey,char* flag) ;

    /****************************************************************************************
    *@input
             pkey
    *@output

    *@return

    *@description   �����������ӿڱ�
    ******************************************************************************************/
    int setTbBilTriggerRecord(char* pKey,char* flag) ;



    /****************************************************************************************
    *@input
             sessionId
    *@output
    re_syn_interface_rel_t
    *@return

    *@description   ��ѯ����ͬ���Ľӿڱ��SESSION��Ӧ��ϵ��
    ******************************************************************************************/
    int getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId) ;

private:
    DcManager*          m_pDcManager;            //DCC
    DcConfig*           m_scConfig;             //������Ϣ
private :

    //map<string,string> m_dataType ;
    char m_sql[2048]  ;   ///ִ�е�SQL
    char m_info[2048] ;   ///��ӡ��־��

public :
    DCRMDBConnection* pCon ;
    CDCCEntry*   m_DccEntry;
    DCRMDBDML*        pDML ;
    DCRMDBDQL*        pDQL ;

    DCRMDBDQL*        ptfDQL[1] ;
    DCRMDBDML*        ptfDML[9] ;

};
#endif


