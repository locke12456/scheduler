#ifndef __ISCHEDULER_H__                 
#define __ISCHEDULER_H__   
#include "ITask.h"              
class IScheduler    
{                                              
public:      
	virtual ~IScheduler(){};
	virtual ITask& addTask(ITask::Task task, ITask::Time) = 0;
	virtual void removeTask(ITask& task) = 0;
	virtual bool RunAllTask() = 0;
	virtual void RecoveryTask() = 0;
	virtual void cancelAllTask() = 0;
	virtual void cancelTask(ITask*) = 0;
};                                             
#endif                                             
