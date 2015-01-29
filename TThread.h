/*******************************************
*Copyrights  2002，深圳天源迪科计算机有限公司
*                   平台 项目组
*All rights reserved.
*
*Filename：
*       TThread.h
*Indentifier：
*       这里填入该文件的标识（参见软件配置管理）
*Description：
*       TThread 头文件
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
    @Description: 构造函数
    @Input:无
    @Output:无
    @Return: 无
    @Author:jiangst
    *******************************************************************************/
    TThread();

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
    virtual ~TThread();

public:

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
    virtual int start();

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
    //virtual int DealSyn();
    virtual int routine();

public:

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
    virtual  pthread_t self();

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: equal
    @Description: 线程线程相等
    @Input:
        t:线程号
    @Output:无
    @Return: =0 成功
    @Author:jiangst
    *******************************************************************************/
    virtual  int equal(pthread_t t);

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: detach
    @Description: 分离线程
    @Input:无
    @Output:无
    @Return: =0 成功
    @Author:jiangst
    *******************************************************************************/
    virtual  int detach();

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
    virtual  int join(pthread_t t);

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
    virtual  int exit();

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
    virtual int cancel(pthread_t t);

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
    virtual int destroy();

private:

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: cleaner
    @Description: TThread 对应的线程清理函数
    @Input:
        pHandle:线程句柄
    @Output:无
    @Return: 无
    @Author:jiangst
    *******************************************************************************/
    static void cleaner(void* pHandle);

    /*! @function
    ********************************************************************************
    @Class:TThread
    @Fun: work
    @Description: 线程函数
    @Input:
        p:线程函数的参数
    @Output:无
    @Return: 无
    @Author:jiangst
    *******************************************************************************/
    static void * work(void* pHandle);

private:

    pthread_t m_tno;                //线程号
};

#endif
