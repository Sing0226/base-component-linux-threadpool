#ifndef _THREAD_POOL_CALLBACK_H__
#define _THREAD_POOL_CALLBACK_H__

class RBaseJob;



// 回调函数
class RThreadPoolCallBack
{
public:

	RThreadPoolCallBack();

	virtual ~RThreadPoolCallBack();

	//当Job运行起来以后，会由 Pool 激发 Begin 和 End 两个函数

	virtual void onJobBegin(int jobId, RBaseJob* pJob );

	virtual void onJobEnd(int jobId, RBaseJob* pJob);

	//如果尚未到达运行状态就被取消的Job，会由Pool调用这个函数
	virtual void onJobCancel(int jobId, RBaseJob* pJob);


	//Progress 和 Error 由 JobBase 的子类激发
	virtual void onJobProgress(int jobId , RBaseJob* pJob,  unsigned long long curPos,  unsigned long long totalSize);


	virtual void onJobError(int jobId , RBaseJob* pJob, int errCode, const char* desc);


};

#endif
