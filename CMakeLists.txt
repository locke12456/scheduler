cmake_minimum_required(VERSION 2.6)

project(scheduler)

INCLUDE(os_env.cmake)
INCLUDE(boost.cmake)
ADD_SUBDIRECTORY(scheludler)
ADD_SUBDIRECTORY(tests)