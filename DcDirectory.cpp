#include <fstream>
#include <cstring>
#include <cstdio>
#include <algorithm>

#include "DcDirectory.h"

Directory::Directory() : m_maxscan(1000)
{
    *m_path = 0;
}

Directory::~Directory()
{
}



bool Directory::file_matches(const char *filename, const char *pattern)
{
    char *pattern_ptr, *filename_ptr;   // Points to pattern and filename

    filename_ptr = (char *) filename;   // Start comparing file name
    pattern_ptr  = (char *) pattern;    // Start comparing file name

    for (;;)
    {
        // If we came to the end of the pattern and the filename, we have
        //  successful match.
        if (*pattern_ptr == '\0' && *filename_ptr == '\0')
            return true;

        // Otherwise, end of either is a failed match
        if (*pattern_ptr == '\0' || *filename_ptr == '\0')
            return false;

        // If the pattern character is '?', then we matched a char
        if (*pattern_ptr == '?' || *pattern_ptr == *filename_ptr)
        {
            pattern_ptr++;
            filename_ptr++;
        }
        else
        {
            // If we have a '*', match as much of the filename as we can
            if (*pattern_ptr == '*')
            {
                pattern_ptr++;        // Try to match following char
                while (*filename_ptr && *filename_ptr != *pattern_ptr)
                    filename_ptr++;
            }
            else
                return false;         // Match failed
        }
    }
}



void Directory::setPath(const char *path)
{
    unsigned int len = strlen(path);
    if ((path != 0) && (len <= 1024))
    {
        strcpy(m_path, path);
        if (*(m_path + len - 1) != '/')
        {
            *(m_path + len) = '/';
            *(m_path + len + 1) = 0;
        }
        strcpy(m_fullpath, m_path);
        m_fullpath[strlen(m_fullpath) - 1] = 0;
    }
}


const char *Directory::getPath(char *path, int len)
{
    if ((path == 0) || (len < strlen(m_path)))
        return 0;
    strcpy(path, m_path);
    return path;
}



int Directory::doEach(EachProc eachProc, int level)
{
    struct stat   statbuf;
    struct dirent *dirp;
    DIR   *dp;
    char  *ptr;
    int   ret;

    if (level-- < 0)
        return 0;

    if (m_currentscan <= 0)
        return 0;

    if (lstat(m_fullpath, &statbuf) < 0)
        return ((this->*eachProc)(m_fullpath, statbuf, FFTW_NS));

    if (S_ISDIR(statbuf.st_mode) == 0)
    {
        // not a directory
        if (--m_currentscan <= 0)
            return 0;
        return ((this->*eachProc)(m_fullpath, statbuf, FFTW_F));
    }

    // is a directory
    if ((ret = (this->*eachProc)(m_fullpath, statbuf, FFTW_D)) != 0)
        return ret;
    if (--m_currentscan <= 0)
        return 0;

    ptr = m_fullpath + strlen(m_fullpath);  // point to end of fullpath
    *ptr++ = '/';
    *ptr = 0;

    if ((dp = opendir(m_fullpath)) == 0)
        // cann't read directory
        return ((this->*eachProc)(m_fullpath, statbuf, FFTW_DNR));

    while ((dirp = readdir(dp)) != 0)
    {
        if (strcmp(dirp->d_name, ".") == 0
            || strcmp(dirp->d_name, "..") == 0)
            // ignore dot and dot-dot
            continue;

        // append name after slash
        strcpy(ptr, dirp->d_name);

        // recursive
        if ((ret = doEach(eachProc, level)) != 0)
            break;    // time to leave
    }
    ptr[-1] = 0;  // erase everything from slash onwards

    if (closedir(dp) < 0)
        return -1;

    return ret;
}

int Directory::ftwBaseProc(const char *pathfile, struct stat& statbuf,FtwFlags flag)
{
    int ret = 0;
    switch (flag)
    {
        case FFTW_F:
        case FFTW_D:
            break;

        case FFTW_DNR:
        case FFTW_NS:
        default:
            ret = -1;
            break;
    }
    if (ret)
        return ret;

    m_offsetName = strstr(pathfile, m_path);
    if (m_offsetName != 0)
    {
        m_offsetName = pathfile + strlen(m_path);
        m_shortName = strrchr(pathfile, '/');
        if (m_shortName != 0)
            ++m_shortName;
    }
    else
        m_shortName = 0;

    return 0;
}


// 0 - OK, other - error
int Directory::scanFiles(const SortFlags sortby, const int level)
{
    m_currentscan = m_maxscan;
    m_filesArray.clear();
    int ret = doEach(&Directory::ftwList, level);

    switch (sortby)
    {
        case SF_NAME:
            sort(m_filesArray.begin(), m_filesArray.end(), fscomp_name);
            break;

        case SF_MODIFYTIME:
            sort(m_filesArray.begin(), m_filesArray.end(), fscomp_mtime);
            break;

        case SF_NONE:
        default:
            break;
    }
    m_pos = 0;
    return ret;
}


int Directory::getFile(char *shortname, const int len )
{
    unsigned long size;
    return getFile(shortname, len , size);
}

// 0 - no file, >0 - get file success, <0 - something error
int Directory::getFile(char *shortname, const int len ,unsigned long& filesize)
{
    if (m_pos >= m_filesArray.size())
        return 0;
    if (strlen(m_filesArray[m_pos].fs_name) >= len)
        return -1;
    strcpy(shortname, m_filesArray[m_pos].fs_name);
    filesize = m_filesArray[m_pos].fs_size;
    m_pos++;
    return m_filesArray.size();
}


int Directory::ftwProc(const char *pathfile, struct stat& statbuf,FtwFlags flag)
{
    int ret = 0;
    switch (flag)
    {
        case FFTW_F:
        case FFTW_D:
            break;

        case FFTW_DNR:
        case FFTW_NS:
        default:
            ret = -1;
            break;
    }
    return ret;

    //-----------------------------------------------------------------------//
    //return ftwList(pathfile, statbuf, flag);
    //return ftwCopy(pathfile, statbuf, flag);
    //return 0;
}


int Directory::ftwList(const char *pathfile, struct stat& statbuf,FtwFlags flag)
{
    if (int ret = Directory::ftwBaseProc(pathfile, statbuf, flag))
        return ret;

    if (flag == FFTW_D)
        return 0;

    if (!isMatch())
        return 0;

    FileStat fs;
    if (strlen(m_shortName) >= MAX_SHORT_FILENAME)
        return 0;
    strcpy(fs.fs_name, m_shortName);

    fs.fs_size = statbuf.st_size;
    //fs.fs_atime = statbuf.st_atime;
    fs.fs_mtime = statbuf.st_mtime;
    //fs.fs_ctime = statbuf.st_ctime;
    m_filesArray.push_back(fs);
    return 0;
}


int Directory::dirCopy(const char * src, const char * dest, const char * pattern, const int level)
{
    char path_bak[PATH_MAX + 1];
    unsigned int len = strlen(src);
    if (len > PATH_MAX)
        return -1;

    m_currentscan = m_maxscan;
    strcpy(path_bak, m_path);
    strcpy(m_path, src);
    if (*(m_path + len - 1) != '/')
    {
        *(m_path + len) = '/';
        *(m_path + len + 1) = 0;
    }
    strcpy(m_fullpath, m_path);
    m_fullpath[strlen(m_fullpath) - 1] = 0;

    m_destPath = dest;
    if (*m_destPath.rbegin() != '/')
        m_destPath.append("/");

    string patterns_bak = m_patterns;
    if (pattern == 0)
        m_patterns = "";
    else
        m_patterns = pattern;

    int ret = doEach(&Directory::ftwCopy, level);

    strcpy(m_path, path_bak);
    strcpy(m_fullpath, path_bak);
    m_fullpath[strlen(m_fullpath) - 1] = 0;

    m_patterns = patterns_bak;
    return ret;
}


int Directory::ftwCopy(const char *pathfile, struct stat& statbuf,FtwFlags flag)
{
    if (int ret = Directory::ftwBaseProc(pathfile, statbuf, flag))
        return ret;

    if ((flag == FFTW_D) && (m_shortName == 0))
        return 0;

    if ((flag == FFTW_F) && (!isMatch()))
        return 0;

    string dest;
    dest = m_destPath + m_offsetName;

    char cmd[2 * (PATH_MAX + 1)];
    if (flag == FFTW_F)
    {
        if (link(pathfile, dest.c_str()))
            return -1;
    }
    if (flag == FFTW_D)
    {
        if (mkdir(dest.c_str(), S_IRWXU|S_IRWXU))
            return -1;
    }
    return 0;
}


bool Directory::isMatch()
{
    char *patterns = (char *)m_patterns.c_str();
    char pattern[PATH_MAX+1];

    if (m_patterns.length() == 0)
        return true;

    for (unsigned int pos1 = 0, pos2 = 0; ; pos1 = pos2 + 1)
    {
        pos2 = m_patterns.find_first_of(";", pos1);
        if (pos2 != string::npos)
        {
            if (pos2 != m_patterns.length() - 1)
            {
                strncpy(pattern, m_patterns.c_str() + pos1, pos2 - pos1);
                pattern[pos2 - pos1] = 0;
                if (file_matches(m_shortName, pattern))
                    return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (pos1 != m_patterns.length())
            {
                if (file_matches(m_shortName, m_patterns.c_str() + pos1))
                    return true;
                else
                    return false;
            }
            return false;
        }
    }
}


