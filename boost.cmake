# Find Boost library
set(BOOST_INCLUDEDIR ${CMAKE_SOURCE_DIR}/lib/include) 
set(BOOST_LIBRARYDIR ${BOOST_LIBS_PATH}/boost)
set(BOOST_ROOT ${BOOST_LIBS_PATH}/boost)
message (${BOOST_LIBRARYDIR})
set(Boost_USE_STATIC_LIBS        ON )    # default OFF
set(Boost_USE_MULTITHREADED      ON )    # default ON
set(Boost_USE_STATIC_RUNTIME     OFF)    # default Platform dependent

find_package(Boost 1.59.0 REQUIRED
    thread
    filesystem
    date_time
    regex
    system
    )

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  #message(${Boost_LIBRARIES})
endif()