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
            pDCCEntry : DCC 实例

    *@output

    *@return

    *@description       构造函数
    ******************************************************************************************/

    DcThreadProcess(DcManager *pDcManager, CDCCEntry *pDCCEntry, DcConfig* pConfig,unsigned int i);

    ~DcThreadProcess();

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

    *@description   设置会话ID
    ******************************************************************************************/
    char* SetSessionID(const DCC_BODY &msgBody);



public :


    /******************************************************************************************/
    int parseXmlData(const std::string& xmlData,t_dcc &t_info,bool &bIsOcsUser, bool &bNewHaveServAttr) ;


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
    int executeSql(string strPartitionId,string strServId, bool bIsUpdateServ,string strSql) ;


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
    int packageSql(tTableAndFlag t,lst_fnav l,string &strPartitioinId,string &strServId,string &strSql, bool bHaveServOld = false) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description   获取一个实例
    ******************************************************************************************/
    //  static DcParseXml* GetInstance();


    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description  //插入工单表
    ******************************************************************************************/

    int InsertWorkOrder(work_order_t &t) ;
    int insertErrRecord(char* sessId) ;


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
    int InsertSession(string & t, string & org_id) ;


    /****************************************************************************************
    *@input
            t
    *@output

    *@return

    *@description  数据库提交与回滚
    ******************************************************************************************/
    void Commit() ;
    void Rollback() ;

    /****************************************************************************************/

    /****************************************************************************************
    *@input
       fromcode
    *@output

    *@return

    *@description 字符编码转换 UTF8 转换为GDK
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


    /****************************************************************************************
    *@input
         sessionId,tabName,pFiles,pValues
    *@output

    *@return

    *@description    插入反向同步的接口表和SESSION对应关系表
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
             sessionId
    *@output
    re_syn_interface_rel_t
    *@return

    *@description   查询反向同步的接口表和SESSION对应关系表
    ******************************************************************************************/
    int getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId) ;

    /****************************************************************************************
    *@input
             表名、字段名、字段值
    *@output
    strKeyInfo   主键信息
    *@return

    *@description   判断当前字段是否主键
    ******************************************************************************************/
	int getTablePrimaryKeyString(char *szTableName, char *szFieldName, char *szFieldValue, string &strKeyInfo);

    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   判断是否主键冲突错误
    ******************************************************************************************/
	bool checkInsertError(const char* pszErrorInfo);
	
    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   绑定主键删除记录
    ******************************************************************************************/
	int deleteRecordByKey(char* pszSql);

    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   绑定serv_id删除serv记录
    ******************************************************************************************/
	int deleteServByServId(int nPartitionId,long lnServId);

    /****************************************************************************************
    *@input
             
    *@output

    *@return

    *@description   判断是否OCS用户，IsOCSUser=true是ocs用户，false不是
    ******************************************************************************************/
	int getServAttr(long lnServId, long lnAttrId, char *szAttrVal, bool &bIsOCSUser);

private:
    DcManager*          m_pDcManager;            //DCC
    CDCCEntry*           m_pDCCEntry;
    unsigned int        m_threadCount;      //线程序号
    DcConfig*           m_scConfig;             //配置信息

    char m_errBuf[48*XML_BUF_SZIE] ;



private :

    xmlRecord     m_xml ;  //一条的记录字段
    map<string,string> m_dataType ;
    work_order_t m_t ;
    lst_fnav       l_list ;
    tTableAndFlag  t_st ;
    tFNameAndValue t_fn ;
    //char m_sql[2048]  ;   ///执行的SQL
    char m_info[2048] ;   ///打印日志用
    work_order_t work_t ;
    int HssMsgDumpFlag;
	int  m_nReserveOCSAndHBData;   //0-存全网数据；1-ACCT、CUST存全网数据，其他表只存OCS所有数据；非0非1默认0
    int  m_nServNeedCombine;        //是否需要合并SERV的delete和insert为update:0-不合并；1-合并；默认不合并

    long m_ExecHssSQLTime;//执行xml文件中的sql时间
    long m_ExecHssSQLTimes; //执行sql的次数

	map<string, int> m_mapServKey;
	map<string, int> m_mapServParam; //SERV表的字段及类型
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
