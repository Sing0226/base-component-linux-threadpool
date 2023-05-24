#include "rbasejob.h"
#include "rslogging.h"
#include "rpevents.h"
#include "rthreadpool.h"


///////////////////////////////////////////// TPBaseJob ///////////////////////////////////////////////////
//  初始化
RBaseJob::RBaseJob(int job_priority):
job_priority_(job_priority)
{
    job_id_                     = 0;
	thread_pool_				= NULL;
	stop_job_event_				= NULL;
	sync_cancel_job_event_		= CreateEvent(true, false);
}

RBaseJob::~RBaseJob()
{
	if (sync_cancel_job_event_)
	{
		DestroyEvent(sync_cancel_job_event_);
		sync_cancel_job_event_ = NULL;
	}
}

bool RBaseJob::operator< (const RBaseJob& other) const
{
	COMPARE_MEM_LESS(job_priority_, other);
	COMPARE_MEM_LESS(job_id_, other);

	return true;
}

int RBaseJob::getJobId() const
{
	return job_id_;
}

bool RBaseJob::requestCancel()
{
	RSLOG_DEBUG << "entry ...";
	RSLOG_DEBUG << "set stop event";
	SetEvent(stop_job_event_);
	RSLOG_DEBUG << "end";

    return true;
}

void RBaseJob::_notifyCancel()
{	
	RSLOG_DEBUG << "entry ...";
	if (thread_pool_)
	{
		RSLOG_DEBUG << "notify job cancel";
		thread_pool_->_notifyJobCancel(this);
	}
    RSLOG_DEBUG << "end";
}


void RBaseJob::_initialize()
{
	// TPASSERT(NULL == stop_job_event_);
	RSLOG_DEBUG << "entry ...";
	if (stop_job_event_)
	{
		DestroyEvent(stop_job_event_);
	}
	stop_job_event_ = NULL;
	RSLOG_DEBUG << "end";
}

void RBaseJob::signal_sync_cancel_job_event()
{
	RSLOG_DEBUG << "signal_sync_cancel_job_event entry...";
	if (sync_cancel_job_event_ != NULL)
	{
		SetEvent(sync_cancel_job_event_);
	}
	RSLOG_DEBUG << "signal_sync_cancel_job_event leave";
}

smart_event_t RBaseJob::get_cancel_job_event()
{
	return sync_cancel_job_event_;
}
