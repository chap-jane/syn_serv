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
#ifndef _DC_THREAD_PROCESS_H_
#define _DC_THREAD_PROCESS_H_

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

#include <iconv.h>
#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"


#include <rmdb.h>

//class CDCCEntry;
class DcManager;


#define XML_BUF_SZIE 1024


class DcThreadProcess : public TThread
{
public:

    /****************************************************************************************
    *@input
            pDCCEntry : DCC ʵ��

    *@output

    *@return

    *@description       ���캯��
    ******************************************************************************************/

    DcThreadProcess(DcManager *pDcManager, CDCCEntry *pDCCEntry, DcConfig* pConfig,unsigned int i);

    ~DcThreadProcess();

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

    *@description   ���ûỰID
    ******************************************************************************************/
    char* SetSessionID(const DCC_BODY &msgBody);



public :


    /******************************************************************************************/
    int parseXmlData(const std::string& xmlData,t_dcc &t_info,bool &bIsOcsUser, bool &bNewHaveServAttr) ;


    /****************************************************************************************
    *@input


    *@output

    *@return

    *@description   ��շ���Ļ���
    ******************************************************************************************/
    int clear() ;


    /****************************************************************************************
    *@input
          xmlData

    *@output
      t_info
    *@return

    *@description ִ�н���XML�ļ�SQL���
    ******************************************************************************************/
    int executeSql(string strPartitionId,string strServId, bool bIsUpdateServ,string strSql) ;


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   ��ȡ���Ͷ���
    ******************************************************************************************/
    int getDataType() ; //��ȡ�����������

    /****************************************************************************************
    *@input
            tTableAndFlag t,lst_fnav l
    *@output

    *@return

    *@description   ��װSQL
    ******************************************************************************************/
    int packageSql(tTableAndFlag t,lst_fnav l,string &strPartitioinId,string &strServId,string &strSql, bool bHaveServOld = false) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   ��ȡһ��ʵ��
    ******************************************************************************************/
    //  static DcParseXml* GetInstance();


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description  //���빤����
    ******************************************************************************************/

    int InsertWorkOrder(work_order_t &t) ;
    int insertErrRecord(char* sessId) ;


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   ��ʼ�����ݿ�����
    ******************************************************************************************/
    int Init() ;

    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  ����SESSION��
    ******************************************************************************************/
    int InsertSession(string & t, string & org_id) ;


    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  ���ݿ��ύ��ع�
    ******************************************************************************************/
    void Commit() ;
    void Rollback() ;

    /****************************************************************************************/

    /****************************************************************************************
    *@input
       fromcode
    *@output

    *@return

    *@description �ַ�����ת�� UTF8 ת��ΪGDK
    ******************************************************************************************/
    int code_convert(const char *fromcode, const char* tocode, char* inbuf, char* poutbuf);

    /****************************************************************************************
    *@input
             fromcode
    *@output

    *@return

    *@description   �ַ�����ת��UTF8 ת��ΪGDK
    ******************************************************************************************/
    int UTF82GBK(char * buf, char *outbuf) ;


    /****************************************************************************************
    *@input
         sessionId,tabName,pFiles,pValues
    *@output

    *@return

    *@description    ���뷴��ͬ���Ľӿڱ��SESSION��Ӧ��ϵ��
    ******************************************************************************************/
    int setResynInerfaceRel(char* sessionId,char* tabName ,char* pFiles,char* pValues) ;

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

    *@description   ���¼���ӿ�b��
    ******************************************************************************************/
    //int setTifServActivationB(char* pKey,char* flag) ;

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

    /****************************************************************************************
    *@input
             �������ֶ������ֶ�ֵ
    *@output
    strKeyInfo   ������Ϣ
    *@return

    *@description   �жϵ�ǰ�ֶ��Ƿ�����
    ******************************************************************************************/
	int getTablePrimaryKeyString(char *szTableName, char *szFieldName, char *szFieldValue, string &strKeyInfo);

    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   �ж��Ƿ�������ͻ����
    ******************************************************************************************/
	bool checkInsertError(const char* pszErrorInfo);
	
    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   ������ɾ����¼
    ******************************************************************************************/
	int deleteRecordByKey(char* pszSql);

    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   ��serv_idɾ��serv��¼
    ******************************************************************************************/
	int deleteServByServId(int nPartitionId,long lnServId);

    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   �ж��Ƿ�OCS�û���IsOCSUser=true��ocs�û���false����
    ******************************************************************************************/
	int getServAttr(long lnServId, long lnAttrId, char *szAttrVal, bool &bIsOCSUser);

private:
    DcManager*          m_pDcManager;            //DCC
    CDCCEntry*           m_pDCCEntry;
    unsigned int        m_threadCount;      //�߳����
    DcConfig*           m_scConfig;             //������Ϣ

    char m_errBuf[48*XML_BUF_SZIE] ;



private :

    xmlRecord     m_xml ;  //һ���ļ�¼�ֶ�
    map<string,string> m_dataType ;
    work_order_t m_t ;
    lst_fnav       l_list ;
    tTableAndFlag  t_st ;
    tFNameAndValue t_fn ;
    //char m_sql[2048]  ;   ///ִ�е�SQL
    char m_info[2048] ;   ///��ӡ��־��
    work_order_t work_t ;
    int HssMsgDumpFlag;
	int  m_nReserveOCSAndHBData;   //0-��ȫ�����ݣ�1-ACCT��CUST��ȫ�����ݣ�������ֻ��OCS�������ݣ���0��1Ĭ��0
    int  m_nServNeedCombine;        //�Ƿ���Ҫ�ϲ�SERV��delete��insertΪupdate:0-���ϲ���1-�ϲ���Ĭ�ϲ��ϲ�

    long m_ExecHssSQLTime;//ִ��xml�ļ��е�sqlʱ��
    long m_ExecHssSQLTimes; //ִ��sql�Ĵ���

	map<string, int> m_mapServKey;
	map<string, int> m_mapServParam; //SERV����ֶμ�����
	map<string, int> m_mapServAttrKey;
	map<string, int> m_mapServLocationKey;
	map<string, int> m_mapAServExKey;
public :
    DCRMDBConnection* pCon ;
    DCRMDBDML*        pDML ;
    DCRMDBDQL*        pDQL ;

    DCRMDBDQL*        ptfDQL[2] ;
    DCRMDBDML*        ptfDML[9] ;
	DCRMDBDML*        ptfServDML[10] ;

};
#endif
