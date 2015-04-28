#ifndef __TASK_H__                 
#define __TASK_H__                 
#include "ITask.h"


#include "DLL_Export.h"
class DLL_EXPORT Task : public ITask
{                                              
public:        

	Task(Task,Time);
	~Task();   
	bool SetTask(Task task, Time time);
	virtual bool DoTask();
	virtual bool StopTask();
	virtual bool isTasking();
	virtual bool isFinished();
protected:
	virtual void _do_task();
	virtual void _exception_handler();
	virtual void _worker_thread();
private:
	typedef boost::lock_guard<boost::mutex> Guard;
	typedef boost::unique_lock<boost::mutex> Lock;
	typedef boost::mutex					Mutex;
	mutable boost::condition_variable	 _wait, _wait_thread_ready;
	mutable Mutex						 _mutex , _thread_mutex;
	bool								_thread_ready , _thread_stop , _thread_terminate;
	Task								_task;
	bool								_tasking, _finished;
	boost::shared_ptr<Worker>			_worker;
	Time								_time;
};                                             
#endif                                             
