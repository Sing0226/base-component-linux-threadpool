#ifndef BJOB_H
#define BJOB_H
#include "rbasejob.h"


class BJob : public RBaseJob
{
public:
    BJob();
    virtual ~BJob();

protected:
    virtual void _initialize();

    // 在这个Run中通常需要循环 调用 GetJobWaitType 方法检测
    virtual int _run();

    // 如果是new出来的，通常需要在 Finalize 中调用 delete this (除非又有另外的生存期管理容器)
    virtual void _finalize();

    // 这个函数用于未运行的Job(直接取消或线程池停止), 用于清除内存等资源, 如 delete this 等
    virtual void _onCancelJob();


private:
    std::string name;
};

#endif // BJOB_H
