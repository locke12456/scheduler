/* multi_task.cpp for the "multi_task" suite */
#include "extern_c.h"
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "Task.h"
#include "Scheduler.h"
boost::mutex mut;
Scheduler * sch = NULL;
void task(std::string id)
{
	boost::lock_guard<boost::mutex> lock(mut);
	std::cout << "task:" + id << std::endl;
}

void test_multi_task__initialize(void)
{
	sch = new Scheduler();
	cl_assert_(sch != NULL, "No memory left?");
	for (int i = 1; i < MIN_TASK * 2; i++) {
		auto src = boost::lexical_cast<std::string>(i);
		auto func = boost::bind(task, src);
		sch->addTask(func, ITask::Seconds(1));
	}
}

void test_multi_task__cleanup(void)
{
	delete sch;
}

void test_multi_task__make_sure_math_still_works(void)
{
	sch->RunAllTask();
	Sleep(5000);

	cl_assert_(sch != NULL, "Success.");
}