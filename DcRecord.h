#ifndef _DC_RECORD_H_
#define _DC_RECORD_H_

#include <stdio.h>
#include <unistd.h>



//表最大列数
const unsigned int MAX_FIELD_NUM = 50;
//字段长度
const unsigned int MAX_FIELD_LEN = 101;
//目录的最大长度
const unsigned int MAX_FILEPATH_LEN = 100;
//文件名称的最大长度
const unsigned int MAX_FILENAME_LEN = 100;
//资料类型的最小值
const unsigned int MIN_INFOTYPE_VALUE = 0;
//资料类型的最大值
const unsigned int MAX_INFOTYPE_VALUE = 999;


//记录类
class Record
{
public:
    Record();

    ~Record();

    bool push_back(const char* fieldvalue) ;

    void clear();

    int size() ;

    char* operator [](const int& i) ;


private:
    char*   m_data[MAX_FIELD_NUM];
    int     m_maxFieldNum;
};


#endif
