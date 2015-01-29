/*Copyrights  2011��������Դ�ϿƼ�������޹�˾
*                   ����ƽ̨��Ŀ��
*All rights reserved.
*
*Filename��
*       DcManager.cpp
*Indentifier��
*
*Description��
*       ͬ��������
*Version��
*       V1.0
*Author:
*       yancy
*Finished��
*       2012��2��20��
*History:
*       2012/2/20  V1.0 �ļ�����
********************************************/

#include "dccType.h"
#include "CDCCEntry.h"
#include "CDCCMsg.h"
#include "dccDef.h"
#include "CValue.h"
#include "CPackage.h"

#include "DcManager.h"
//#include "DcThreadProcessFileDeal.h"


const int MAX_RECV_TIME = 10000000; //recv��ʱʱ��


DcManager* DcManager::m_instance = NULL;

DcManager* DcManager::GetInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new DcManager();
    }
    return m_instance;
}

DcManager::DcManager()
{
    m_config = NULL;
    m_dccEntry = NULL;
    pThreadReSyn = NULL;

}

DcManager::~DcManager()
{
    if(NULL != m_instance)
    {
        delete m_instance;
        m_instance = NULL ;
    }
}

/****************************************************************************************
        *@input

        *@output

        *@return
                0:�ɹ�
                other:ʧ��

        *@description       DcManager���ʼ��
        ******************************************************************************************/
int DcManager::Init()
{

	memset(wBuf, 0, sizeof(wBuf));
	memset(buf, 0, sizeof(buf));

    /*��ȡ������ʵ��*/
    m_config = DcConfig::GetInstance();
    m_xml    = DcParseXml::GetInstance();
    if(!m_config)
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("Get Config failed")));
    }

    /*��ʼ�������ļ�*/
    if(m_config->Init())
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("Config Init failed")));
    }
    cout<<m_config->OCSOracleString<<endl ;
    /*��ʼ��DCC*/
    m_dccEntry = CDCCEntry::instance();
    if(m_dccEntry->init(m_config->m_serviceNo, DCC_HOST_UNKNOWN) != 0)
    {
        cout<<"init DCC error"<<endl;
        return -1;
    }
    if(m_xml->Init())
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("DcParseXml Init failed")));
        return -1 ;
    }
    //װ������
    if(m_xml->getDataType())
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("DcParseXml getDataType failed")));
        return -1 ;
    }

    m_pIndb=Indb::GetInstance();
    if(m_pIndb->Init())
    {
        ACE_DEBUG((MY_ERROR ACE_TEXT("Indb Init failed")));
        return -1 ;
    }

    pthread_mutex_init(&m_mutex_ReSyn, NULL);
	pthread_mutex_init(&m_mutex_Syn, NULL);

    DcThreadProcess *pThread = NULL;
    for(int i = 1; i <= m_config->m_threadCount; i++)
    {
        //pThread = NULL;
        pThread = new DcThreadProcess(this,m_dccEntry,m_config,i);
        if(pThread)
        {
            pThread->start();
            m_lstThread.push_back(pThread);
        }
    }

     pThreadReSynCCA = new DcThreadProcessReSynCCA(m_dccEntry,this,m_config);
    if(pThreadReSynCCA)
    {
        pThreadReSynCCA->start();
    }


    pThreadReSyn = new DcThreadProcessReSyn(m_dccEntry,m_config);
    if(pThreadReSyn)
    {
        pThreadReSyn->start();
    }
    int ret = 0;
    CDCCMsg* pMsg;
    while(1)
    {
        ret = m_dccEntry->recv(&pMsg,MAX_RECV_TIME);
        if(ret<0 || NULL ==pMsg)
        {
            ACE_DEBUG((MY_DEBUG_01 ACE_TEXT("DcThreadProcess::svc receive  DCCMsg timeout.\n")));
            continue ;
        }
		ACE_DEBUG((MY_DEBUG_01 ACE_TEXT("DcThreadProcess::svc get a DCC msg \n")));
#if 0
        lnQueueSize = m_pDCCEntry->getQueueSize();
        lnReciveMsgTime = PublicLib::getTimeUsec();
        lnReceiveOriMsgTime =  pMsg->getMsgBin().GetReceiveTime();
        lnParseMsgTime =lnReciveMsgTime - lnReceiveOriMsgTime;
        lnHssDateSize = pMsg->getHead().len;
#endif
        if(DcConfig::GetInstance()->m_nHssDumpFlag)
        {
            ACE_DEBUG((MY_DEBUG ACE_TEXT("receive a HSS Msg:")));
            pMsg->dump(buf);
            memset(wBuf,0x00,sizeof(wBuf));
            for(int i=0 ; i<160 ; i++)
            {
                int j=i*2048 ;
                memcpy(wBuf[i] ,buf+j,2048) ;
                if(wBuf[i][0] !='\0')
                {
                    ACE_DEBUG((MY_DEBUG ACE_TEXT("%s"),wBuf[i]));
                }
                else
                {
                    break ;
                }
            }
            ACE_DEBUG((MY_DEBUG ACE_TEXT("Msg end")));
        }
        char t_systime[21]= {0} ;
        PublicLib::GetTime(t_systime,6) ;
        SDCCHead& head = pMsg->head();
        ///���ܵ�����CCA
        if(head.sign<128)
        {
        	pthread_mutex_lock(&m_mutex_ReSyn);
        	m_QueBackSyn.push(pMsg);
			pthread_mutex_unlock(&m_mutex_ReSyn);
        }
		else
		{
			pthread_mutex_lock(&m_mutex_Syn);
			m_QueSyn.push(pMsg);
			pthread_mutex_unlock(&m_mutex_Syn);
		}
    }


    return 0;
}

/****************************************************************************************
        *@input

        *@output

        *@return
                0:�ɹ�
                other:ʧ��
        *@description       DcManager����
******************************************************************************************/

int DcManager::Destroy()
{
    for(list<DcThreadProcess*>::iterator it = m_lstThread.begin(); it != m_lstThread.end(); it++)
    {
        delete *it;
        *it = NULL;
    }
    if(pThreadReSyn !=NULL)
    {
        delete pThreadReSyn ;
        pThreadReSyn =NULL ;
    }

    if(m_xml !=NULL)
    {
        delete m_xml ;
        m_xml =NULL ;
    }
    m_dccEntry->destroy();
    m_config->Destroy();
    return 0;
}


CDCCMsg* DcManager::GetDCCMsgFromQueue()
{
	CDCCMsg* msg = NULL;
	pthread_mutex_lock(&m_mutex_Syn);

	if (!m_QueSyn.empty())
	{		
		msg = m_QueSyn.front();
		m_QueSyn.pop();		
	}
	pthread_mutex_unlock(&m_mutex_Syn);
    return msg;

}

CDCCMsg* DcManager::GetDCCMsgFromBackQueue()
{
    CDCCMsg* msg = NULL;
	pthread_mutex_lock(&m_mutex_ReSyn);
	if (!m_QueBackSyn.empty())
	{		
		msg = m_QueBackSyn.front();
		ACE_DEBUG((MY_DEBUG ACE_TEXT("deal dcc msg, left msg amount[%q]"),m_QueBackSyn.size()));
		m_QueBackSyn.pop();		
	}
	pthread_mutex_unlock(&m_mutex_ReSyn);
    return msg;
}



