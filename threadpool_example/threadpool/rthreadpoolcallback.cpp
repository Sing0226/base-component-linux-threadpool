#include "rthreadpoolcallback.h"
#include "rslogging.h"



RThreadPoolCallBack::RThreadPoolCallBack()
{
    RSLOG_DEBUG << "construct";
}


RThreadPoolCallBack::~RThreadPoolCallBack()
{
    RSLOG_DEBUG << "desconstruct";
}


void RThreadPoolCallBack::onJobBegin(int jobId, RBaseJob* pJob )
{
    RSLOG_DEBUG << "entry ...";
    RSLOG_DEBUG << "end";
} 

void RThreadPoolCallBack::onJobEnd(int jobId, RBaseJob* pJob)
{
    RSLOG_DEBUG << "entry ...";
    RSLOG_DEBUG << "end";
}

//如果尚未到达运行状态就被取消的Job，会由Pool调用这个函数
void RThreadPoolCallBack::onJobCancel(int jobId, RBaseJob* pJob)
{
    RSLOG_DEBUG << "entry ...";
    RSLOG_DEBUG << "end";
}

//Progress 和 Error 由 JobBase 的子类激发
void RThreadPoolCallBack::onJobProgress(int jobId, RBaseJob* pJob,  unsigned long long curPos,  unsigned long long totalSize)
{
    RSLOG_DEBUG << "entry ...";
    RSLOG_DEBUG << "end";
}

void RThreadPoolCallBack::onJobError(int jobId, RBaseJob* pJob, int errCode, const char* desc)
{
    RSLOG_DEBUG << "entry ...";
    RSLOG_DEBUG << "end";
}
