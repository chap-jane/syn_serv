/*******************************************
*Copyrights   2011��������Դ�ϿƼ�������޹�˾
*                   ����ƽ̨��Ŀ��
*All rights reserved.
*
*Filename��
*       DcParseXml.h
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

#ifndef _DC_PARSEXMLDOCMENT_H_
#define _DC_PARSEXMLDOCMENT_H_

#include <stdio.h>
#include <unistd.h>


#include <string.h>
#include <map>
#include <rmdb.h>
#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"
#include "syn_struct.h"
#include "DcConfig.h"




class DcParseXml
{
public :
    DcParseXml() ;

    ~DcParseXml() ;
public:
    /****************************************************************************************
    *@input
            xmlData

    *@output
    t_info
    *@return

    *@description   ����DCC Data-Table-Simple-Row�ַ���
    ******************************************************************************************/
    int parseXmlData(const std::string& xmlData,t_dcc &t_info) ;


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
    int executeSql() ;


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
    int packageSql(tTableAndFlag t,lst_fnav l) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   ��ȡһ��ʵ��
    ******************************************************************************************/
    static DcParseXml* GetInstance();


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description  //���빤����
    ******************************************************************************************/

    int InsertWorkOrder(work_order_t &t) ;


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
    int InsertSession(string & t) ;


    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  ���ݿ��ύ��ع�
    ******************************************************************************************/
    void Commit() ;
    void Rollback() ;

    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  �õ������ӿ�����
    ******************************************************************************************/
    int getTransfer(syn_transfer_t& t) ;


    /****************************************************************************************
    *@input
             userId
    *@output

    *@return

    *@description   ���¹�������״̬
    ******************************************************************************************/
    int updateTransfer(char* flag,char* userId) ;


    /****************************************************************************************
    *@input
             tif_serv_activation_a_t t
    *@output
     tif_serv_activation_a_t& t
    *@return

    *@description   ��ȡ����ӿ�A��
    ******************************************************************************************/
    int getTifServActivationA(tif_serv_activation_a_t& t) ;


    /****************************************************************************************
    *@input
             tif_serv_activation_a_t t
    *@output
     tif_serv_activation_a_t& t
    *@return

    *@description   ��ȡ����ӿ�B��
    ******************************************************************************************/
    //  int getTifServActivationB(tif_serv_activation_b_t& t) ;



    /****************************************************************************************
    *@input
             cdma_stop_open_interface_t& t
    *@output
     cdma_stop_open_interface_t& t
    *@return

    *@description       ��ȡͣ�����ӿڱ�
    ******************************************************************************************/
    int getCdmaStopOpenInterface(cdma_stop_open_interface_t& t) ;


    /****************************************************************************************
    *@input
             tb_bil_trigger_record_t& t
    *@output
     tb_bil_trigger_record_t& t
    *@return

    *@description   ��ȡ�������ӿڱ�
    ******************************************************************************************/
    int getTbBilTriggerRecord(tb_bil_trigger_record_t& t) ;


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
             servid
    *@output
    serv_t
    *@return

    *@description   ��ȡ�û���
    ******************************************************************************************/
    int getServ(serv_t& t , char* servId) ;


    /****************************************************************************************
    *@input
             servid
    *@output
    serv_t
    *@return

    *@description   ��ȡ�û���
    ******************************************************************************************/
    int getServStateAttr(serv_state_attr_t& t , char* servId,char* state) ;



    /****************************************************************************************
    *@input
             attrId,servId
    *@output
    serv_t
    *@return

    *@description   ��ȡ�û������Ա������
    ******************************************************************************************/
    int getServAttr(serv_attr_t& t , char* servId, char* attrId) ;

    /****************************************************************************************
    *@input
         attrId,servId
    *@output
    serv_t
    *@return

    *@description    �õ��û���λ��
    ******************************************************************************************/
    int getServLocation(serv_location_t& t ,char* servId) ;


    int getOrg(org_t& t,char* exchangeId) ;

    /****************************************************************************************
    *@input
             sessionId,tabName,pFiles,pValues
    *@output

    *@return

    *@description   ���뷴��ͬ���Ľӿڱ��SESSION��Ӧ��ϵ��
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
             fromcode
    *@output

    *@return

    *@description   �ַ�����ת�� UTF8 ת��ΪGDK
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



public:
    xmlRecord     m_xml ;  //һ���ļ�¼�ֶ�
    map<string,string> m_dataType ;
    static DcParseXml* m_instance ;
    work_order_t m_t ;
public :
    lst_fnav       l_list ;
    tTableAndFlag  t_st ;
    tFNameAndValue t_fn ;
    char m_sql[2048]  ;   ///ִ�е�SQL
    char m_info[2048] ;   ///��ӡ��־��
    DcConfig* m_config ;
public :
    DCRMDBConnection* pCon ;
    DCRMDBDML*        pDML ;
    DCRMDBDQL*        pDQL ;

    ///����ʹ��/����/ͣ����/������
    DCRMDBDQL*        ptfDQL[15] ;
    DCRMDBDML*        ptfDML[15] ;

    //char utf8Buf[65536];
    //char gbkBuf[65536] ;

};
#endif
