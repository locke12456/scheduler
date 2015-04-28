#include "Scheduler.h"   
#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>
#ifndef FOREACH
#define FOREACH BOOST_FOREACH
#endif

Scheduler::Scheduler() : _tasks(NULL), _task_list(NULL) ,_pending(), _mutex()
{                          
	_tasks = new TaskQueue(MAX_TASK);
	_task_list = new TaskQueue(MAX_TASK);

	for (int i = MIN_TASK; i > 0;i--)
		_task_list->push(new Task(NULL, ITask::Time(1)));
}                             
                              
                              
Scheduler::~Scheduler()   
{      
	Task * task = NULL;
	while (_tasks->pop(task))delete task;
	while (_task_list->pop(task))delete task;
	TaskList tasks = _pending;
	FOREACH( task, tasks)
	{
		delete task;
	}
	delete _task_list;
	delete _tasks;
} 

bool Scheduler::RunAllTask()
{
	guard lock(_mutex);
	Task * task = NULL;
	while (_tasks->pop(task)){
		task->DoTask();
		_pending.push_back(task);
	}
	return true;
}

void Scheduler::RecoveryTask()
{
	guard lock(_mutex);
	TaskList tasks = _pending;
	//int index = 0;
	boost::hash<Task *> hashher;
	//size_t index;
	FOREACH(Task * task, tasks)
	{
#ifdef TRACE
		index = hashher(task);
		std::cout << "index : "<< index <<" , task status : " << ( task->isFinished() ?"finished" : "running" ) << std::endl;
#endif
		if (NULL != task && task->isFinished())
		{
			_pending.remove(task);
			_task_list->push(task);
#ifdef TRACE
			std::cout << "task removed index : " << index << std::endl;
#endif
		}
	}
#ifdef TRACE
	std::cout << "panding tasking : " << _pending.size() << std::endl;
#endif
}

void Scheduler::addTask(ITask::Task task, ITask::Time time)
{
	guard lock(_mutex);
	Task * get_task;
	if (_task_list->pop(get_task))
	{
		get_task->SetTask(task, time);
		_tasks->push(get_task);
	}
	else {		
		_tasks->push(new Task(task, time));
#ifdef TRACE
		std::cout << "tasking : " <<_pending.size() << std::endl;
#endif
	}
	
	//else
	//	_tasks->push( new Task(task, time) );
}

void Scheduler::cancelAllTask()
{
	guard lock(_mutex);
	TaskList tasks = _pending;
	FOREACH(Task * task, tasks)
	{
		task->StopTask();
		_pending.remove(task);
		_task_list->push(task);
	}
#ifdef TRACE
	std::cout << "panding tasking : " << _pending.size() << std::endl;
#endif
}

void Scheduler::cancelTask(ITask* job)
{
	_cancel(job);
}

void Scheduler::_cancel(ITask* job)
{
	job->StopTask();
}
