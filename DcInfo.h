#ifndef _DC_INFO_H_
#define _DC_INFO_H_
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "DcRecord.h"

using namespace std ;
//��Ϣ��,�洢��¼��ļ���
class Info
{
public:
    Info();

    ~Info();

    void clear();

    int size();

    void push_back(Record* record) ;


    Record* operator [](const int& i);

private:

    vector<Record*> m_data;
};


#endif
