#ifndef _TP_BASE_JOB_H__
#define _TP_BASE_JOB_H__

#include "rthreadpooldefine.h"

//  前向声明，
//! 具有以下特点：
//  1.能自动根据任务和线程的多少在 最小/最大 线程个数之间调整
//  2.能方便的对单个任务进行取消，如任务尚未运行则由框架代码处理，如任务已经运行，则需要 JobBase 的子类根据 GetJobWaitType 的返回值进行处理
//  3.能对整个线程池进行 暂停、继续、停止 处理 -- 需要 JobBase 的子类根据 GetJobWaitType 的返回值进行处理
//  4.支持回调方式的反馈通知( Progress/Error 等)
//  5.使用的是微软的基本API，能支持WinXP、Vista、Win7等各种操作系统
class RThreadPool;
struct smart_event_t_;
typedef struct smart_event_t_ *smart_event_t;


//////////////////////////////////////////////////////////////////////////
// 定义Job基类,后期可以用模板作为参数也行，这个就交给张方去实现吧。

class RBaseJob
{
    friend class RThreadPool;   //允许Threadpool设置 m_pThreadPool/m_nJobIndex 的值

public:

	RBaseJob(int job_priority = 0);
	virtual ~RBaseJob();

	//! 比较Job的优先级大小，用于确定在 Waiting 容器中的队列， 排序依据为 Priority -> Index
	bool operator < (const RBaseJob& other) const;

    int getJobId() const;

	//如果Job正在运行过程中被取消，会调用这个方法
	bool requestCancel();


protected:

	//这个三个函数一组, 用于运行起来的Job： if( Initialize ){ Run -> Finalize }
	virtual void _initialize();

	// 在这个Run中通常需要循环 调用 GetJobWaitType 方法检测
	virtual int _run() = 0;

	// 如果是new出来的，通常需要在 Finalize 中调用 delete this (除非又有另外的生存期管理容器)
	virtual void _finalize() = 0;

	// 这个函数用于未运行的Job(直接取消或线程池停止), 用于清除内存等资源, 如 delete this 等
	virtual void _onCancelJob() = 0;

protected:
	virtual void _notifyCancel();

	// 外界传入
	void signal_sync_cancel_job_event();
	smart_event_t get_cancel_job_event();


private:

	//设置为私有的变量和方法，即使是子类也不要直接更改，由Pool调用进行控制
	int		job_priority_;

	int		job_id_;

	smart_event_t	stop_job_event_;					// 停止Job的事件，该变量将由Pool创建和释放(TODO:Pool中缓存?)
	smart_event_t	sync_cancel_job_event_;				// 同步event

	RThreadPool* thread_pool_;
};


#endif

