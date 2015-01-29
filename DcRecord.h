#ifndef _DC_RECORD_H_
#define _DC_RECORD_H_

#include <stdio.h>
#include <unistd.h>



//���������
const unsigned int MAX_FIELD_NUM = 50;
//�ֶγ���
const unsigned int MAX_FIELD_LEN = 101;
//Ŀ¼����󳤶�
const unsigned int MAX_FILEPATH_LEN = 100;
//�ļ����Ƶ���󳤶�
const unsigned int MAX_FILENAME_LEN = 100;
//�������͵���Сֵ
const unsigned int MIN_INFOTYPE_VALUE = 0;
//�������͵����ֵ
const unsigned int MAX_INFOTYPE_VALUE = 999;


//��¼��
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
