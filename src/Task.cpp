#include "Task.h"  
#include <boost/bind.hpp>
using namespace Utility;
ThreadTask::ThreadTask(ITask::Task task, ITask::Time time) :
	_task(task)			,
	_tasking(false)  , _finished(false),
	_time(time),
	_wait(),
	_thread_mutex(), _mutex(),
	_thread_ready(false), _thread_stop(false), _thread_terminated(false),
	_worker(), _status()
{   
	Lock lock(_thread_mutex);
	_status.Initialize();
	_worker = boost::shared_ptr<Worker>(new Worker(boost::bind(&ThreadTask::_worker_thread, this)));
	if (!_thread_ready)
	//if (_status.GetState().GetIndex() != typeid(TaskStatus::Ready))
		_wait_thread_ready.wait(lock);
}                                                        
                              
ThreadTask::~ThreadTask()   
{
	try{
		if (!_thread_terminated && _worker.get()){
			
			if (_worker->joinable()){
				Destroy();
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
	if (_status.GetState().GetIndex() == typeid(TaskStatus::Running))return false;
	_status.SetState(typeid(TaskStatus::Start));
	_wait.notify_one();
	_tasking = true;
	return true;
}
bool ThreadTask::SetTask(ITask::Task task, ITask::Time time)
{
	if (_thread_terminated)return false;
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
	if (_status.GetState().GetIndex() == typeid(TaskStatus::Running) && _worker->joinable()) {
		_status.SetState(typeid(TaskStatus::Stop));
		_worker->interrupt();
	}
	return true;
}

bool ThreadTask::Destroy()
{
	Stop();
	if (_worker->joinable()) {
		_status.SetState(typeid(TaskStatus::Destroy));
		_worker->interrupt();
		_worker->join();
		_status.SetState(typeid(TaskStatus::Terminated));
	}
	return true;
}

bool ThreadTask::isTasking()
{
	return _status.GetState().GetIndex() == typeid(TaskStatus::Running);
}

bool ThreadTask::isFinished()
{
	return _status.GetState().GetIndex() == typeid(TaskStatus::Idle);
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
	_status.SetState(typeid(TaskStatus::Ready));
	_wait_thread_ready.notify_all();
	// thread is ready
	do{
		//finished
		_status.SetState(typeid(TaskStatus::Idle));
		// continue
		_wait.wait(lock);
		// start
		try{
			double time = static_cast<double>(_time.total_seconds());
			long wait_time = static_cast<long>(time * 1000);
			boost::this_thread::sleep(boost::posix_time::milliseconds(wait_time));
			try{
				_status.SetState(typeid(TaskStatus::Running));
				if (!_thread_stop){ // true : cancel
					//started
					_do_task(); // exception : abort
					//finish
				}
				_status.SetState(typeid(TaskStatus::Stopped));
			}
			catch ( boost::thread_interrupted & interrupted ) //aborted
			{
				std::cout << "task interrupted .. " << std::endl;
				_task = NULL;
				if(_status.GetState().GetIndex() == typeid(TaskStatus::Destroy))
					break;
			}
			catch (std::exception & abort) //aborted
			{
				// TODO : implement exception handler
				std::cout << "task aborted : " << abort.what() << std::endl;
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
	_thread_terminated = true;
}
