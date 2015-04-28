#include "Task.h"  
#include <boost/bind.hpp>
Task::Task(Task task, Time time) :
	_task(task)			,
	_tasking(false)  , _finished(false),
	_time(time),
	_wait(),
	_thread_mutex(), _mutex(),
	_thread_ready(false), _thread_stop(false), _thread_terminate(false),
	_worker()
{   
	Lock lock(_thread_mutex);
	_worker = boost::shared_ptr<Worker>(new Worker(boost::bind(&Task::_worker_thread, this)));
	if (!_thread_ready)
		_wait_thread_ready.wait(lock);
}                                                        
                              
Task::~Task()   
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

bool Task::DoTask()
{
	if (_tasking)return false;
	_wait.notify_one();
	_tasking = true;
	return true;
}
bool Task::SetTask(Task task, Time time)
{
	_time = time;
	_task = task;
	_finished = false;
	_tasking = false;
	_thread_stop = false;

	return true;
}
bool Task::StopTask()
{
	_finished = true;
	_tasking = false;
	_thread_stop = true;
	_time = Time(0);
	_wait.notify_all();
	return true;
}

bool Task::isTasking()
{
	return _tasking;
}

bool Task::isFinished()
{
	return _finished;
}

void Task::_do_task()
{
	if (!_task.empty()){
		_task();
		_task = NULL;
	}
}

void Task::_exception_handler()
{

	while (!_thread_stop)
	{
		if (_finished && !_tasking)	break;
	}
	if (_thread_stop)
		throw(std::runtime_error("thread_stop"));
}

void Task::_worker_thread()
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
