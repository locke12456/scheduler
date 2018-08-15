cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

include(CheckCXXCompilerFlag)
project(scheduler)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add debug definitions
if(UNIX)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

	find_package (Threads)
	add_definitions("-DLINUX -O2 -s -Wall -Dlinux")

	if(COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
	    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no
			C++11 support. Please use a different C++ compiler.")
	endif()
endif(UNIX)

if(WIN32)
	if(MSVC_VERSION EQUAL 1700)
	  add_definitions(-D_VARIADIC_MAX=10)
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501 -D_ENABLE_ATOMIC_ALIGNMENT_FIX=1")
	# force this option to ON so that Google Test will use /MD instead of /MT
	# /MD is now the default for Visual Studio, so it should be our default, too
	#option(gtest_force_shared_crt ON)
endif(WIN32)

INCLUDE_DIRECTORIES(inc)
ADD_DEFINITIONS("-DDLL_EXPORTS=1")
ADD_DEFINITIONS("-DMIN_TASK=4 ")
ADD_DEFINITIONS("-DMAX_TASK=128 ")
add_library( scheduler STATIC
	src/Scheduler.cpp
	src/Task.cpp
	src/TaskStatus.cpp
	inc/IScheduler.h
	inc/Scheduler.h

	inc/ITask.h
	inc/Task.h
)
if(UNIX)
target_link_libraries( scheduler atomic)
endif(UNIX)
target_link_libraries( scheduler ${Boost_LIBRARIES})
ADD_SUBDIRECTORY(test)

