cmake_minimum_required(VERSION 2.6)

project(scheduler)

INCLUDE_DIRECTORIES(../inc)
ADD_DEFINITIONS("-DDLL_BUILT_AS_STATIC")
ADD_DEFINITIONS("-DTRACE")
ADD_DEFINITIONS("-DMIN_TASK=8 ")
ADD_DEFINITIONS("-DMAX_TASK=128 ")
ADD_EXECUTABLE( scheludler_func_task
	../src/Scheduler.cpp
	../src/Task.cpp
	../inc/IScheduler.h
	../inc/Scheduler.h
	../inc/ITask.h
	../inc/Task.h
	main.cpp
)

target_link_libraries( scheludler_func_task ${Boost_LIBRARIES})