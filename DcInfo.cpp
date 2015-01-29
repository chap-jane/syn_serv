#include "DcInfo.h"

Info::Info()
{
}


Info::~Info()
{
    clear();
}


void Info::clear()
{
    for (int i=0; i<m_data.size(); ++i)
    {
        delete m_data[i];
        m_data[i] = NULL;
    }
    m_data.clear();
}


int Info::size()
{
    return m_data.size();
}


void Info::push_back(Record* record)
{
    m_data.push_back(record);
}


Record* Info::operator [](const int& i)
{
    return m_data[i];
}