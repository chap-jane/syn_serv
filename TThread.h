/*******************************************
*Copyrights  2002��������Դ�ϿƼ�������޹�˾
*                   ƽ̨ ��Ŀ��
*All rights reserved.
*
*Filename��
*       TThread.h
*Indentifier��
*       ����������ļ��ı�ʶ���μ�������ù���
*Description��
*       TThread ͷ�ļ�
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
#ifndef __TTHREAD_H__
#define __TTHREAD_H__
#include <pthread.h>
#include "logmgr/ACELogMsgCallbackCmn.h"
#include "logmgr/DCAceLogAgentCmn.h"

class TThread
{
public:

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
    TThread();

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
    virtual ~TThread();

public:

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
    virtual int start();

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
    //virtual int DealSyn();
    virtual int routine();

public:

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
    virtual  pthread_t self();

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: equal
    @Description: �߳��߳����
    @Input:
        t:�̺߳�
    @Output:��
    @Return: =0 �ɹ�
    @Author:jiangst
    *******************************************************************************/
    virtual  int equal(pthread_t t);

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: detach
    @Description: �����߳�
    @Input:��
    @Output:��
    @Return: =0 �ɹ�
    @Author:jiangst
    *******************************************************************************/
    virtual  int detach();

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
    virtual  int join(pthread_t t);

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
    virtual  int exit();

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
    virtual int cancel(pthread_t t);

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
    virtual int destroy();

private:

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: cleaner
    @Description: TThread ��Ӧ���߳�������
    @Input:
        pHandle:�߳̾��
    @Output:��
    @Return: ��
    @Author:jiangst
    *******************************************************************************/
    static void cleaner(void* pHandle);

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: work
    @Description: �̺߳���
    @Input:
        p:�̺߳����Ĳ���
    @Output:��
    @Return: ��
    @Author:jiangst
    *******************************************************************************/
    static void * work(void* pHandle);

private:

    pthread_t m_tno;                //�̺߳�
};

#endif
