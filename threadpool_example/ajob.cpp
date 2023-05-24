#include "ajob.h"
#include "rslogging.h"
#include <unistd.h>

AJob::AJob()
    : name("A")
{
    RSLOG_DEBUG << "construct class object " << name.c_str();
}

AJob::~AJob()
{
    RSLOG_DEBUG << "desconstruct class object " << name.c_str();
}

void AJob::_initialize()
{
    RSLOG_DEBUG << "job " << name.c_str() <<" initisalize";
}

// 在这个Run中通常需要循环 调用 GetJobWaitType 方法检测
int AJob::_run()
{
    RSLOG_DEBUG << "job " << name.c_str() <<" running...";
    sleep(10);
    return 0;
}

// 如果是new出来的，通常需要在 Finalize 中调用 delete this (除非又有另外的生存期管理容器)
void AJob::_finalize()
{
    RSLOG_DEBUG << "job " << name.c_str() <<" run end, finished!";
}

// 这个函数用于未运行的Job(直接取消或线程池停止), 用于清除内存等资源, 如 delete this 等
void AJob::_onCancelJob()
{
     RSLOG_DEBUG << "job " << name.c_str() <<" canceled!";
}
