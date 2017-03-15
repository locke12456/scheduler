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
				Stop();
				_worker->join(); 
			}
		}
	}
	catch (std::exception &error)
	{
		// TODO : implement exception handler
		std::cout << "Error : " << error.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknow error" << std::endl;
	}
}    

bool ThreadTask::Start()
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
bool ThreadTask::Stop()
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
// TODO : change to meta state machine
// http://www.boost.org/doc/libs/1_57_0/libs/msm/doc/HTML/ch03s02.html#d0e419
void ThreadTask::_worker_thread()
{
	Mutex time_mutex;
	Lock lock(_mutex);
	// prepare thread state
	Lock time_lock(time_mutex);
	_thread_ready = true;
	_wait_thread_ready.notify_all();
	// thread is ready
	do{
		//finished
		_finished = true;
		_tasking = false;
		// continue
		_wait.wait(lock);
		// start
		try{
			double time = static_cast<double>(_time.total_seconds());
			long wait_time = static_cast<long>(time * 1000);
			boost::this_thread::sleep(boost::posix_time::milliseconds(wait_time));
			try{
				if (!_thread_stop){ // true : cancel
					//started
					_do_task(); // exception : abort
					//finish
				}
			}
			catch ( std::exception & abort ) //aborted
			{
				// TODO : implement exception handler
				std::cout << "task aborted : " << abort.what()  << std::endl;
			}
			
		}
		catch (std::exception &error)
		{
			// TODO : implement exception handler
			std::cout << "Error : " << error.what() << std::endl;
		}
		catch (...)
		{
			std::cout << "unknow error" << std::endl;
		}
		// TODO : implement recovery all resources if task cancel or abort.
	} while (_thread_ready);
}
