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

    *@description ���캯��
    ******************************************************************************************/
    Indb() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ��������
    ******************************************************************************************/
    ~Indb() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ��ʼ������
    ******************************************************************************************/
    int Init() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ������������������Ҫ�Ĵ����߼�
    ******************************************************************************************/
    int runProcess() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ��ȡһ���ļ���
    ******************************************************************************************/
    int getFileName() ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ��ȡ�ļ�Ŀ¼
    ******************************************************************************************/
    int getFile(char *path, const int length) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ��ȡ�ļ�Ŀ¼
    ******************************************************************************************/
    int getFile(char *path, const int length , unsigned long& filesize);

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ��ȡ�����б�
    ******************************************************************************************/
    int getTaskList()   ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ���������б�
    ******************************************************************************************/
    int updateTaskList(char* taskId,char* state);

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description д��־�����ݿ�
    ******************************************************************************************/
    int insertFilesynLog(tFileSynLog t_Rec) ;

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ����Ự��
    ******************************************************************************************/
    int InsertSession(char* t) ;

    /****************************************************************************************
        *@input

        *@output

        *@return

        *@description   ��ȡһ��ʵ��
        ******************************************************************************************/
    static Indb* GetInstance();

    /****************************************************************************************
    *@input

    *@output

    *@return

    *@description ���빤����
    ******************************************************************************************/
    int InsertWorkOrder(work_order_t &t) ;

public :
    static Indb* m_instance ;
    Directory* m_dir ;
    FileDeal* m_fileDeal ;
    lst_tstm m_lst ;
    Info     m_info ;

    //��������ļ����ļ�·���������ļ�·���������ļ�·�����ļ����ơ��ļ��ָ����š�����
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