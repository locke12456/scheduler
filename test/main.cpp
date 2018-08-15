#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/foreach.hpp>
#include "Task.h"
#include "Scheduler.h"
using namespace Utility;
boost::mutex mut;
float do_something() {
	//boost::lock_guard<boost::mutex> lock(mut);
	static float a = 1;
	a = a * 1000 * -0.123456789;
	return a;
}
void task(std::string id) 
{
	//boost::this_thread::interruption_enabled();
	//
	//try {
		while (1) {
			std::cout << "task:" + id << std::endl;
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
			std::cout << "compute floating:" << do_something() << std::endl;
			boost::this_thread::yield();
			boost::this_thread::interruption_point();
		}
	//}
}
int main() 
{
	std::list<ITask*> tasks;
	Scheduler sch;
	for (int i = 1; i < MIN_TASK * 2; i++) {
		auto src = boost::lexical_cast<std::string>(i);
		auto func = boost::bind(task, src);
		ITask& task = sch.addTask(func, ITask::Seconds(1));
		tasks.push_back(&task);
	}
	sch.RunAllTask();
	boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
	BOOST_FOREACH(ITask* task, tasks) {
		task->Stop();
	}
	boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));

	sch.RecoveryTask();
	std::list<ITask*> tasks2;
	for (int i = 1; i < MIN_TASK * 2; i++) {
		auto src = boost::lexical_cast<std::string>(i);
		auto func = boost::bind(task, src);
		ITask& task = sch.addTask(func, ITask::Seconds(1));
		tasks2.push_back(&task);
	}

	sch.RunAllTask();
	boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
	BOOST_FOREACH(ITask* task, tasks2) {
		task->Destroy();
	}
	int in;
	std::cin>>in;
	return 0;
}

//#include <boost/thread.hpp>
//#include <iostream>
//
//using namespace std;
//void func() {
//	for (;;) {
//		try
//		{
//			// Sleep and check for interrupt.
//			// To check for interrupt without sleep,
//			// use boost::this_thread::interruption_point()
//			// which also throws boost::thread_interrupted
//			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
//		}
//		catch (boost::thread_interrupted&)
//		{
//			cout << "Thread is stopped" << endl;
//			return;
//		}
//	}
//}
//void ThreadFunction()
//{
//	int counter = 0;
//
//	for (;;)
//	{
//		cout << "thread iteration " << ++counter << " Press Enter to stop" << endl;
//
//		try
//		{
//
//			// Sleep and check for interrupt.
//			// To check for interrupt without sleep,
//			// use boost::this_thread::interruption_point()
//			// which also throws boost::thread_interrupted
//			boost::this_thread::sleep(boost::posix_time::milliseconds(500));
//		}
//		catch (boost::thread_interrupted&)
//		{
//			cout << "Thread is stopped" << endl;
//			return;
//		}
//	}
//}
//
//int main()
//{
//	// Start thread
//	boost::thread t(&ThreadFunction);
//
//	// Wait for Enter 
//	char ch;
//	cin.get(ch);
//
//	// Ask thread to stop
//	t.interrupt();
//
//	// Join - wait when thread actually exits
//	t.join();
//	cout << "main: thread ended" << endl;
//
//	return 0;
//}