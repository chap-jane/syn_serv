/*******************************************
*Copyrights   2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcParseXml.h
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

    *@description   解析DCC Data-Table-Simple-Row字符串
    ******************************************************************************************/
    int parseXmlData(const std::string& xmlData,t_dcc &t_info) ;


    /****************************************************************************************
    *@input


    *@output

    *@return

    *@description   清空分配的缓存
    ******************************************************************************************/
    int clear() ;


    /****************************************************************************************
    *@input
          xmlData

    *@output
      t_info
    *@return

    *@description 执行解析XML文件SQL语句
    ******************************************************************************************/
    int executeSql() ;


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   获取类型定义
    ******************************************************************************************/
    int getDataType() ; //获取表的数据类型

    /****************************************************************************************
    *@input
            tTableAndFlag t,lst_fnav l
    *@output

    *@return

    *@description   组装SQL
    ******************************************************************************************/
    int packageSql(tTableAndFlag t,lst_fnav l) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   获取一个实例
    ******************************************************************************************/
    static DcParseXml* GetInstance();


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description  //插入工单表
    ******************************************************************************************/

    int InsertWorkOrder(work_order_t &t) ;


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   初始化数据库连接
    ******************************************************************************************/
    int Init() ;

    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  插入SESSION表
    ******************************************************************************************/
    int InsertSession(string & t) ;


    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  数据库提交与回滚
    ******************************************************************************************/
    void Commit() ;
    void Rollback() ;

    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  得到过户接口数据
    ******************************************************************************************/
    int getTransfer(syn_transfer_t& t) ;


    /****************************************************************************************
    *@input
             userId
    *@output

    *@return

    *@description   更新过户数据状态
    ******************************************************************************************/
    int updateTransfer(char* flag,char* userId) ;


    /****************************************************************************************
    *@input
             tif_serv_activation_a_t t
    *@output
     tif_serv_activation_a_t& t
    *@return

    *@description   获取激活接口A表
    ******************************************************************************************/
    int getTifServActivationA(tif_serv_activation_a_t& t) ;


    /****************************************************************************************
    *@input
             tif_serv_activation_a_t t
    *@output
     tif_serv_activation_a_t& t
    *@return

    *@description   获取激活接口B表
    ******************************************************************************************/
    //  int getTifServActivationB(tif_serv_activation_b_t& t) ;



    /****************************************************************************************
    *@input
             cdma_stop_open_interface_t& t
    *@output
     cdma_stop_open_interface_t& t
    *@return

    *@description       获取停复机接口表
    ******************************************************************************************/
    int getCdmaStopOpenInterface(cdma_stop_open_interface_t& t) ;


    /****************************************************************************************
    *@input
             tb_bil_trigger_record_t& t
    *@output
     tb_bil_trigger_record_t& t
    *@return

    *@description   获取上网卡接口表
    ******************************************************************************************/
    int getTbBilTriggerRecord(tb_bil_trigger_record_t& t) ;


    /****************************************************************************************
    *@input
             sessionId
    *@output
    re_syn_interface_rel_t
    *@return

    *@description   查询反向同步的接口表和SESSION对应关系表
    ******************************************************************************************/
    int getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId) ;


    /****************************************************************************************
    *@input
             servid
    *@output
    serv_t
    *@return

    *@description   获取用户表
    ******************************************************************************************/
    int getServ(serv_t& t , char* servId) ;


    /****************************************************************************************
    *@input
             servid
    *@output
    serv_t
    *@return

    *@description   获取用户表
    ******************************************************************************************/
    int getServStateAttr(serv_state_attr_t& t , char* servId,char* state) ;



    /****************************************************************************************
    *@input
             attrId,servId
    *@output
    serv_t
    *@return

    *@description   获取用户表属性表的数据
    ******************************************************************************************/
    int getServAttr(serv_attr_t& t , char* servId, char* attrId) ;

    /****************************************************************************************
    *@input
         attrId,servId
    *@output
    serv_t
    *@return

    *@description    得到用户的位置
    ******************************************************************************************/
    int getServLocation(serv_location_t& t ,char* servId) ;


    int getOrg(org_t& t,char* exchangeId) ;

    /****************************************************************************************
    *@input
             sessionId,tabName,pFiles,pValues
    *@output

    *@return

    *@description   插入反向同步的接口表和SESSION对应关系表
    ******************************************************************************************/
    int setResynInerfaceRel(char* sessionId,char* tabName ,char* pFiles,char* pValues) ;

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

    *@description   更新激活接口b表
    ******************************************************************************************/
    //int setTifServActivationB(char* pKey,char* flag) ;

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
             fromcode
    *@output

    *@return

    *@description   字符编码转换 UTF8 转换为GDK
    ******************************************************************************************/
    int code_convert(const char *fromcode, const char* tocode, char* inbuf, char* poutbuf);

    /****************************************************************************************
    *@input
             fromcode
    *@output

    *@return

    *@description   字符编码转换UTF8 转换为GDK
    ******************************************************************************************/
    int UTF82GBK(char * buf, char *outbuf) ;



public:
    xmlRecord     m_xml ;  //一条的记录字段
    map<string,string> m_dataType ;
    static DcParseXml* m_instance ;
    work_order_t m_t ;
public :
    lst_fnav       l_list ;
    tTableAndFlag  t_st ;
    tFNameAndValue t_fn ;
    char m_sql[2048]  ;   ///执行的SQL
    char m_info[2048] ;   ///打印日志用
    DcConfig* m_config ;
public :
    DCRMDBConnection* pCon ;
    DCRMDBDML*        pDML ;
    DCRMDBDQL*        pDQL ;

    ///过户使用/激活/停复机/上网卡
    DCRMDBDQL*        ptfDQL[15] ;
    DCRMDBDML*        ptfDML[15] ;

    //char utf8Buf[65536];
    //char gbkBuf[65536] ;

};
#endif
