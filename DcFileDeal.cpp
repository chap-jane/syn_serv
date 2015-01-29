#include "DcFileDeal.h"
#include "util/publiclib.h"

//ϵͳ�ļ���¼��󳤶�
const unsigned int MAX_FILERECORD_LEN = 2000;

FileDeal::FileDeal()
{
    m_fp = NULL ;
    m_efp =NULL ;
    m_lastchgid = "";
    m_lastinfotype = -1;
    m_lastrecord = NULL;

}


FileDeal::~FileDeal()
{
    if (m_lastrecord != NULL)
    {
        delete m_lastrecord;
        m_lastrecord = NULL;
    }

    if (m_fp != NULL)
    {
        close();
    }
}


void FileDeal::setfilepath(const char* filepath)
{
    strcpy(m_filepath, filepath);
}


void FileDeal::setbakfilepath(const char* bakfilepath)
{
    strcpy(m_bakfilepath, bakfilepath);
}

void FileDeal::setabnormalpath(const char* abnormalpath)
{
    strcpy(m_abnormalpath, abnormalpath);
}

void FileDeal::setinvalidpath(const char* invalidpath)
{
    strcpy(m_invalidpath, invalidpath);
}


void FileDeal::setfilename(const char* filename)
{
    strcpy(m_filename, filename);

}


void FileDeal::setseperator(const char* seperator)
{
    strcpy(m_seperator, seperator);
}

void FileDeal::writeErrFile(Info &info)
{
    char errfilefullname[MAX_FILENAME_LEN]= {0};
    char errfile[100]= {0};
    strcat(errfile,m_filename) ;
    strcat(errfile,".err") ;
    getfilefullname(m_abnormalpath,errfile,errfilefullname);

    if((m_efp=fopen(errfilefullname,"wb"))== NULL)
    {
        printf(" %s �½����ߴ�ʧ��\n",errfilefullname);
        return ;
    }


    for(int i = 0 ; i<info.size(); i++)
    {
        char line[MAX_FILERECORD_LEN] = "\0";
        Record* rec=info[i] ;
        for(int j = 0 ; j<rec->size() ; j++)
        {
            char c[100]= {0};
            sprintf(c,"%s|",(*rec)[j]);
            strcat(line,c);
        }
        line[strlen(line)-1] = '\n' ;
        fputs(line,m_efp) ;
    }

    if(m_efp != NULL)
    {
        if (fclose(m_efp) != 0)
        {
            printf("�ر������ļ�������,�ļ�����:=%s\n", errfilefullname);
        }
        m_efp = NULL;
    }

    return ;
}

int FileDeal::open()
{
    m_lastchgid = "";
    m_lastinfotype = -1;
    m_lastrecord = NULL;
    int ret =0 ;
    //��ȡ�����ļ�ȫ��
    char filefullname[MAX_FILENAME_LEN]= {0};
    getfilefullname(m_filepath, m_filename, filefullname);

    //�������ļ�
    int i = 0;
label1:
    if ((m_fp=fopen(filefullname, "rb")) == NULL)
    {
        ++i;
        sleep(5);
        if (i == 5)
        {
            printf("%s ��ʧ��\n",filefullname);
            ret = -1 ;
        }
        else
        {
            goto label1;
        }
    }
    return ret ;
}


void FileDeal::close()
{
    //�ر������ļ���
    if(m_fp != NULL)
    {
        if (fclose(m_fp) != 0)
        {
            printf("�ر������ļ�������,�ļ�����:=%s\n", m_filename);
        }
        m_fp = NULL;
    }
}


void FileDeal::remove()
{
    //ԭʼ�ļ��ƶ�������Ŀ¼����
    char filefullname[MAX_FILENAME_LEN]= {0};
    char bakfilefullname[MAX_FILENAME_LEN]= {0};
    getfilefullname(m_filepath, m_filename, filefullname);
    getfilefullname(m_bakfilepath, m_filename, bakfilefullname);
    if (rename(filefullname, bakfilefullname) != 0)
    {
        printf("�Ƴ��ļ���ʱ�����,ԭ�ļ�����:=%s,Ŀ���ļ�����Ϊ:=%s\n", filefullname, bakfilefullname);
    }
}

void FileDeal::removeInv()
{
    //����Ч���ļ��ƶ�����Ч��Ŀ¼����
    char filefullname[MAX_FILENAME_LEN]= {0};
    char invfilefullname[MAX_FILENAME_LEN]= {0};
    char invFileNme[128]= {0} ;
    sprintf(invFileNme,"%s.inv",m_filename);
    getfilefullname(m_filepath, m_filename, filefullname);
    getfilefullname(m_invalidpath, invFileNme, invfilefullname);
    if (rename(filefullname, invfilefullname) != 0)
    {
        printf("�Ƴ��ļ���ʱ�����,ԭ�ļ�����:=%s,Ŀ���ļ�����Ϊ:=%s\n", filefullname, invfilefullname);
    }
}


int FileDeal::getInfo(string &chgid, int &infotype, Info &info, char* errrecord)
{
    int iRecNum = 0;

    //ֵ��ʼ��
    chgid = "";
    infotype = -1;
    info.clear();

    char s[2]= {0};
    string str = m_filename ;
    string c = str.substr(0,1) ;
    strcpy(s,c.c_str()) ;

    if((strcmp("I",s) != 0) && (strcmp("A",s) != 0) && (strcmp("B",s) != 0) )
    {
        removeInv() ;
        return iRecNum ;
    }

    string filename= m_filename ;
    string szSystime;
    char szStartTime[15]= {0};
    PublicLib::GetTime(szStartTime,1) ;
    szSystime =szStartTime ;
    unsigned int ipos =filename.find_last_of("_") ;
    unsigned int cpos =filename.find_last_of(".") ;
    string stbuf =filename.substr(ipos+1,cpos-ipos-1) ;
    if((ipos == string::npos) || (cpos == string::npos) || ((cpos-ipos-1) != 14) ||(stbuf>szSystime))
    {
        removeInv() ;
        return iRecNum ;
    }

    //������λ�ȡ��¼��������ļ��ĵ�һ��
    if (m_lastrecord != NULL)
    {
        //��¼���Զ�����
        ++iRecNum;
        chgid = m_lastchgid;
        infotype = m_lastinfotype;
        info.push_back(m_lastrecord);
        m_lastrecord = NULL;
    }

    //��ȡ��¼
    string  tmpchgid = "" ;
    int     tmpinfotype = -1 ;
    char line[MAX_FILERECORD_LEN] = "\0";
    while(fgets(line, MAX_FILERECORD_LEN, m_fp) != NULL)
    {
        //ȥ�����һ�����з���
        if (line[strlen(line)-1] == '\n')
        {
            line[strlen(line)-1] = '\0';
        }

        //������ȡ���ļ�¼���ݣ��������ֱ������
        m_lastrecord = new Record();
        if (!parse(line, *m_lastrecord))
        {
            delete m_lastrecord;
            m_lastrecord = NULL;
            strcpy(errrecord, line);
            iRecNum = -1;
            break;
        }
        else
        {
            info.push_back(m_lastrecord) ;
            iRecNum++ ;
        }

    }

    return iRecNum ;
}



bool FileDeal::parse(char* line, Record &record)
{
    //���ַ���������Record
    string strline(line);

    string strsep(m_seperator);
    vector<string> sVector;
    split(strline, strsep, sVector);

    record.clear();
    for (int i=0; i<sVector.size(); ++i)
    {
        //ÿ���ֶ�����ȡ�����ֻ����100λ
        //record.push_back(sVector[i].substr(0,100).c_str());
        record.push_back(sVector[i].c_str());
    }

    //��Record�ֶν��м��
    int infotype = atoi(record[0]);
    if (infotype != 10 && infotype != 20 && infotype != 60 && infotype != 90 )
    {
        return false;
    }

    return true;
}


void FileDeal::getfilefullname(const char* filepath, const char* filename, char* filefullname)
{
    if (filepath[strlen(filepath)-1] != '/')
    {
        sprintf(filefullname, "%s/%s",filepath, filename);
    }
    else
    {
        sprintf(filefullname, "%s%s",filepath, filename);
    }
}


unsigned int FileDeal::split(const string& str,const string& delim,vector<string>& result)
{
    if(str.length() == 0) return 0;

    if (delim.length() <=0)
    {
        result.push_back(str);
        return 1;
    }

    unsigned int  c = 0;
    int  i = 0;
    //10|20120202112000|0001
    for (int  pos = 0; pos<str.length() ; pos = i+delim.length())
    {
        i = str.find(delim, pos);//���׵�ַ��ʼ����delim,�ҵ�����λ��,���򷵻�npos ;

        if (i == string::npos)
            result.push_back(str.substr(pos));
        else
        {
            if((i-pos)==0)
            {
                result.push_back("00");   //���ĳ���ֶε�ֵΪ�յĻ�,Ĭ��Ϊ00
            }
            else
            {
                result.push_back(str.substr(pos,i - pos));
            }
        }

        c++;
        if (i == string::npos) break;
    }

    return c;
};
