#include <string.h>
#include "DcRecord.h"



Record::Record()
{
    for (int i=0; i<MAX_FIELD_NUM; ++i)
    {
        m_data[i] = new char[MAX_FIELD_LEN];
    }
    m_maxFieldNum = 0;

}

Record::~Record()
{
    for (int i=0; i<MAX_FIELD_NUM; ++i)
    {
        delete m_data[i];
        m_data[i] = NULL;
    }
    m_maxFieldNum = 0;
}


bool Record::push_back(const char* fieldvalue)
{
    if(m_maxFieldNum < (MAX_FIELD_NUM-1))
    {
        strcpy(m_data[m_maxFieldNum], fieldvalue);
        m_maxFieldNum++;
        return true;
    }
    else
    {
        return false;
    }
}


void Record::clear()
{
    m_maxFieldNum = 0;
}


int Record::size()
{
    return m_maxFieldNum;
}


char* Record::operator [](const int& i)
{
    if (i <= m_maxFieldNum)
    {
        return m_data[i];
    }
    return 0;
}
