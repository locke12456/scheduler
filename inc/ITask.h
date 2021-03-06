#ifndef __ITASK_H__                 
#define __ITASK_H__  
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "DLL_Export.h"
class DLL_EXPORT ITask
{                                              
public:  
	typedef boost::thread Worker;
	typedef boost::function<void()> Task;
	typedef boost::posix_time::seconds Seconds;
	typedef boost::posix_time::minutes Minutes;
	typedef boost::posix_time::hours Hours;
	typedef Seconds Time;
	virtual ~ITask(){};
	virtual bool Start() = 0;
	virtual bool Stop() = 0;
	virtual bool Destroy() = 0;
	virtual bool isTasking() = 0;
	virtual bool isFinished() = 0;
};                                             
#endif                                             
