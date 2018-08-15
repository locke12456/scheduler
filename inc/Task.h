#ifndef __TASK_H__                 
#define __TASK_H__                 
#include "ITask.h"
#include "TaskStatus.h"
#include "DLL_Export.h"
namespace Utility {
	class DLL_EXPORT ThreadTask : public ITask
	{
	public:

		ThreadTask(ITask::Task, ITask::Time);
		~ThreadTask();
		bool SetTask(ITask::Task task, ITask::Time time);
		virtual bool Start();
		virtual bool Stop();
		virtual bool Destroy();
		virtual bool isTasking();
		virtual bool isFinished();
	protected:
		virtual void _do_task();
		virtual void _exception_handler();
		virtual void _worker_thread();
	private:
		// TODO : refactor
		typedef boost::lock_guard<boost::mutex> Guard;
		typedef boost::unique_lock<boost::mutex> Lock;
		typedef boost::mutex					Mutex;
		mutable boost::condition_variable	 _wait, _wait_thread_ready;
		mutable Mutex						 _mutex, _thread_mutex;
		TaskStatus							_status;
		bool								_thread_ready, _thread_stop, _thread_terminated;
		ITask::Task							_task;
		bool								_tasking, _finished;
		boost::shared_ptr<Worker>			_worker;
		ITask::Time							_time;
	};
};
#endif                                             
