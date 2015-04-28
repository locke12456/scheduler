#ifndef __SCHEDULER_H__                 
#define __SCHEDULER_H__ 

#include <list>
#include "IScheduler.h" 
#include "Task.h"
#include <boost/lockfree/queue.hpp>
#include "DLL_Export.h"
class DLL_EXPORT Scheduler : public IScheduler
{                                              
public:     
	Scheduler();                               
	virtual ~Scheduler();  
	virtual void addTask(ITask::Task task, ITask::Time);
	virtual bool RunAllTask();
	virtual void RecoveryTask();
	virtual void cancelAllTask();
	virtual void cancelTask(ITask*);
private:

	typedef boost::lock_guard<boost::mutex> guard;
	typedef boost::shared_ptr<Task> Task;
	ITask* _generator(Task::Task);
	virtual void _cancel(ITask*);
	typedef boost::lockfree::queue< Task * > TaskQueue;
	typedef std::list< Task * > TaskList;
	boost::mutex		_mutex;
	TaskQueue*			_tasks;
	TaskQueue*			_task_list;
	TaskList			_pending;
};                                             
#endif                                             
