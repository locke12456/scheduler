#include "Scheduler.h"   
#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>
#ifndef FOREACH
#define FOREACH BOOST_FOREACH
#endif
using namespace Utility;
Scheduler::Scheduler() : _tasks(NULL), _task_list(NULL) ,_pending(), _mutex()
{                          
	_tasks = new TaskQueue(MAX_TASK);
	_task_list = new TaskQueue(MAX_TASK);

	for (int i = MIN_TASK; i > 0;i--)
		_task_list->push(new ThreadTask(NULL, ITask::Time(1)));
}                             
                              
                              
Scheduler::~Scheduler()   
{      
	ITask * task = NULL;
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
	ThreadTask * task = NULL;
	while (_tasks->pop(task)){
		task->Start();
		_pending.push_back(task);
	}
	return true;
}

void Scheduler::RecoveryTask()
{
	guard lock(_mutex);
	TaskList tasks = _pending;
	//int index = 0;
	boost::hash<ThreadTask *> hashher;
	//size_t index;
	FOREACH(ITask * task, tasks)
	{
#ifdef TRACE
		auto index = hashher(task);
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

ITask& Scheduler::addTask(ITask::Task task, ITask::Time time)
{
	guard lock(_mutex);
	ThreadTask * get_task = 0;
	if (_task_list->pop(get_task))
	{
		if (!get_task->SetTask(task, time))
		{
			delete get_task;
			get_task = new ThreadTask(task, time);
		}
	}
	else {		
		get_task = new ThreadTask(task, time);
#ifdef TRACE
		std::cout << "tasking : " <<_pending.size() << std::endl;
#endif
	}

	_tasks->push(get_task);
	return *get_task;
}

void Scheduler::removeTask(ITask& task)
{
	task.Stop();
	_pending.remove(&task);
}

void Scheduler::cancelAllTask()
{
	guard lock(_mutex);
	TaskList tasks = _pending;
	FOREACH(ITask * task, tasks)
	{
		task->Stop();
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
	job->Stop();
}
