#ifndef __DC_INDB_H__
#define __DC_INDB_H__
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <rmdb.h>
#include "DcDirectory.h"
#include "DcFileDeal.h"
#include "DcInfo.h"
#include "DcRecord.h"
#include "syn_struct.h"
#include "DcParseXml.h"


class Indb
{
public:
    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 构造函数
    ******************************************************************************************/
    Indb() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 析构函数
    ******************************************************************************************/
    ~Indb() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 初始化函数
    ******************************************************************************************/
    int Init() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 处理主函数，负责主要的处理逻辑
    ******************************************************************************************/
    int runProcess() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 获取一个文件名
    ******************************************************************************************/
    int getFileName() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 获取文件目录
    ******************************************************************************************/
    int getFile(char *path, const int length) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 获取文件目录
    ******************************************************************************************/
    int getFile(char *path, const int length , unsigned long& filesize);

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 获取任务列表
    ******************************************************************************************/
    int getTaskList()   ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 更新任务列表
    ******************************************************************************************/
    int updateTaskList(char* taskId,char* state);

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 写日志到数据库
    ******************************************************************************************/
    int insertFilesynLog(tFileSynLog t_Rec) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 插入会话表
    ******************************************************************************************/
    int InsertSession(char* t) ;

    /****************************************************************************************
        *@input

        *@output

        *@return

        *@description   获取一个实例
        ******************************************************************************************/
    static Indb* GetInstance();

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description 插入工单表
    ******************************************************************************************/
    int InsertWorkOrder(work_order_t &t) ;

public :
    static Indb* m_instance ;
    Directory* m_dir ;
    FileDeal* m_fileDeal ;
    lst_tstm m_lst ;
    Info     m_info ;

    //最后处理的文件、文件路径、备份文件路径、错误文件路径、文件名称、文件分隔符号、表名
    string m_lastdealfilename;
    string m_filepath;
    string m_bakfilepath;
    string m_errfilepath;
    string m_filename;
    string m_seperator;
    string m_tablename ;
public :
    DCRMDBConnection* pCon ;
    DCRMDBDML*        pDML1[5] ;
    DCRMDBDML*        pDML ;
    DCRMDBDML*        pDML2;
    DCRMDBDQL*        pDQL ;
    DcParseXml*       m_data ;

};

#endif
