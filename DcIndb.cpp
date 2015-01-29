#include <string>
#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"
#include "pub_common/DCPrepareSql.h"
#include "DcIndb.h"
#include "util/publiclib.h"
#include "DcConfig.h"


long long seq =0 ;
Indb* Indb::m_instance = NULL ;
Indb* Indb::GetInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new Indb() ;
    }
    return m_instance ;
}

Indb::Indb()
{
    pCon=NULL ;
    pDML=NULL ;
    pDML2=NULL ;
    pDQL=NULL ;
    m_seperator="|";
    m_fileDeal =NULL ;
    m_info.clear();
    for(int i= 0 ; i<5 ; i++)
    {
        pDML1[i]=NULL ;
    }
    m_data=NULL ;
}

Indb::~Indb()
{
    if(m_dir != NULL)
        delete m_dir ;

    for(int i= 0 ; i<5 ; i++)
    {
        if(pDML1[i] !=NULL)
        {
            delete pDML1[i] ;
            pDML1[i]=NULL ;
        }
    }
    if(m_data!=NULL)
    {
        delete m_data ;
        m_data=NULL ;
    }

}

int Indb::Init()
{
    try
    {
        char buf[1024] = {0};
        DcConfig* iconfig = DcConfig::GetInstance();
        strcpy(buf,iconfig->OCSOracleString) ;

        unsigned int ret = DCRMDBPool::Instance()->Init(buf, 8);
        if (ret)
        {
            printf("init RMDB failed\n");
            return -1;
        }

        pCon = DCRMDBPool::Instance()->GetConnection();
        if (NULL == pCon)
        {
            printf("get connection failed\n");
            return -2;
        }


        memset(buf,0,sizeof(buf)) ;
        pDQL = new DCRMDBDQL(pCon);
        strcpy(buf, "select trim(TASK_ID),trim(FILE_TYPE),trim(TABLE_NAME),trim(TASK_NAME),trim(TASK_PATH),trim(to_char(CREATED_TIME,'yyyy-MM-dd HH24:mi:ss')),trim(to_char(DEAL_TIME,'yyyy-MM-dd HH24:mi:ss')),trim(TASK_STATE)  from TB_SYN_TASK_MANAGER where TASK_STATE = '0' ");
        pDQL->Prepare(buf);


        for(int i =0 ; i<5 ; i++)
        {
            pDML1[i] =new DCRMDBDML(pCon);
        }

        memset(buf,0,sizeof(buf)) ;
        strcpy(buf," update tb_syn_task_manager set task_state =:1,DEAL_TIME =TO_DATE(:2,'YYYYMMDDHH24MISS') WHERE task_id=:3 ") ;
        pDML1[0]->Prepare(buf) ;




        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare buf[%s] fails"),buf));
            return -1 ;
        }
        pDML1[0]->SetCharParam(1) ;
        pDML1[0]->SetCharParam(2) ;
        pDML1[0]->SetCharParam(3) ;
        pDML1[0]->UnBindParam();



        memset(buf,0,sizeof(buf)) ;
        strcpy(buf,"INSERT INTO TB_SYN_FILESYN_LOG(LOG_ID,TASK_ID,TASK_NAME,TASK_PATH,TOTAL_RECORDS,NORMAL_RECORDS,ABNORMAL_RECORDS,INVALID_RECORDS,BEGIN_TIME,END_TIME) VALUES(:1,:2,:3,:4,:5,:6,:7,:8,to_date(:9,'yyyymmddhh24miss'),to_date(:10,'yyyymmddhh24miss'))") ;
        pDML1[1]->Prepare(buf) ;
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare buf[%s] fails"),buf));
            return -1 ;
        }

        pDML1[1]->SetCharParam(1);
        pDML1[1]->SetCharParam(2);
        pDML1[1]->SetCharParam(3);
        pDML1[1]->SetCharParam(4);
        pDML1[1]->SetCharParam(5);
        pDML1[1]->SetCharParam(6);
        pDML1[1]->SetCharParam(7);
        pDML1[1]->SetCharParam(8);
        pDML1[1]->SetCharParam(9);
        pDML1[1]->SetCharParam(10);
        pDML1[1]->UnBindParam();


        memset(buf,0,1024) ;
        strcat(buf,"insert into syn_session_t(session_id,stat,deal_time) values(:1 ,:2 ,to_date(:3 ,'yyyymmddhh24miss'))") ;
        ret= pDML1[2]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        pDML1[2]->SetCharParam(1);
        pDML1[2]->SetCharParam(2);
        pDML1[2]->SetCharParam(3);



        memset(buf,0,1024) ;
        strcat(buf,"insert into work_order_t(session_id,ser_cnt_id,action_id,table_name,filds_name,filds_value,oper_type,session_req,indb_time) values(:1 ,:2 ,:3 ,:4 ,:5 ,:6, :7, :8, to_date(:9,'yyyymmddhh24miss')) ") ;
        ret= pDML1[3]->Prepare(buf);
        if(ret != 0)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Prepare sql[%s] fails"),buf));
            return -1 ;
        }
        pDML1[3]->SetCharParam(1);
        pDML1[3]->SetCharParam(2);
        pDML1[3]->SetCharParam(3);
        pDML1[3]->SetCharParam(4);
        pDML1[3]->SetCharParam(5);
        pDML1[3]->SetCharParam(6);
        pDML1[3]->SetCharParam(7);
        pDML1[3]->SetCharParam(8);
        pDML1[3]->SetCharParam(9);


        m_data=DcParseXml::GetInstance();

    }
    catch(DCRMDBException& e)
    {
        printf("execption[%s]\n", e.ToString());
        return -1 ;
    }
    return 0 ;

}

int Indb::getFile(char *path, const int length)
{
    unsigned long size;
    return getFile(path, length, size);
}


int Indb::getFile(char *path, const int length , unsigned long& filesize)
{
    int ret = 0 ;
    if ((ret = m_dir->getFile(path, length, filesize)) != 0)
        return ret;
    m_dir->scanFiles((Directory::SortFlags)2);//按文件修改时间
    return m_dir->getFile(path, length, filesize);
}


int Indb::runProcess()
{


    char buf[2048] = {0};
    char iBuf[128]= {0};
    map<int,string> map_str ;
    char szStartTime[21]= {0};
    char szEndTime[21]= {0} ;
    try
    {
        DcConfig* iconfig = DcConfig::GetInstance();
//          strcpy(iBuf,iconfig->OCSOracleString) ;
        int retTask = -1 ;
        retTask=getTaskList();
        if(retTask ==0 || retTask == -1)
        {
            return 0 ;
        }

        int ret =0 ;

        //pDML->Prepare(buf);

        m_fileDeal=new FileDeal() ;
        m_fileDeal->setseperator(m_seperator.c_str())   ;
        m_fileDeal->setbakfilepath(iconfig->m_bakPath)  ;
        m_fileDeal->setabnormalpath(iconfig->m_errPath) ;
        m_fileDeal->setinvalidpath(iconfig->m_invPath)  ;

        for(list<tSynTaskMgr>::iterator iter = m_lst.begin(); iter != m_lst.end() ; iter++)
        {
            PublicLib::GetTime(szStartTime,1) ;
            m_fileDeal->setfilepath(iter->taskPath) ;
            m_fileDeal->setfilename(iter->taskName) ;
            //此处需要增加备份目录
            if((ret=m_fileDeal->open())!=0)
            {
                continue ;
            }

            bool flag =false ;
            if(strcmp(iter->tableName,"SERV")==0|| strcmp(iter->tableName,"SERV_ACCT")==0 ||
               strcmp(iter->tableName,"SERV_ATTR")==0||strcmp(iter->tableName,"SERV_PRODUCT")==0||
               strcmp(iter->tableName,"PRODUCT_OFFER_INSTANCE")==0||strcmp(iter->tableName,"PRODUCT_OFFER_INSTANCE_DETAIL")==0||
               strcmp(iter->tableName,"PRODUCT_OFFER_INSTANCE_ATTR")==0)
            {
                flag =true ;
            }

            char line[2000] = "\0";
            int infotype = 0;
            string chgid = "";
            Info info;
            int tRec = -1 ;
            if((tRec =m_fileDeal->getInfo(chgid, infotype,info,line)) == 0)
            {
                m_fileDeal->close() ;
                continue ;
            }

            m_info.push_back(info[0]) ;//文件头
            m_info.push_back(info[1]) ;//文件字段
            pDML2=new DCRMDBDML(pCon);
            if(strcmp("A",iter->fileType)==0)//全量文件,先truncate整张表
            {
                char trBuf[128]= {0};
                sprintf(trBuf,"truncate table %s",iter->tableName);
                pDML2->Prepare(trBuf);
                pDML2->Execute();
                pCon->Commit() ;
            }
            if(pDML2!=NULL)
            {
                delete pDML2 ;
                pDML2=NULL ;
            }
            //拼接表字段的名,
            //m_fields = insert into table_name (id,name) ;
            //m_bindParam = vulues(:1,:2)
            //m_del=delete from tab where id =:1 and name=:2

            Record* m_RecordField=info[1] ;//字段的行
            char m_fields[1024] = {0} ;
            char m_bindParam[1024] = {0} ;
            char m_del[1024]= {0};
            char m_primary[128]= {0};     //存放主键名称,给插入工单表用
            char m_primary_value[128]= {0}; //存放主键值,给插入工单表用
            char m_oper_type[2]= {0} ;
            strcat(m_fields,"(") ;
            strcat(m_bindParam," values(") ;
            for(int i = 2 ; i<m_RecordField->size()-1 ; i++)
            {
                char m[30]= {0} ;
                sprintf(m,"%s,",(*m_RecordField)[i])  ;
                strcat(m_fields,m) ;

                char c[4]= {0} ;
                sprintf(c,":%d , ",i-1) ;
                strcat(m_bindParam,c) ;
            }

            for(int i = 2 ; i<=m_RecordField->size()-1 ; i++)
            {
                char key[96]= {0} ;
                sprintf(key,"%s|%s",iter->tableName,(*m_RecordField)[i]) ;

                map<string, string>::iterator iter = m_data->m_dataType.find(key) ;
                if(iter != m_data->m_dataType.end())
                {
                    if((strcmp("4",iter->second.c_str())==0) || (strcmp("5",iter->second.c_str())==0))
                    {
                        map_str.insert(map<int,string>::value_type(i,(*m_RecordField)[i]));
                    }
                }
            }
            //获取主键名称,给插入工单表用
            if(flag)
            {
                map<int, string>::iterator itera=map_str.begin();
                int y=0 ;
                for(; itera !=map_str.end() ; ++itera)
                {
                    y++ ;
                    strcat(m_primary,itera->second.c_str()) ;
                    if(y<map_str.size())
                    {
                        strcat(m_primary,",");
                    }
                }
            }

            ///最后一个字段
            char t[30] = {0};
            sprintf(t ,"%s",(*m_RecordField)[m_RecordField->size()-1]) ;
            strcat(m_fields,t) ;
            strcat(m_fields,")") ;

            char s[4] = {0} ;
            sprintf(s ,":%d",m_RecordField->size()-2) ;
            strcat(m_bindParam,s) ;
            strcat(m_bindParam,")") ;

            //-------------------------------------------------------------------------------------

            int m_failNum =0 ;//失败的记录数
            int m_sucsNum =0 ;//成功的记录数
            int m_totalNum =0 ;//总的记录数
            int m_comimtNum=0 ;//多少条记录提交一次
            for(int i = 2 ; i<info.size()-1 ; i++)
            {
                pDML = new DCRMDBDML(pCon);
                Record* m_RecordVelue=info[i];//值行
                try
                {
                    m_totalNum++ ;//记录总的记录数
                    //Record* m_RecordVelue=info[i];//值行
                    if(strcmp("1",(*m_RecordVelue)[1])==0||strcmp("0",(*m_RecordVelue)[1])==0)//add sql
                    {
                        sprintf(buf,"insert into %s %s %s",iter->tableName,m_fields,m_bindParam);
                        pDML->Prepare(buf);
                        int k =0 ;
                        for(int j = 2 ; j< m_RecordVelue->size(); j++ )
                        {
                            k++ ;
                            pDML->SetCharParam(k);   //设置变量
                        }


                        pDML->UnBindParam();
                        int r =0 ;
                        for(int a = 2 ; a< m_RecordVelue->size(); a++ )
                        {
                            r++ ;
                            pDML->BindParam(r, (*m_RecordVelue)[a]);//绑定变量
                        }
                        strcpy(m_oper_type,"1");

                    }
                    else if(strcmp("2",(*m_RecordVelue)[1]) == 0)//del sql
                    {
                        sprintf(buf,"delete from %s where ",iter->tableName) ;
                        strcpy(m_oper_type,"2");
                        int n=0 ;
                        //获取到字段名
                        map<int, string>::iterator it=map_str.begin();
                        for(; it !=map_str.end() ; ++it)
                        {
                            n++ ;
                            char t[32]= {0};
                            sprintf(t,"%s =:%d",it->second.c_str(),n) ;
                            strcat(buf,t) ;
                            if(n<map_str.size())
                            {
                                strcat(buf," and ");
                            }
                        }

                        pDML->Prepare(buf);
                        for(int k =1 ; k<=map_str.size() ; k++)
                        {
                            pDML->SetCharParam(k);      //设置类型
                        }
                        pDML->UnBindParam();
                        int c=0 ;
                        //获取到字段值
                        for(int a = 2 ; a< m_RecordVelue->size(); a++ )
                        {
                            map<int, string>::iterator itr = map_str.find(a) ;//字段与值的对应,通过这个KEY来实现
                            if(itr !=  map_str.end())
                            {
                                c++ ;
                                pDML->BindParam(c,(*m_RecordVelue)[a]);//绑定变量
                            }
                        }
                    }
                    else
                    {
                        m_info.push_back(m_RecordVelue) ;
                        m_failNum++ ;
                        continue ;
                    }



                    //获取到主键的值
                    if(flag)
                    {
                        int r =0 ;
                        for(int a = 2 ; a< m_RecordVelue->size(); a++ )
                        {
                            map<int, string>::iterator itr = map_str.find(a) ;
                            if(itr !=     map_str.end())
                            {
                                r++ ;
                                strcat(m_primary_value,(*m_RecordVelue)[a]) ;
                                if(r<map_str.size())
                                {
                                    strcat(m_primary_value,",") ;
                                }
                            }
                        }
                    }

                    pDML->Execute();
                    m_comimtNum++ ;
                    if(m_comimtNum == 1000)
                    {
                        pCon->Commit() ;
                        m_comimtNum =0 ;
                    }
                    m_sucsNum++ ;
                    if(pDML !=NULL)
                    {
                        delete pDML ;
                        pDML =NULL ;
                    }


                    if(flag)
                    {
                        char sysTime[21]= {0} ;
                        PublicLib::GetTime(sysTime,6) ;
                        char sessid[22]= {0} ;
                        sprintf(sessid,"%s%ld",sysTime,seq) ;
                        InsertSession(sessid) ;
                        work_order_t t ;
                        memset(&t,0x00,sizeof(t));
                        strcpy(t.session_id,sessid);
                        strcpy(t.ser_cnt_id,"0");
                        strcpy(t.action_id,"0");
                        strcpy(t.table_name,iter->tableName);
                        strcpy(t.filds_name,m_primary);
                        strcpy(t.filds_value,m_primary_value);
                        strcpy(t.oper_type,m_oper_type);
                        strcpy(t.cc_number,"0");
                        InsertWorkOrder(t) ;
                        memset(m_oper_type,0,sizeof(m_oper_type));
                        memset(m_primary_value,0,sizeof(m_primary_value)) ;

                    }

                }
                catch(DCRMDBException& e)
                {
                    //printf("execption[%s]\nsql[%s]\n", e.ToString(), buf);
                    ACE_DEBUG((MY_DEBUG ACE_TEXT("execption[%s]\nsql[%s]"), e.ToString(), buf));

                    m_info.push_back(m_RecordVelue) ;
                    m_failNum++ ;
                    if(pDML !=NULL)
                    {
                        delete pDML ;
                        pDML =NULL ;
                    }
                    continue ;
                }

            }
            pCon->Commit() ;//剩余的记录提交一次

            //update tb_syn_task_manager
            if(m_failNum == m_totalNum)
            {
                updateTaskList(iter->taskId,"9"); //整个文件处理失败
            }
            else if(m_sucsNum == m_totalNum)
            {
                updateTaskList(iter->taskId,"1"); //整个文件处理成功
            }
            else if(m_failNum != 0 && m_sucsNum !=0)
            {
                updateTaskList(iter->taskId,"8"); //文件有一部分成功
            }

            //插入日志
            PublicLib::GetTime(szEndTime,1) ;
            tFileSynLog tFile ;
            memset(&tFile,0x00,sizeof(tFile)) ;
            strcpy(tFile.logId,iter->taskId);
            strcpy(tFile.taskId,iter->taskId);
            strcpy(tFile.taskName,iter->taskName);
            strcpy(tFile.taskPath,iter->taskPath) ;
            sprintf(tFile.totalRecords,"%d",m_totalNum) ;
            sprintf(tFile.normalRecords,"%d",m_sucsNum) ;
            sprintf(tFile.abnormalRecords,"%d",m_failNum) ;
            sprintf(tFile.invalidrecords,"%d",m_failNum) ;
            strcpy(tFile.beginTime,szStartTime) ;
            strcpy(tFile.endTime,szEndTime);
            insertFilesynLog(tFile) ;
            memset(szStartTime,0,21) ;
            memset(szEndTime,0,21);
            m_info.push_back(info[info.size()-1]);
            if(m_info.size()>3)
            {
                m_fileDeal->writeErrFile(m_info) ;
            }
            m_fileDeal->close() ;
            info.clear()        ;
            map_str.clear()     ;
            m_fileDeal->remove();
        }
        m_lst.clear() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDML->GetSqlString(buf, 1024);
        //printf("execption[%s]\nsql[%s]\n", e.ToString(), buf);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption[%s]\nsql[%s]"), e.ToString(), buf));
        //return -1 ;
    }

    if(m_fileDeal !=NULL)
    {
        delete m_fileDeal ;
        m_fileDeal =NULL  ;
    }

    return 0 ;
}


int Indb::getFileName()
{
    char fileName[80]= {0};
    //没有获取到文件,返回错误
    if ( !getFile(fileName, sizeof(fileName)))
    {
        return 1;
    }

    m_filename = fileName ;

    return 0 ;
}


int Indb::getTaskList()
{
    int ret =0 ;
    int fNum=0 ;
    char buf[1024] = {0};

    try
    {

        pDQL->UnBindParam();
        pDQL->Query();
        while (pDQL->Next())
        {
            tSynTaskMgr t_tsk ;
            memset(&t_tsk,0x00,sizeof(t_tsk)) ;
            pDQL->GetValue(1,t_tsk.taskId);
            pDQL->GetValue(2,t_tsk.fileType);
            pDQL->GetValue(3,t_tsk.tableName);
            pDQL->GetValue(4,t_tsk.taskName);
            pDQL->GetValue(5,t_tsk.taskPath);
            pDQL->GetValue(6,t_tsk.createTime);
            pDQL->GetValue(7,t_tsk.dealTime);
            pDQL->GetValue(8,t_tsk.taskState);
            m_lst.push_back(t_tsk) ;
            fNum++ ;
        }
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDQL->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption[%s]\nsql[%s]"), e.ToString(), buf));
        return -1 ;
    }

    return fNum ;
}

int Indb::updateTaskList(char* taskId,char* state)
{

    if(taskId == NULL && state == NULL) return -1 ;
    int ret =0 ;
    char buf[1024] = {0};
    char szTime[21]= {0} ;
    PublicLib::GetTime(szTime,1) ;

    try
    {
        pDML1[0]->BindParam(1,state) ;
        pDML1[0]->BindParam(2,szTime) ;
        pDML1[0]->BindParam(3,taskId) ;
        pDML1[0]->Execute();
        pCon->Commit() ;
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDML1[0]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption[%s]\nsql[%s]"), e.ToString(), buf));
        return -1 ;
    }

    return ret ;
}


int Indb::insertFilesynLog(tFileSynLog t)
{

    int ret =0 ;
    char buf[1024] = {0};

    try
    {

        pDML1[1]->BindParam(1,t.logId)          ;
        pDML1[1]->BindParam(2,t.taskId)         ;
        pDML1[1]->BindParam(3,t.taskName)       ;
        pDML1[1]->BindParam(4,t.taskPath)       ;
        pDML1[1]->BindParam(5,t.totalRecords)   ;
        pDML1[1]->BindParam(6,t.normalRecords)  ;
        pDML1[1]->BindParam(7,t.abnormalRecords);
        pDML1[1]->BindParam(8,t.invalidrecords) ;
        pDML1[1]->BindParam(9,t.beginTime)      ;
        pDML1[1]->BindParam(10,t.endTime)       ;

        pDML1[1]->Execute()    ;
        pCon->Commit()     ;
        ACE_TEXT("insertFilesynLog Execute successfully");
    }
    catch(DCRMDBException& e)
    {
        memset(buf, 0, 1024);
        pDML1[1]->GetSqlString(buf, 1024);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption[%s]\nsql[%s]"), e.ToString(), buf));
        return -1 ;
    }

    return ret ;
}

int Indb::InsertSession(char* t)
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
        pDML1[2]->BindParam(1,t);
        pDML1[2]->BindParam(2,"0");
        pDML1[2]->BindParam(3,szDealTime);
        ACE_DEBUG((MY_DEBUG ACE_TEXT("insert into syn_session_t values(%s,%s,to_date(%s,'yyyymmddhh24miss'))"),t,"0",szDealTime));
        pDML1[2]->Execute();
        //pCon->Commit();
    }
    catch(DCRMDBException& e)
    {
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption: %s"),e.ToString()));
        ACE_DEBUG((MY_DEBUG ACE_TEXT("execption-sql: %s"),e.GetSqlErrorInfo()));
    }

    return 0 ;
}

int Indb::InsertWorkOrder(work_order_t &t)
{
    try
    {
        //pDML = new DCRMDBDML(pCon);
        char szStartTime[21]= {0} ;
        char szDealTime[21]= {0} ;
        PublicLib::GetTime(szStartTime,1) ;
        PublicLib::GetTime(szDealTime,1) ;
        unsigned int  ret =0 ;
        char sql[1024] ;

        pDML1[3]->UnBindParam();
        pDML1[3]->BindParam(1,t.session_id);
        pDML1[3]->BindParam(2,t.ser_cnt_id);
        pDML1[3]->BindParam(3,t.action_id);
        pDML1[3]->BindParam(4,t.table_name);
        pDML1[3]->BindParam(5,t.filds_name);
        pDML1[3]->BindParam(6,t.filds_value);
        pDML1[3]->BindParam(7,t.oper_type);
        pDML1[3]->BindParam(8,t.cc_number);
        pDML1[3]->BindParam(9,szStartTime);

        pDML1[3]->Execute();
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