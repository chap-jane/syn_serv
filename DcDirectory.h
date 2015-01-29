#ifndef __DC_DIRECTORY_H__
#define __DC_DIRECTORY_H__

#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std ;
const int MAX_SHORT_FILENAME = 80;
#define size_t unsigned int ;

class Directory
{
public:

    Directory() ;
    ~Directory() ;

    //目录项类型
    enum FtwFlags
    {
        FFTW_F = 1, ///文件
        FFTW_D,     ///目录
        FFTW_DNR,   ///其他
        FFTW_NS     ///网络文件
    };

    ///文件列表排序方法
    enum SortFlags
    {
        SF_NONE = 0,   ///未指定
        SF_NAME,       ///按文件名
        SF_MODIFYTIME  ///按修改时间
    };

    /**
    *文件名匹配
    *@param filename 文件名
    *@param pattern 匹配模式
    *@return 是否匹配
    */
    static bool file_matches(const char *filename, const char *pattern);


    /**
    *设置匹配模式
    */
    void setFilter(const char *patterns = "*")
    {
        m_patterns = (patterns ? patterns : "*");
    }

    /**
    *设置输入目录
    *@param path 输入目录
    */
    void setPath(const char *path);

    /**
    *得到一个文件名
    *@param path 文件名路径(全路径)
    *@param len 文件名最大长度
    */
    const char *getPath(char *path, int len = PATH_MAX);


    /**
    *设置一次处理最大文件个数
    */
    void setMaxScan(const int i)
    {
        m_maxscan = i;
    }

    virtual int ftwProc(const char *pathfile, struct stat& statbuf,FtwFlags ftwFlag);

    typedef int (Directory::*EachProc)(const char *pathfile,struct stat& statbuf, FtwFlags ftwFlag);

    /**
    * 递归处理当前目录下文件
    * @param eachProc 每个文件使用的处理函数
    * @param level 处理深度
    * @return 0 - OK, other - error
    */

    int doEach(EachProc eachProc = &Directory::ftwProc, int level = 1);


    /* 扫描当前目录取得文件列表
    * @param sortBy 使用的排序方法
    * @param level 处理深度
    * @return 0 - OK, other - error
    */
    int scanFiles(const SortFlags = SF_NONE, const int level = 1);


    /**
    * 从列表中取文件
    * @param shortname 返回的文件名
    * @param len 最大文件名长度
    * @param filesize 文件大小
    @ @return = 0 无文件, >0 得到一个文件, <0 发生错误
    */
    int getFile(char *shortname, const int len ,unsigned long& filesize);

    /**
     * 从列表中取文件
     * @param shortname 返回的文件名
     * @param len 最大文件名长度
     * @return = 0 无文件, >0 得到一个文件, <0 发生错误
    */
    int getFile(char *shortname, const int len );

    /**
     * 复制文件或目录
     * @param src 源文件或目录
     * @param dest 目标文件或目录
     * @param pattern 匹配
     * @param level 复制深度
     @ @return = 0 - OK, other - error
     */
    int dirCopy(const char * src, const char * dest, const char * pattern = 0, const int level = 999);


protected :

    int ftwList(const char *pathfile, struct stat& statbuf,FtwFlags ftwFlag);
    int ftwCopy(const char *pathfile, struct stat& statbuf,FtwFlags ftwFlag);
    int ftwBaseProc(const char *pathfile, struct stat& statbuf,FtwFlags ftwFlag);

    bool isMatch();

    struct FileStat
    {
        char   fs_name[MAX_SHORT_FILENAME];
        unsigned int fs_size;  // File size (bytes)
        //time_t fs_atime; // Time of last access
        unsigned int fs_mtime; // Last modification time
        //time_t fs_ctime; // Last file status change time
    };

    static  bool fscomp_mtime(const FileStat& x, const FileStat& y)
    {
        return x.fs_mtime < y.fs_mtime ? true : false;
    }

    static  bool fscomp_name(const FileStat& x, const FileStat& y)
    {
        return (strcmp(x.fs_name, y.fs_name) < 0)? true : false;
    }


    char m_path[1024 + 1];
    int  m_maxscan;
    int  m_currentscan;
    const char * m_offsetName;
    const char * m_shortName;

    vector <FileStat> m_filesArray;
    unsigned int m_pos;


private:
    char m_fullpath[1024 + 1];
    string m_destPath;
    string m_patterns;


};

#endif
