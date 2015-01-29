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

    //Ŀ¼������
    enum FtwFlags
    {
        FFTW_F = 1, ///�ļ�
        FFTW_D,     ///Ŀ¼
        FFTW_DNR,   ///����
        FFTW_NS     ///�����ļ�
    };

    ///�ļ��б����򷽷�
    enum SortFlags
    {
        SF_NONE = 0,   ///δָ��
        SF_NAME,       ///���ļ���
        SF_MODIFYTIME  ///���޸�ʱ��
    };

    /**
    *�ļ���ƥ��
    *@param filename �ļ���
    *@param pattern ƥ��ģʽ
    *@return �Ƿ�ƥ��
    */
    static bool file_matches(const char *filename, const char *pattern);


    /**
    *����ƥ��ģʽ
    */
    void setFilter(const char *patterns = "*")
    {
        m_patterns = (patterns ? patterns : "*");
    }

    /**
    *��������Ŀ¼
    *@param path ����Ŀ¼
    */
    void setPath(const char *path);

    /**
    *�õ�һ���ļ���
    *@param path �ļ���·��(ȫ·��)
    *@param len �ļ�����󳤶�
    */
    const char *getPath(char *path, int len = PATH_MAX);


    /**
    *����һ�δ�������ļ�����
    */
    void setMaxScan(const int i)
    {
        m_maxscan = i;
    }

    virtual int ftwProc(const char *pathfile, struct stat& statbuf,FtwFlags ftwFlag);

    typedef int (Directory::*EachProc)(const char *pathfile,struct stat& statbuf, FtwFlags ftwFlag);

    /**
    * �ݹ鴦��ǰĿ¼���ļ�
    * @param eachProc ÿ���ļ�ʹ�õĴ�����
    * @param level �������
    * @return 0 - OK, other - error
    */

    int doEach(EachProc eachProc = &Directory::ftwProc, int level = 1);


    /* ɨ�赱ǰĿ¼ȡ���ļ��б�
    * @param sortBy ʹ�õ����򷽷�
    * @param level �������
    * @return 0 - OK, other - error
    */
    int scanFiles(const SortFlags = SF_NONE, const int level = 1);


    /**
    * ���б���ȡ�ļ�
    * @param shortname ���ص��ļ���
    * @param len ����ļ�������
    * @param filesize �ļ���С
    @ @return = 0 ���ļ�, >0 �õ�һ���ļ�, <0 ��������
    */
    int getFile(char *shortname, const int len ,unsigned long& filesize);

    /**
     * ���б���ȡ�ļ�
     * @param shortname ���ص��ļ���
     * @param len ����ļ�������
     * @return = 0 ���ļ�, >0 �õ�һ���ļ�, <0 ��������
    */
    int getFile(char *shortname, const int len );

    /**
     * �����ļ���Ŀ¼
     * @param src Դ�ļ���Ŀ¼
     * @param dest Ŀ���ļ���Ŀ¼
     * @param pattern ƥ��
     * @param level �������
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
