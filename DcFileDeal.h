#ifndef _FILE_DEAL_H_
#define _FILE_DEAL_H_
#include <unistd.h>
#include <stdio.h>
#include <string>
#include "DcInfo.h"
#include "DcRecord.h"

class FileDeal
{
public:

    /**
    * FileDeal:�����ļ��๹�캯��
    * @param    ��
    * @return
    */
    FileDeal();

    /**
    * ~FileDeal:�����ļ�����������
    * @param    ��
    * @return
    */
    ~FileDeal();

    /**
    * setfilepath:���������ļ�Ŀ¼
    * @param    filepath    �����ļ�Ŀ¼
    * @return   ��
    */
    void    setfilepath(const char* filepath);

    /**
    * setbakfilepath:���������ļ�����Ŀ¼
    * @param    bakfilepath �����ļ�����Ŀ¼
    * @return   ��
    */
    void    setbakfilepath(const char* bakfilepath);

    /**
    * setbakfilepath:���������ļ��쳣Ŀ¼
    * @param  bakfilepath �����ļ��쳣Ŀ¼
    * @return ��
    */
    void    setabnormalpath(const char* abnormalpath);


    /**
    * setbakfilepath:������Ч�ļ�Ŀ¼
    * @param    bakfilepath ������Ч�ļ�Ŀ¼��
    * @return   ��
    */
    void    setinvalidpath(const char* invalidpath);

    /**
    * setfilename:���������ļ��ļ�����
    * @param    filename    �����ļ��ļ�����
    * @return   ��
    */
    void    setfilename(const char* filename);


    /**
    * setloadtables:����ϵͳ������װ����Ϣ
    * @param    loadtables  ָ������װ����Ϣ�����ָ��
    * @return   ��
    */
    //void  setloadtables(LoadTable* loadtables);

    /**
    * setseperator:���������ļ��ļ��ָ���
    * @param    seperator   ָ�������ļ��ļ��ָ�����ָ��
    * @return   ��
    */
    void    setseperator(const char* seperator);



    /**
    * open:�������ļ�
    * @param    ��
    * @return   ��
    */
    int open();

    /**
    * close:�ر������ļ�
    * @param    ��
    * @return   ��
    */
    void    close();

    /**
    * unlink:�Ƴ������ļ�
    * @param
    * @return   ��
    */
    void    remove();

    /**
    * unlink:�Ƴ���Ч�ļ������ļ�
    * @param
    * @return   ��
    */
    void    removeInv();


    void writeErrFile(Info &info) ;
    /**
    * getIIInfo:��ȡ������������Ϣ
    * @param    chgid       �����ˮ��
    * @param    info        ������Ϣ
    * @param    log         ��־��Ϣ
    * @return   ִ�н�� true:�ɹ� false:ʧ��,�޼�¼
    */
    int     getInfo(string &chgid, int &infotype, Info &info, char* errrecord);



private:

    /**
    * parse:��һ�м�¼���ַ���ת����Record��
    * @param    filetype    �ļ�����
    * @param    line        һ�м�¼(�ַ�����)
    * @param    record      һ�м�¼(Record��)
    * @return   ִ�н�� true:�ɹ� false:ʧ��,��¼�����ļ����ڴ����¼
    */
    bool    parse(char* line, Record &record);

    /**
    * getfilefullname:���������·�����ļ�����ȡ�ļ�ȫ��
    * @param    filepath        ·��
    * @param    filename        �ļ���
    * @param    filefullname    �ļ�ȫ��
    * @return   ��
    */
    void    getfilefullname(const char* filepath, const char* filename, char* filefullname);

    /**
    * str:���������·�����ļ�����ȡ�ļ�ȫ��
    * @param    str       ԭʼ�ַ���
    * @param    delim       �ָ��
    * @param    result  �洢���
    * @return   ��
    */
    unsigned int    split(const string& str,const string& delim,vector<string>& result) ;

private:

    //�����ļ�����·��������·�����ļ����ơ��ļ��ָ����,�����¼��ЧĿ¼,��Ч�ļ��Ĵ���Ŀ¼
    char m_filepath[MAX_FILEPATH_LEN];
    char m_bakfilepath[MAX_FILEPATH_LEN];
    char m_filename[MAX_FILENAME_LEN];
    char m_seperator[2];
    char m_abnormalpath[MAX_FILENAME_LEN];
    char m_invalidpath[MAX_FILENAME_LEN];

    //�����ļ���Ӧ���ļ���
    FILE            *m_fp;
    FILE      *m_efp ;

    //�����ļ���ǰ��ȡ�������һ����¼����Ϣ
    string          m_lastchgid;
    int               m_lastinfotype;
    Record          *m_lastrecord;

};


#endif
