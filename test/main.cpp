#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "Task.h"
#include "Scheduler.h"
boost::mutex mut;
void task(std::string id) 
{
	boost::lock_guard<boost::mutex> lock(mut);
	std::cout << "task:"+id << std::endl;
}
int main() 
{
	Scheduler sch;
	for (int i = 1; i < MIN_TASK * 2; i++) {
		auto src = boost::lexical_cast<std::string>(i);
		auto func = boost::bind(task, src);
		sch.addTask(func, ITask::Seconds(1));
	}
	sch.RunAllTask();
	Sleep(5000);
	return 0;
}