cmake_minimum_required(VERSION 2.6)

project(scheduler)
INCLUDE(clar.cmake)
# Add OS specific build options
if(UNIX)
	add_definitions("-DLINUX -O2 -s -Wall")
endif(UNIX)

if(WIN32)
	if(MSVC_VERSION EQUAL 1700)
	  add_definitions(-D_VARIADIC_MAX=10)
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501")
	# force this option to ON so that Google Test will use /MD instead of /MT
	# /MD is now the default for Visual Studio, so it should be our default, too
	#option(gtest_force_shared_crt ON)
endif(WIN32)

INCLUDE_DIRECTORIES(../inc)
INCLUDE_DIRECTORIES(./)
INCLUDE_DIRECTORIES(${CLAR_ROOT})
INCLUDE_DIRECTORIES(${CLAR_ROOT}/clar)

ADD_DEFINITIONS("-DDLL_BUILT_AS_STATIC")
ADD_DEFINITIONS("-DTRACE")
ADD_DEFINITIONS("-DMIN_TASK=8 ")
ADD_DEFINITIONS("-DMAX_TASK=128 ")
ADD_EXECUTABLE( tests
	
	../src/Scheduler.cpp
	../src/Task.cpp
	../inc/IScheduler.h
	../inc/Scheduler.h
	../inc/ITask.h
	../inc/Task.h
	
	clar.suite
	${Clar}
	multi_task.cpp
	main.cpp
)

target_link_libraries( tests ${Boost_LIBRARIES})