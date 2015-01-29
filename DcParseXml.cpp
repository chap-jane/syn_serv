/*******************************************
*Copyrights   2011，深圳天源迪科计算机有限公司
*                   技术平台项目组
*All rights reserved.
*
*Filename：
*       DcParseXml.cpp
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

///解析XML文件的数据结构
/*
**      <REGULATION_DOMAIN> //根节点
**          <PRODUCT FLAG="1"> //表名及增删改标志
**              <OLD>            //表里已有数据
**                  <PRODUCT_ID>11111</PRODUCT_ID> 字段名及值
**              </OLD>
**              <NEW> //表里没有的数据
**                  <PRODUCT_ID></PRODUCT_ID>
**              </NEW>
**          </PRODUCT>
**     </REGULATION_DOMAIN>
**
**
*/

//#include <stdio.h>
//#include <unistd.h>
#include <iostream>
#include "DcParseXml.h"
#include <list>
#include <iconv.h>
#include "tinyxml.h"
#include "tinystr.h"

using namespace std ;


DcParseXml* DcParseXml::m_instance = NULL ;
DcParseXml* DcParseXml::GetInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new DcParseXml() ;
    }
    return m_instance ;
}

DcParseXml::DcParseXml()
{
    memset(m_sql,0,2048) ;
    m_config =NULL ;
    memset(&m_t,0x00,sizeof(m_t)) ;
    pCon  =NULL;
    pDML = NULL;
    pDQL = NULL;
    for(int i =0 ; i<15 ; i++)
    {
        ptfDQL[i] = NULL ;
        ptfDML[i]=NULL ;
    }
    //docDataTableRow =NULL ;
    m_info[0]='\0' ;
    //memset(utf8Buf,0,sizeof(utf8Buf)) ;
    //memset(gbkBuf,0,sizeof(gbkBuf)) ;
}

DcParseXml::~DcParseXml()
{

    for(int i = 0 ; i<15 ; i++)
    {
        if(ptfDQL[i] !=NULL)
        {
            delete ptfDQL[i] ;
            ptfDQL[i] =NULL ;
        }
        if(ptfDML[i] != NULL)
        {
            delete ptfDML[i] ;
            ptfDML[i] =NULL ;
        }
    }
    if(pCon != NULL)
    {
        delete pCon ;
        pCon = NULL ;
    }
//  if(docDataTableRow !=NULL)
//  {
//      delete docDataTableRow ;
//      docDataTableRow =NULL ;
//  }
}

int DcParseXml::clear()
{
    //l_list.clear() ;
    memset(&t_st,0x00,sizeof(t_st)) ;
    //memset(&t_st,0x00,sizeof(t_fn)) ;
    // memset(m_sql,0,2048) ;

    return 0 ;
}

int DcParseXml::parseXmlData(const std::string& xmlDAta,t_dcc &t_info)
{
    int ret =0 ;
    ACE_DEBUG((MY_DEBUG ACE_TEXT("parse  xml buf strart.......")));
    char utf8Buf[24576]= {0};
    char gbkBuf[24576]= {0};
    memset(utf8Buf,0,sizeof(utf8Buf)) ;
    memset(gbkBuf,0,sizeof(gbkBuf)) ;
    strcpy(utf8Buf,xmlDAta.c_str()) ;
    ret=UTF82GBK(utf8Buf,gbkBuf);
    if(ret == 0)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("utf-8 to gdk fails!")));
    }
    TiXmlDocument*  docDataTableRow   = new TiXmlDocument() ;

    docDataTableRow->Parse(gbkBuf,NULL, TIXML_ENCODING_UNKNOWN) ;
    TiXmlElement *tElement =  docDataTableRow->RootElement() ;


    if(tElement == NULL)
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("get FirstChildElement false!!")));
        delete docDataTableRow ;
        docDataTableRow =NULL ;
        return -1 ;//可以返回错误码
    }

    char buf[100] = {0} ;
    strcpy(buf,tElement->Value()) ;

    for(TiXmlElement* tElement2 =tElement->FirstChildElement() ; tElement2 ; tElement2=tElement2->NextSiblingElement())
    {

        memset(&t_st,0x00,sizeof(t_st)) ;
        strcpy(t_st.table,tElement2->Value()) ;//取得表名
        TiXmlAttribute* AttrFlag = tElement2->FirstAttribute() ;
        strcpy(t_st.flag,AttrFlag->Value()) ; //取得flag标识

        for(TiXmlElement* tElement3 =tElement2->FirstChildElement() ; tElement3 ; tElement3=tElement3->NextSiblingElement())
        {
            char bufNewOrOld[4]= {0} ;
            strcpy(t_st.newandold,tElement3->Value()) ;//新旧标识

            for(TiXmlElement* tElement4 =tElement3->FirstChildElement() ; tElement4 ; tElement4=tElement4->NextSiblingElement())
            {
                memset(&t_fn,0x00,sizeof(t_fn)) ;
                strcpy(t_fn.fieldName,tElement4->Value()) ;    //取得叶子节点的名称
                strcpy(t_fn.fieldValue,tElement4->GetText()) ; //取得叶子节点的值
                if(t_fn.fieldValue[0] !='\0')
                {
                    l_list.push_back(t_fn) ;
                }
            }
            //////组装SQL语句
            packageSql(t_st,l_list) ;
            if(m_sql !=NULL)cout<<"packageSql-sql :"<<m_sql<<endl ;
            int iExe =0 ;
            /////////执行SQL语句
            if((iExe=executeSql()) != 0)
            {
                l_list.clear() ;
                if(docDataTableRow !=NULL)
                {
                    delete docDataTableRow ;
                    docDataTableRow =NULL ;
                }
                return iExe ;
            }
            //插入工单表
            work_order_t work_t ;
            memset(&work_t,0x00,sizeof(work_t)) ;
            strcpy(work_t.session_id ,t_info.session_id);
            strcpy(work_t.ser_cnt_id ,t_info.sev_cnt_id);
            strcpy(work_t.action_id  ,t_info.ation_id);
            strcpy(work_t.table_name ,t_st.table);
            if(strcmp("NEW",t_st.newandold)==0)
            {
                strcpy(work_t.oper_type  ,"1");
            }
            else if(strcmp("OLD",t_st.newandold)==0)
            {
                strcpy(work_t.oper_type  ,"2");
            }

            strcpy(work_t.stat       ,"0");
            strcpy(work_t.cc_number  ,t_info.cc_request_num);
            char t_name[60]= {0};
            char t_value[60]= {0};
            for(list<tFNameAndValue>::iterator iter = l_list.begin(); iter != l_list.end() ; iter++)
            {
                char key[96]= {0} ;
                sprintf(key,"%s|%s",t_st.table,iter->fieldName) ;
                // DcParseXml* m_data =DcParseXml::GetInstance();
                map<string, string>::iterator it =m_dataType.find(key) ;
                if(it !=m_dataType.end())
                {
                    //找到主键和值
                    if((strcmp("4",it->second.c_str())==0) || (strcmp("5",it->second.c_str())==0))
                    {
                        strcat(t_name,iter->fieldName)   ;
                        strcat(t_name,",")   ;
                        strcat(t_value,iter->fieldValue) ;
                        strcat(t_value,",") ;
                    }
                }
            }
            if(t_name[0] !='\0')
            {
                memcpy(work_t.filds_name,t_name,strlen(t_name)-1);
                memcpy(work_t.filds_value,t_value,strlen(t_value)-1);
            }
            //插入工单表
            InsertWorkOrder(work_t) ;
            memset(&work_t,0x00,sizeof(work_t)) ;
            l_list.clear() ;
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Execute successful........... ")));
        }
    }

    if(docDataTableRow !=NULL)
    {
        delete docDataTableRow ;
        docDataTableRow =NULL ;
    }
    return 0 ;
}


int DcParseXml::executeSql()
{

    unsigned int ret =0 ;
    DCRMDBDML*        pDML =NULL;
    try
    {
        pDML = new DCRMDBDML(pCon);
        if(m_sql != NULL)
            ret= pDML->Prepare(m_sql);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),m_sql));
            if(pDML !=NULL)
            {
                delete pDML ;
                pDML =NULL ;
            }
            return 2002 ;
        }
        if(strcmp("OLD",t_st.newandold)==0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("execute-sql: %s"),m_sql));
            pDML->Execute();
            //pCon->Commit() ;
            if(pDML !=NULL)
            {
                delete pDML ;
                pDML =NULL ;
            }
            return 0 ;
        }
        int i= 0 ;
        for(list<tFNameAndValue>::iterator iter = l_list.begin(); iter != l_list.end() ; iter++)
        {
            char key[96]= {0} ;
            sprintf(key,"%s|%s",t_st.table,iter->fieldName) ;
            //DcParseXml* m_data =DcParseXml::GetInstance();
            map<string, string>::iterator miter =m_dataType.find(key) ;
            if(miter != m_dataType.end())
            {
                i++ ;
                if((strcmp("1",miter->second.c_str())==0)||(strcmp("4",miter->second.c_str())==0))
                {
                    pDML->SetCharParam(i);
                }
                if((strcmp("2",miter->second.c_str())==0)||(strcmp("5",miter->second.c_str())==0))
                {
                    //pDML->SetLongParam(i);
                    pDML->SetCharParam(i);
                }
            }
            else
            {

                ACE_DEBUG((MY_DEBUG ACE_TEXT("%s undefine in syn_date_type table"),key));
                if(pDML !=NULL)
                {
                    delete pDML ;
                    pDML =NULL ;
                }
                return 2004 ;
            }
        }

        pDML->UnBindParam();
        int j=0 ;
        char sqlInfo[1024]= {0} ;
        strcat(sqlInfo,"values(") ;
        int k =l_list.size() ;
        for(list<tFNameAndValue>::iterator iter = l_list.begin(); iter != l_list.end() ; iter++)
        {
            j++ ;
            pDML->BindParam(j, iter->fieldValue);
            strcat(sqlInfo,iter->fieldValue) ;
            if(j<k)
            {
                strcat(sqlInfo,",") ;
            }
        }
        strcat(sqlInfo,")") ;
        strcat(m_info,sqlInfo) ;
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execute-sql: %s"),m_info));
        m_info[0]='\0' ;

        pDML->Execute();
        //pCon->Commit();
    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        if(pDML !=NULL)
        {
            delete pDML ;
            pDML =NULL ;
        }
        return -1 ;
    }

    if(pDML !=NULL)
    {
        delete pDML ;
        pDML =NULL ;
    }
    return 0 ;
}

int DcParseXml::getTransfer(syn_transfer_t& t)
{
    char buf[1024]= {0} ;

    try
    {

        ptfDQL[0]->UnBindParam();
        ptfDQL[0]->Query();
        while (ptfDQL[0]->Next())
        {
            ptfDQL[0]->GetValue(1, t.old_acct_id);
            ptfDQL[0]->GetValue(2, t.user_id);
            ptfDQL[0]->GetValue(3, t.new_acct_id);
            ptfDQL[0]->GetValue(4, t.session_id);
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[0]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcParseXml::updateTransfer(char* flag,char* userId)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[0]->UnBindParam();
        ptfDML[0]->BindParam(1,flag) ;
        ptfDML[0]->BindParam(2,userId) ;
        ptfDML[0]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDML->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcParseXml::getDataType()
{

    int ret =0 ;
    char buf[1024] = {0};
    DCRMDBConnection* pCon  =NULL;
    DCRMDBDML*        pDML = NULL;
    DCRMDBDQL*        pDQL = NULL;

    try
    {

        char iBuf[128]= {0};
        DcConfig* iconfig = DcConfig::GetInstance();
        strcpy(iBuf,iconfig->OCSOracleString) ;

        unsigned int ret = DCRMDBPool::Instance()->Init(iBuf, 8);
        if (ret)
        {

            ACE_DEBUG((MY_ERROR ACE_TEXT("init RMDB failed!!")));
            return -1;
        }

        pCon = DCRMDBPool::Instance()->GetConnection();
        if (NULL == pCon)
        {
            ACE_DEBUG((MY_ERROR ACE_TEXT("get connection failed!!")));
            return -2;
        }

        pDQL = new DCRMDBDQL(pCon);
        strcpy(buf, "select upper(trim(table_name)),upper(trim(files_name)),upper(trim(data_type)) from syn_data_type");

        pDQL->Prepare(buf);
        pDQL->UnBindParam();
        pDQL->Query();
        while (pDQL->Next())
        {
            char buf1[32]= {0};
            char buf2[64]= {0} ;
            char buf3[2]= {0} ;
            pDQL->GetValue(1, buf1);
            pDQL->GetValue(2, buf2);
            pDQL->GetValue(3, buf3);
            if(buf1[0]=='0' ||buf2[0]=='\0'|| buf3[0]=='\0')
            {
                ACE_DEBUG((MY_ERROR ACE_TEXT("syn_date_type table maybe have unll value !!")));
                return -1 ;
            }

            char key[96]= {0};
            sprintf(key,"%s|%s",buf1,buf2);
            m_dataType.insert(map<string,string>::value_type(key,buf3)) ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDQL->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        if(pDQL != NULL)
        {
            delete pDQL ;
            pDQL = NULL ;
        }
        return -1 ;
    }
    if(pDQL != NULL)
    {
        delete pDQL ;
        pDQL = NULL ;
    }
    return ret ;
}

int DcParseXml::packageSql(tTableAndFlag t,lst_fnav l)
{
    char sqlBuf[2048]= {0} ;
    if(strcmp("NEW",t.newandold)==0) //add
    {
        char fields[500]= {0} ;
        char bindParam[200] = {0};
        int i =1 ;
        for(list<tFNameAndValue>::iterator iter = l.begin(); iter != l.end() ; iter++)
        {
            strcat(fields,iter->fieldName);
            strcat(fields,",");

            char c[5] = {0} ;
            sprintf(c,":%d , ",i++) ;
            strcat(bindParam,c);
        }

        string str1 = fields ;
        string str2 = bindParam ;
        strcat(sqlBuf,"insert into ") ;
        strcat(sqlBuf,t.table) ;
        strcat(sqlBuf,"(") ;

        unsigned int n =str1.find_last_of(",") ;
        if(n == string::npos )
        {
            return NULL ;
        }
        string tmp1= str1.substr(0,n) ;
        strcat(sqlBuf,tmp1.c_str()) ;
        strcat(sqlBuf,") ") ;
        strcat(m_info,sqlBuf) ;
        strcat(sqlBuf,"values(") ;
        unsigned int j =str2.find_last_of(",") ;
        if(j == string::npos )
        {
            return NULL ;
        }
        string tmp2= str2.substr(0,j-1) ;

        strcat(sqlBuf,tmp2.c_str()) ;
        strcat(sqlBuf, " )") ;

        //cout<<"ADD-SQL:"<<sqlBuf<<endl ;
        memset(m_sql,0,2048) ;
        strcpy(m_sql,sqlBuf) ;
        return 0 ;
    }

    if(strcmp("OLD",t.newandold)==0)//delete
    {
        //delete table
        char dBuf[200]= {0} ;
        for(list<tFNameAndValue>::iterator iter = l.begin(); iter != l.end() ; iter++)
        {

            char key[96]= {0} ;
            sprintf(key,"%s|%s",t.table,iter->fieldName) ;
            //DcParseXml* m_data =DcParseXml::GetInstance();
            map<string, string>::iterator it = m_dataType.find(key) ;
            if(it !=m_dataType.end())
            {
                //根据主键删除记录
                if((strcmp("4",it->second.c_str())==0) || (strcmp("5",it->second.c_str())==0))
                {
                    strcat(dBuf,iter->fieldName);
                    strcat(dBuf,"=");
                    strcat(dBuf,iter->fieldValue) ;
                    strcat(dBuf," and ") ;
                }
            }
        }

        string st = dBuf ;
        unsigned int ipos =st.find_last_of("and") ;
        if(ipos == string::npos )
        {
            return NULL ;
        }
        string stbuf =st.substr(0,ipos-3) ;

        strcat(sqlBuf,"delete from ") ;
        strcat(sqlBuf,t.table) ;
        strcat(sqlBuf," where ");
        strcat(sqlBuf,stbuf.c_str()) ;
        //cout<<"DEL-SQL:"<<sqlBuf<<endl ;
        memset(m_sql,0,2048) ;
        strcpy(m_sql,sqlBuf) ;
        return 0 ;
    }

    return 0 ;
}


int DcParseXml::InsertWorkOrder(work_order_t &t)
{

    try
    {
        if(strcmp(t.table_name ,"ACCT")==0 && strcmp(t.table_name ,"CUST")==0)
        {
            return 0 ;
        }

        //pDML = new DCRMDBDML(pCon);
        char szStartTime[21]= {0} ;
        char szDealTime[21]= {0} ;
        PublicLib::GetTime(szStartTime,1) ;
        PublicLib::GetTime(szDealTime,1) ;
        unsigned int  ret =0 ;
        char sql[1024] ;

        ptfDML[7]->UnBindParam();
        ptfDML[7]->BindParam(1,t.session_id);
        ptfDML[7]->BindParam(2,t.ser_cnt_id);
        ptfDML[7]->BindParam(3,t.action_id);
        ptfDML[7]->BindParam(4,t.table_name);
        ptfDML[7]->BindParam(5,t.filds_name);
        ptfDML[7]->BindParam(6,t.filds_value);
        ptfDML[7]->BindParam(7,t.oper_type);
        ptfDML[7]->BindParam(8,t.cc_number);
        ptfDML[7]->BindParam(9,szStartTime);

        ptfDML[7]->Execute();
        //pCon->Commit();

    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        return -1 ;
    }


    return 0 ;

}


int DcParseXml::Init()
{
    try
    {
        char buf[1024]= {0} ;
        char login[128]= {0} ;
        DcConfig* iconfig = DcConfig::GetInstance();
        strcpy(login,iconfig->OCSOracleString) ;

        unsigned int ret = DCRMDBPool::Instance()->Init(login,12);
        if (ret)
        {
            printf("init RMDB failed\n");
            return -1;
        }

        pCon = DCRMDBPool::Instance()->GetConnection();
        if (NULL == pCon)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("get connection failed!")));
            return -2;
        }
        //ptfDQL = new DCRMDBDQL(pCon);
        for(int i =0 ; i<15 ; i++)
        {
            ptfDQL[i] = new DCRMDBDQL(pCon);
        }
        //ptfDML = new DCRMDBDQL(pCon);
        for(int i =0 ; i<15 ; i++)
        {
            ptfDML[i] = new DCRMDBDML(pCon);
        }



        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select old_acct_id ,user_id ,new_acct_id ,session_id from syn_transfer where flag='0' ");
        ret=ptfDQL[0]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[0] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update syn_transfer set flag = :1 where user_id =:2 ");
        ret=ptfDML[0]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[0]->SetCharParam(1);
        ptfDML[0]->SetCharParam(2);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[0] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select serv_id,decode(pre_serv_state ,'F0B','2HA','F0L','2ID','F0A','2HN','F0X','2HC','F0K','2IX','F0F','2HD','F0E','2HS','F0G','2HE','F0C','2HB') ,decode(state ,'F0B','2HA','F0L','2ID','F0A','2HN','F0X','2HC','F0K','2IX','F0F','2HD','F0E','2HS','F0G','2HE','F0C','2HB'),to_char(proc_date,'yyyymmddhh24miss') from TIF_SERV_ACTIVATION where SYN_DEAL_STATE = 0 order by create_date ");
        ret=ptfDQL[1]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[1]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[1] success, sql[%s]"),buf));



        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select  oper_id ,serv_id ,stop_type ,to_char(create_time,'yyyymmddhh24miss') from tb_bil_user_change_history where syn_deal_state = 0 order by create_time ");
        ret=ptfDQL[3]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[3]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[3] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select warn_record_id ,serv_id ,param_id,to_char(create_date,'yyyymmddhh24miss'),to_char(add_months(sysdate,1),'yyyymm')||'01000000' from tb_bil_trigger_record where (TRIGGER_TYPE=3 or TRIGGER_TYPE=9) and SYN_STATE =0 order by create_date ");
        ret=ptfDQL[4]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[4]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[4] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select session_id ,table_name ,primary_name ,primary_value from re_syn_interface_rel  where session_id = :1 ");
        ret=ptfDQL[5]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[5]->SetCharParam(1);
        ptfDQL[5]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[5] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select serv_id,agreement_id,cust_id,product_id,billing_cycle_type_id,product_family_id,create_date,rent_date,completed_date,acc_nbr,state,state_date from serv where serv_id =:1");
        ret=ptfDQL[6]->Prepare(buf);
        if(ret)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[6]->SetCharParam(1);
        ptfDQL[6]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[6] success, sql[%s]"),buf));


        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select serv_id,agreement_id,attr_id,attr_val,eff_date,exp_date from serv_attr where serv_id =:1 and attr_id =:2");
        ret=ptfDQL[7]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[7]->SetCharParam(1);
        ptfDQL[7]->SetCharParam(2);
        ptfDQL[7]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[7] success, sql[%s]"),buf));


        memset(buf,0,sizeof(buf)) ;
        //strcpy(buf, "select  serv_id,agreement_id,address_id,bureau_id,exchange_id,stat_region_id,eff_date,exp_date from serv_location where serv_id =:1 ");
        strcpy(buf, "select exchange_id from serv_location where serv_id =:1 ");
        ret=ptfDQL[8]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[8]->SetCharParam(1);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[8] success, sql[%s]"),buf));


        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select org_id from (select * from org   start with org_id = :1 connect by org_id = prior parent_org_id) where org_level_id = 200");
        ret=ptfDQL[9]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[9]->SetCharParam(1);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[9] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf, "select serv_id ,agreement_id,owe_business_type_id,state,eff_date,exp_date from serv_state_attr where serv_id =:1 and state=:2 order by eff_date  desc ");
        ret=ptfDQL[10]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDQL[10]->SetCharParam(1);
        ptfDQL[10]->SetCharParam(2);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDQL[10] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update TIF_SERV_ACTIVATION set syn_deal_state =:1 where serv_id=:2 ");
        ret=ptfDML[1]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[1]->SetCharParam(1);
        ptfDML[1]->SetCharParam(2);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[1] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update tb_bil_user_change_history set syn_deal_state =:1 where oper_id =:2  ");
        ptfDML[3]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[3]->SetCharParam(1);
        ptfDML[3]->SetCharParam(2);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[3] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "update tb_bil_trigger_record set SYN_STATE = :1 where warn_record_id =:2  ");
        ret=ptfDML[4]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[4]->SetCharParam(1);
        ptfDML[4]->SetCharParam(2);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[4] success, sql[%s]"),buf));

        memset(buf,0,sizeof(buf)) ;
        sprintf(buf, "insert into  re_syn_interface_rel(session_id ,table_name ,primary_name ,primary_value,deal_date) values(:1,:2,:3,:4,sysdate)");
        ret=ptfDML[5]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[5]->SetCharParam(1);
        ptfDML[5]->SetCharParam(2);
        ptfDML[5]->SetCharParam(3);
        ptfDML[5]->SetCharParam(4);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[5] success, sql[%s]"),buf));


        memset(buf,0,1024) ;
        strcat(buf,"insert into syn_session_t(session_id,stat,deal_time) values(:1 ,:2,to_date(:3,'yyyymmddhh24miss'))") ;
        ret= ptfDML[6]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[6]->SetCharParam(1);
        ptfDML[6]->SetCharParam(2);
        ptfDML[6]->SetCharParam(3);
        ptfDML[6]->UnBindParam();
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[6] success, sql[%s]"),buf));


        memset(buf,0,1024) ;
        strcat(buf,"insert into work_order_t(session_id,ser_cnt_id,action_id,table_name,filds_name,filds_value,oper_type,session_req,indb_time) values(:1 ,:2 ,:3 ,:4 ,:5 ,:6, :7, :8, to_date(:9,'yyyymmddhh24miss')) ") ;
        ret= ptfDML[7]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        ptfDML[7]->SetCharParam(1);
        ptfDML[7]->SetCharParam(2);
        ptfDML[7]->SetCharParam(3);
        ptfDML[7]->SetCharParam(4);
        ptfDML[7]->SetCharParam(5);
        ptfDML[7]->SetCharParam(6);
        ptfDML[7]->SetCharParam(7);
        ptfDML[7]->SetCharParam(8);
        ptfDML[7]->SetCharParam(9);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("prepare ptfDML[7] success, sql[%s]"),buf));


    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
        return -1 ;
    }
    return 0 ;
}

void DcParseXml::Commit()
{
    pCon->Commit();
}

void DcParseXml::Rollback()
{
    pCon->Rollback(0);
}
int DcParseXml::InsertSession(string& t)
{
    try
    {

        char szDealTime[21]= {0} ;
        PublicLib::GetTime(szDealTime,1) ;
        unsigned int  ret =0 ;
        char buf[1024] ;
        if(t[0]=='\0')
        {
            return -1 ;
        }
        ptfDML[6]->BindParam(1,t.c_str());
        ptfDML[6]->BindParam(2,"0");
        ptfDML[6]->BindParam(3,szDealTime);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("insert into syn_session_t values(%s,%s,to_date(%s,'yyyymmddhh24miss')"),t.c_str(),"0",szDealTime));
        ptfDML[6]->Execute();
        //pCon->Commit();
    }
    catch(DCRMDBException& e)
    {

        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));

    }

    return 0 ;
}

int DcParseXml::code_convert(const char *fromcode, const char* tocode, char* inbuf, char* poutbuf)
{
    int ret=0;
    iconv_t cd ;

    if((cd = iconv_open(tocode, fromcode)) == (iconv_t)-1)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("convert error[%d][%s]\n"),errno,strerror(errno)));
        return 0;
    }


    size_t inbytesleft = strlen(inbuf);
    size_t outbytesleft = 24576;

    ret = iconv(cd, &inbuf, &inbytesleft, &poutbuf, &outbytesleft);

    iconv_close(cd);

    return 24576-outbytesleft;
}

int DcParseXml::UTF82GBK(char * buf, char *outbuf)
{
    return code_convert("utf8", "gb18030", buf, outbuf);
};

int DcParseXml::getTifServActivationA(tif_serv_activation_a_t& t)
{
    char buf[1024]= {0} ;
    try
    {
//select serv_id,decode(pre_serv_state ,'F0B','2HA','F0L','2ID','F0A','2HN','F0X','2HC','F0K',
//'2IX','F0F','2HD','F0E','2HS','F0G','2HE','F0C','2HB') ,decode(state ,'F0B','2HA','F0L','2ID',
//'F0A','2HN','F0X','2HC','F0K','2IX','F0F','2HD','F0E','2HS','F0G','2HE','F0C','2HB'),
//to_char(proc_date,'yyyymmddhh24miss') from TIF_SERV_ACTIVATION where SYN_DEAL_STATE = 0 order by create_date
        ptfDQL[1]->Query();
        while (ptfDQL[1]->Next())
        {
            ptfDQL[1]->GetValue(1, t.serv_id);
            ptfDQL[1]->GetValue(2, t.pre_serv_state);
            ptfDQL[1]->GetValue(3, t.state);
            ptfDQL[1]->GetValue(4, t.create_date);
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[1]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}




int DcParseXml::getCdmaStopOpenInterface(cdma_stop_open_interface_t& t)
{
    char buf[1024]= {0} ;
    try
    {
//select  oper_id ,serv_id ,stop_type ,to_char(create_time,'yyyymmddhh24miss') 
//from tb_bil_user_change_history where syn_deal_state = 0 order by create_time
        ptfDQL[3]->Query();
        while (ptfDQL[3]->Next())
        {
            ptfDQL[3]->GetValue(1, t.oper_id);
            ptfDQL[3]->GetValue(2, t.serv_id);
            ptfDQL[3]->GetValue(3, t.stop_type);
            ptfDQL[3]->GetValue(4, t.create_time);
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[3]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcParseXml::getTbBilTriggerRecord(tb_bil_trigger_record_t& t)
{
    char buf[1024]= {0} ;
    try
    {
//select warn_record_id ,serv_id ,param_id,to_char(create_date,'yyyymmddhh24miss'),
//to_char(add_months(sysdate,1),'yyyymm')||'01000000' from tb_bil_trigger_record where
//TRIGGER_TYPE=3 and SYN_STATE =0 order by create_date
        ptfDQL[4]->Query();
        while (ptfDQL[4]->Next())
        {
            ptfDQL[4]->GetValue(1, t.warn_record_id);
            ptfDQL[4]->GetValue(2, t.serv_id);
            ptfDQL[4]->GetValue(3, t.param_id);
            ptfDQL[4]->GetValue(4, t.create_date);
            ptfDQL[4]->GetValue(5, t.exp_date);
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[4]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}


int DcParseXml::getResynInerfaceRel(re_syn_interface_rel_t& t,char* sessionId)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDQL[5]->BindParam(1,sessionId);
        ptfDQL[5]->Query();
        while (ptfDQL[5]->Next())
        {
            ptfDQL[5]->GetValue(1, t.session_id);
            ptfDQL[5]->GetValue(2, t.table_name);
            ptfDQL[5]->GetValue(3, t.primary_name);
            ptfDQL[5]->GetValue(4, t.primary_value);
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[5]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}


int DcParseXml::getServ(serv_t& t , char* servId)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDQL[6]->BindParam(1,servId);
        ptfDQL[6]->Query();
        while (ptfDQL[6]->Next())
        {
            ptfDQL[6]->GetValue(1, t.serv_id)               ;
            ptfDQL[6]->GetValue(2, t.agreement_id)          ;
            ptfDQL[6]->GetValue(3, t.cust_id)               ;
            ptfDQL[6]->GetValue(4, t.product_id)            ;
            ptfDQL[6]->GetValue(5, t.billing_cycle_type_id) ;
            ptfDQL[6]->GetValue(6, t.product_family_id)     ;
            ptfDQL[6]->GetValue(7, t.create_date)           ;
            ptfDQL[6]->GetValue(8, t.rent_date)             ;
            ptfDQL[6]->GetValue(9, t.completed_date)        ;
            ptfDQL[6]->GetValue(10,t.acc_nbr)               ;
            ptfDQL[6]->GetValue(11,t.state)                 ;
            ptfDQL[6]->GetValue(12,t.state_date)            ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[6]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcParseXml::getServAttr(serv_attr_t& t , char* servId, char* attrId)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDQL[7]->BindParam(1,servId);
        ptfDQL[7]->BindParam(2,attrId);
        ptfDQL[7]->Query();
        while (ptfDQL[7]->Next())
        {
            ptfDQL[7]->GetValue(1, t.serv_id);
            ptfDQL[7]->GetValue(2, t.agreement_id) ;
            ptfDQL[7]->GetValue(3, t.attr_id)      ;
            ptfDQL[7]->GetValue(4, t.attr_val)     ;
            ptfDQL[7]->GetValue(5, t.eff_date)     ;
            ptfDQL[7]->GetValue(6, t.exp_date)     ;
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[7]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}



int DcParseXml::getServLocation(serv_location_t& t ,char* servId)
{

    char buf[1024]= {0} ;
    try
    {

        ptfDQL[8]->UnBindParam();
        ptfDQL[8]->BindParam(1,servId);
        ptfDQL[8]->Query();
        while (ptfDQL[8]->Next())
        {
//        ptfDQL[8]->GetValue(1, t.serv_id)        ;
//          ptfDQL[8]->GetValue(2, t.agreement_id)   ;
//          ptfDQL[8]->GetValue(3, t.address_id)     ;
//          ptfDQL[8]->GetValue(4, t.bureau_id)      ;
            ptfDQL[8]->GetValue(1, t.exchange_id)    ;
//          ptfDQL[8]->GetValue(6, t.stat_region_id) ;
//          ptfDQL[8]->GetValue(4, t.eff_date)       ;
//          ptfDQL[8]->GetValue(5, t.exp_date)       ;
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[8]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;

    }
    return 0 ;
}


int DcParseXml::getOrg(org_t& t,char* exchangeId)
{

    char buf[1024]= {0} ;
    try
    {
        ptfDQL[9]->UnBindParam();
        ptfDQL[9]->BindParam(1,exchangeId);
        ptfDQL[9]->Query();
        while (ptfDQL[9]->Next())
        {
            ptfDQL[9]->GetValue(1, t.org_id) ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[9]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;

    }
    return 0 ;
}




int DcParseXml::getServStateAttr(serv_state_attr_t& t , char* servId,char* state)
{

    char buf[1024]= {0} ;
    try
    {
        ptfDQL[10]->UnBindParam();
        ptfDQL[10]->BindParam(1,servId);
        ptfDQL[10]->BindParam(2,state);
        ptfDQL[10]->Query();
        while (ptfDQL[10]->Next())
        {
            ptfDQL[10]->GetValue(1, t.serv_id) ;
            ptfDQL[10]->GetValue(2, t.agreement_id) ;
            ptfDQL[10]->GetValue(3, t.owe_business_type_id) ;
            ptfDQL[10]->GetValue(4, t.state) ;
            ptfDQL[10]->GetValue(5, t.eff_date) ;
            ptfDQL[10]->GetValue(6, t.exp_date) ;
            break ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDQL[10]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;

    }
    return 0 ;
}



int DcParseXml::setTifServActivationA(char* pKey,char* flag)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[1]->UnBindParam();
        ptfDML[1]->BindParam(1,flag) ;
        ptfDML[1]->BindParam(2,pKey) ;
        ptfDML[1]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[1]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}


int DcParseXml::setCdmaStopOpenInterface(char* pKey,char* flag)
{
	ACE_DEBUG((MY_DEBUG ACE_TEXT("execute sql[update tb_bil_user_change_history set syn_deal_state = %s where oper_id = %s]"),flag, pKey));
    char buf[1024]= {0} ;
    try
    {
        ptfDML[3]->UnBindParam();
        ptfDML[3]->BindParam(1,flag) ;
        ptfDML[3]->BindParam(2,pKey) ;
        ptfDML[3]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[3]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcParseXml::setTbBilTriggerRecord(char* pKey,char* flag)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[4]->UnBindParam();
        ptfDML[4]->BindParam(1,flag) ;
        ptfDML[4]->BindParam(2,pKey) ;
        ptfDML[4]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[4]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}

int DcParseXml::setResynInerfaceRel(char* sessionId,char* tabName ,char* pFiles,char* pValues)
{
    char buf[1024]= {0} ;
    try
    {
        ptfDML[5]->UnBindParam();
        ptfDML[5]->BindParam(1,sessionId) ;
        ptfDML[5]->BindParam(2,tabName) ;
        ptfDML[5]->BindParam(3,pFiles) ;
        ptfDML[5]->BindParam(4,pValues) ;

        ptfDML[5]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        ptfDML[5]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_ERROR ACE_TEXT("execption[%s]\nsql[%s]\n"),e.ToString(), buf));
        return -1 ;
    }

    return 0 ;
}
