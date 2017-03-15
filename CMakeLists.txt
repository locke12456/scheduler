cmake_minimum_required(VERSION 2.6)

project(scheduler)

INCLUDE(os_env.cmake)
INCLUDE(boost.cmake)
INCLUDE_DIRECTORIES(inc)
ADD_DEFINITIONS("-DMIN_TASK=4 ")
ADD_DEFINITIONS("-DMAX_TASK=128 ")
add_library( scheludler
	src/Scheduler.cpp
	src/Task.cpp

	inc/IScheduler.h
	inc/Scheduler.h

	inc/ITask.h
	inc/Task.h
)

target_link_libraries( scheludler ${Boost_LIBRARIES})
ADD_SUBDIRECTORY(test)