/*******************************************
*Copyrights  2002，深圳天源迪科计算机有限公司
*                   平台 项目组
*All rights reserved.
*
*Filename：
*       TThread.cpp
*Indentifier：
*       这里填入该文件的标识（参见软件配置管理）
*Description：
*       TThread 实现文件
*Version：
*       V1.0
*Author:
*       蒋述庭
*Finished：
*       2007年11月16日
*History:
*       (作者     日期     版本    说明)
*       文件修订历史纪录（一般情况下用中文）
********************************************/
#include <signal.h>
#include "TThread.h"

extern int errno;

/*! @function
********************************************************************************
@Class:TThread
@Fun: TThread
@Description: 构造函数
@Input:无
@Output:无
@Return: 无
@Author:jiangst
*******************************************************************************/
TThread::TThread():m_tno(0)
{

}

/*! @function
********************************************************************************
@Class:TThread
@Fun: ~TThread
@Description: 析构函数
@Input:无
@Output:无
@Return: 无
@Author:jiangst
*******************************************************************************/
TThread::~TThread()
{

}

/*! @function
********************************************************************************
@Class:TThread
@Fun: start
@Description: 启动线程
@Input:无
@Output:无
@Return: =0 成功
@Author:jiangst
*******************************************************************************/
int TThread::start()
{
    int ret = 0;
    //创建线程
    //return pthread_create(&m_tno, NULL, work, this);


    size_t size = 1024*1024*8;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize (&attr, size);

    //创建线程
    ret = pthread_create(&m_tno, &attr, work, this);
    pthread_attr_destroy(&attr);

    return ret ;
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: routine
@Description: 线程的运行函数，由派生类实现
@Input:无
@Output:无
@Return: 无
@Author:jiangst
*******************************************************************************/
int TThread::routine()
{
}

/*! @function
********************************************************************************
@Class:TThread
@Fun: self
@Description: 线程自身
@Input:无
@Output:无
@Return: 线程号
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
@Description: 线程线程相等
@Input:
    t:线程号
@Output:无
@Return:=0 成功
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
@Description: 分离线程
@Input:无
@Output:无
@Return:=0 成功
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
@Description: 连接线程
@Input:
    t:线程号
@Output:无
@Return: =0 成功
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
@Description: 线程退出
@Input:无
@Output:无
@Return:=0 成功
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
@Description: 取消线程
@Input:
    t:线程号
@Output:无
@Return: =0 成功
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
@Description: 销毁线程
@Input:无
@Output:无
@Return: =0 成功
@Author:jiangst
*******************************************************************************/
int TThread::destroy()
{
    return cancel(m_tno);
}

/*! @function
********************************************************************************
@Class:无
@Fun: TThread
@Description: TThread 对应的线程清理函数
@Input:
    pHandle:线程句柄
@Output:无
@Return: 无
@Author:jiangst
*******************************************************************************/
void  TThread::cleaner(void* pHandle)
{
    TThread* p = (TThread*)pHandle;
    delete p;
}


/*! @function
********************************************************************************
@Class:无
@Fun: work
@Description: 线程函数
@Input:
    p:线程函数的参数
@Output:无
@Return: 无
@Author:jiangst
*******************************************************************************/
void*  TThread::work(void* pHandle)
{
    TThread* pThread = (TThread *)pHandle;

    //注册线程处理函数
    pthread_cleanup_push(cleaner, pHandle);

    pThread->routine();

    //线程资源释放
    pthread_cleanup_pop(1);

    return NULL;
}
