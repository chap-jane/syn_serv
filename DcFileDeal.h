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
    * FileDeal:输入文件类构造函数
    * @param    无
    * @return
    */
    FileDeal();

    /**
    * ~FileDeal:输入文件类析构函数
    * @param    无
    * @return
    */
    ~FileDeal();

    /**
    * setfilepath:设置输入文件目录
    * @param    filepath    输入文件目录
    * @return   无
    */
    void    setfilepath(const char* filepath);

    /**
    * setbakfilepath:设置输入文件备份目录
    * @param    bakfilepath 输入文件备份目录
    * @return   无
    */
    void    setbakfilepath(const char* bakfilepath);

    /**
    * setbakfilepath:设置输入文件异常目录
    * @param  bakfilepath 输入文件异常目录
    * @return 无
    */
    void    setabnormalpath(const char* abnormalpath);


    /**
    * setbakfilepath:设置无效文件目录
    * @param    bakfilepath 输入无效文件目录名
    * @return   无
    */
    void    setinvalidpath(const char* invalidpath);

    /**
    * setfilename:设置输入文件文件名称
    * @param    filename    输入文件文件名称
    * @return   无
    */
    void    setfilename(const char* filename);


    /**
    * setloadtables:设置系统级资料装载信息
    * @param    loadtables  指向资料装载信息数组的指针
    * @return   无
    */
    //void  setloadtables(LoadTable* loadtables);

    /**
    * setseperator:设置输入文件文件分隔符
    * @param    seperator   指向输入文件文件分隔符的指针
    * @return   无
    */
    void    setseperator(const char* seperator);



    /**
    * open:打开输入文件
    * @param    无
    * @return   无
    */
    int open();

    /**
    * close:关闭输入文件
    * @param    无
    * @return   无
    */
    void    close();

    /**
    * unlink:移除输入文件
    * @param
    * @return   无
    */
    void    remove();

    /**
    * unlink:移除无效文件输入文件
    * @param
    * @return   无
    */
    void    removeInv();


    void writeErrFile(Info &info) ;
    /**
    * getIIInfo:获取增量的增量信息
    * @param    chgid       变更流水号
    * @param    info        资料信息
    * @param    log         日志信息
    * @return   执行结果 true:成功 false:失败,无记录
    */
    int     getInfo(string &chgid, int &infotype, Info &info, char* errrecord);



private:

    /**
    * parse:将一行记录从字符串转换成Record型
    * @param    filetype    文件类型
    * @param    line        一行记录(字符串型)
    * @param    record      一行记录(Record型)
    * @return   执行结果 true:成功 false:失败,记录存在文件属于错误记录
    */
    bool    parse(char* line, Record &record);

    /**
    * getfilefullname:根据输入的路径和文件名获取文件全名
    * @param    filepath        路径
    * @param    filename        文件名
    * @param    filefullname    文件全名
    * @return   无
    */
    void    getfilefullname(const char* filepath, const char* filename, char* filefullname);

    /**
    * str:根据输入的路径和文件名获取文件全名
    * @param    str       原始字符串
    * @param    delim       分割符
    * @param    result  存储结果
    * @return   无
    */
    unsigned int    split(const string& str,const string& delim,vector<string>& result) ;

private:

    //资料文件输入路径、备份路径、文件名称、文件分割符号,错误记录无效目录,无效文件的错误目录
    char m_filepath[MAX_FILEPATH_LEN];
    char m_bakfilepath[MAX_FILEPATH_LEN];
    char m_filename[MAX_FILENAME_LEN];
    char m_seperator[2];
    char m_abnormalpath[MAX_FILENAME_LEN];
    char m_invalidpath[MAX_FILENAME_LEN];

    //资料文件对应的文件流
    FILE            *m_fp;
    FILE      *m_efp ;

    //资料文件当前读取到的最后一条记录的信息
    string          m_lastchgid;
    int               m_lastinfotype;
    Record          *m_lastrecord;

};


#endif
