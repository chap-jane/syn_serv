/*******************************************
*Copyrights  2002��������Դ�ϿƼ�������޹�˾
*                   ƽ̨ ��Ŀ��
*All rights reserved.
*
*Filename��
*       TThread.cpp
*Indentifier��
*       ����������ļ��ı�ʶ���μ�������ù���
*Description��
*       TThread ʵ���ļ�
*Version��
*       V1.0
*Author:
*       ����ͥ
*Finished��
*       2007��11��16��
*History:
*       (����     ����     �汾    ˵��)
*       �ļ��޶���ʷ��¼��һ������������ģ�
********************************************/
#include <signal.h>
#include "TThread.h"

extern int errno;

/*! @function
********************************************************************************
@Class:TThread
@Fun: TThread
@Description: ���캯��
@Input:��
@Output:��
@Return: ��
@Author:jiangst
*******************************************************************************/
TThread::TThread():m_tno(0)
{

}

/*! @function
********************************************************************************
@Class:TThread
@Fun: ~TThread
@Description: ��������
@Input:��
@Output:��
@Return: ��
@Author:jiangst
*******************************************************************************/
TThread::~TThread()
{

}

/*! @function
********************************************************************************
@Class:TThread
@Fun: start
@Description: �����߳�
@Input:��
@Output:��
@Return: =0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::start()
{
    int ret = 0;
    //�����߳�
    //return pthread_create(&m_tno, NULL, work, this);


    size_t size = 1024*1024*8;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, size);

    //�����߳�
    ret = pthread_create(&m_tno, &attr, work, this);
    pthread_attr_destroy(&attr);

    return ret ;
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: routine
@Description: �̵߳����к�������������ʵ��
@Input:��
@Output:��
@Return: ��
@Author:jiangst
*******************************************************************************/
int TThread::routine()
{
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: self
@Description: �߳�����
@Input:��
@Output:��
@Return: �̺߳�
@Author:jiangst
*******************************************************************************/
pthread_t TThread::self()
{
    if (!m_tno)
    {
        m_tno = pthread_self();
    }

    return m_tno;
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: equal
@Description: �߳��߳����
@Input:
    t:�̺߳�
@Output:��
@Return:=0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::equal(pthread_t t)
{
    int ret = 0;
    ret = pthread_equal(m_tno, t);
    return (ret)?0:-1;
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: detach
@Description: �����߳�
@Input:��
@Output:��
@Return:=0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::detach()
{
    return pthread_detach(m_tno);
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: join
@Description: �����߳�
@Input:
    t:�̺߳�
@Output:��
@Return: =0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::join(pthread_t t)
{
    return pthread_join(t, NULL);
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: exit
@Description: �߳��˳�
@Input:��
@Output:��
@Return:=0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::exit()
{
    int ret = 0;
    pthread_exit((void *)&ret);
    return ret;
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: cancel
@Description: ȡ���߳�
@Input:
    t:�̺߳�
@Output:��
@Return: =0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::cancel(pthread_t t)
{
    return  pthread_cancel(t);
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: destory
@Description: �����߳�
@Input:��
@Output:��
@Return: =0 �ɹ�
@Author:jiangst
*******************************************************************************/
int TThread::destroy()
{
    return cancel(m_tno);
}

/*! @function
********************************************************************************
@Class:��
@Fun: TThread
@Description: TThread ��Ӧ���߳�������
@Input:
    pHandle:�߳̾��
@Output:��
@Return: ��
@Author:jiangst
*******************************************************************************/
void  TThread::cleaner(void* pHandle)
{
    TThread* p = (TThread*)pHandle;
    delete p;
}


/*! @function
********************************************************************************
@Class:��
@Fun: work
@Description: �̺߳���
@Input:
    p:�̺߳����Ĳ���
@Output:��
@Return: ��
@Author:jiangst
*******************************************************************************/
void*  TThread::work(void* pHandle)
{
    TThread* pThread = (TThread *)pHandle;

    //ע���̴߳�����
    pthread_cleanup_push(cleaner, pHandle);

    pThread->routine();

    //�߳���Դ�ͷ�
    pthread_cleanup_pop(1);

    return NULL;
}
