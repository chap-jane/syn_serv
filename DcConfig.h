/*******************************************
*Copyrights  2011��������Դ�ϿƼ�������޹�˾
*                   ����ƽ̨��Ŀ��
*All rights reserved.
*
*Filename��
*       DcConfig.h
*Indentifier��
*
*Description��
*       �����ļ���
*Version��
*       V1.0
*Author:
*       yancy
*Finished��
*       2011��12��20��
*History:
*       2011/12/20  V1.0 �ļ�����
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

    /*���켰���� ���� */
    DcConfig();
    ~DcConfig();

public:

    static DcConfig* GetInstance(); /*�õ�һ��ʵ�� �ṩ���ⲿ�ӿ�*/

    /*��ʼ�� ��ȡ������Ϣ*/
    int Init();

    /*�ͷ���Դ*/
    int Destroy();

    //int LoadAVPDef();

public:

    static DcConfig* m_instance;
    int   m_threadCount;                /*�߳���*/
    int   m_reConIntervalTime ;       /*�������ʱ��*/
    int   m_serviceNo;                /*DCC�˿ں�*/
    char m_dbUser[20] ;                  /*�����û���*/
    char m_dbPass[30] ;                  /*��������*/
    char m_dbSerName[20] ;               /*���ݷ�����*/
    char OCSOracleString[128] ;                         //ods30_dx/ods30_dx6@tydic_5
    char m_logFullPath[100] ;             /*��־·��*/
    char m_fileProFlag[4]   ;             //�ļ������ʶ
    char m_bakPath[100] ;                //�ļ������ʱ����Ŀ¼
    char m_errPath[100] ;                //�ļ�����ʱ�����¼Ŀ¼
    char m_invPath[100] ;                //�ļ�����ʱ����ЧĿ¼
    int  m_nHssDumpFlag;//��Ӧ��־�Ŀ���
    int  m_nReserveOCSAndHBData;   //0-��ȫ�����ݣ�1-ACCT��CUST��ȫ�����ݣ�������ֻ��OCS�������ݣ���0��1Ĭ��0
    int  m_nServNeedCombine;        //�Ƿ���Ҫ�ϲ�SERV��delete��insertΪupdate:0-���ϲ���1-�ϲ���Ĭ�ϲ��ϲ�

    //DCPrepareSql *m_sqlPrepare;

};
#endif
