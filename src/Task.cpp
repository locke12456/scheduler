#include "Task.h"  
#include <boost/bind.hpp>
ThreadTask::ThreadTask(ITask::Task task, ITask::Time time) :
	_task(task)			,
	_tasking(false)  , _finished(false),
	_time(time),
	_wait(),
	_thread_mutex(), _mutex(),
	_thread_ready(false), _thread_stop(false), _thread_terminate(false),
	_worker()
{   
	Lock lock(_thread_mutex);
	_worker = boost::shared_ptr<Worker>(new Worker(boost::bind(&ThreadTask::_worker_thread, this)));
	if (!_thread_ready)
		_wait_thread_ready.wait(lock);
}                                                        
                              
ThreadTask::~ThreadTask()   
{
	try{
		if (_worker.get()){
			
			if (_worker->joinable()){
				_thread_ready = false;
				StopTask();
				_worker->join(); 
			}
		}
	}
	catch (std::exception &error)
	{
		std::cout << "Error : " << error.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknow error" << std::endl;
	}
}    

bool ThreadTask::DoTask()
{
	if (_tasking)return false;
	_wait.notify_one();
	_tasking = true;
	return true;
}
bool ThreadTask::SetTask(ITask::Task task, ITask::Time time)
{
	_time = time;
	_task = task;
	_finished = false;
	_tasking = false;
	_thread_stop = false;

	return true;
}
bool ThreadTask::StopTask()
{
	_finished = true;
	_tasking = false;
	_thread_stop = true;
	_time = Time(0);
	_wait.notify_all();
	return true;
}

bool ThreadTask::isTasking()
{
	return _tasking;
}

bool ThreadTask::isFinished()
{
	return _finished;
}

void ThreadTask::_do_task()
{
	if (!_task.empty()){
		_task();
		_task = NULL;
	}
}

void ThreadTask::_exception_handler()
{

	while (!_thread_stop)
	{
		if (_finished && !_tasking)	break;
	}
	if (_thread_stop)
		throw(std::runtime_error("thread_stop"));
}

void ThreadTask::_worker_thread()
{
	Mutex time_mutex;
	Lock lock(_mutex);
	Lock time_lock(time_mutex);
	_thread_ready = true;
	_wait_thread_ready.notify_all();
	do{
		_finished = true;
		_tasking = false;
		_wait.wait(lock);
		try{
		
			double time = static_cast<double>(_time.total_seconds());
			long wait_time = static_cast<long>(time * 1000);
			boost::this_thread::sleep(boost::posix_time::milliseconds(wait_time));
			try{
				if (!_thread_stop){
					_do_task();
				}
			}
			catch ( std::exception & abort )
			{
				std::cout << "task aborted : " << abort.what()  << std::endl;
			}
			
		}
		catch (std::exception &error)
		{
			std::cout << "Error : " << error.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "unknow error" << std::endl;
		}
	
	} while (_thread_ready);
}
