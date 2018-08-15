#include "TaskStatus.h"
#include <boost/thread.hpp>
using namespace Utility;

TaskStatus::TaskStatus() : _modes(), _current(0)
{
}
TaskStatus::~TaskStatus()
{
}

void TaskStatus::Initialize()
{
	_init_state<TaskStatus::Start>();
	_init_state<TaskStatus::Stop>();
	_init_state<TaskStatus::Destroy>();
	_init_state<TaskStatus::Ready>();
	_init_state<TaskStatus::Running>();
	_init_state<TaskStatus::Stopped>();
	_init_state<TaskStatus::Terminated>();
	_init_state<TaskStatus::Idle>();
	_init_state<TaskStatus::None>();
	_current = &Utilities::Mapping<State, TaskStatus::None>::GetFrom(_modes);
}

void TaskStatus::SetState(State & state)
{
	SetState(state.GetIndex());
}
void TaskStatus::SetState(const std::type_index & state)
{
	{
		boost::lock_guard<boost::mutex> lock(_mutex);
		auto it = _modes.find(state);
		if (it != _modes.end()) {
			_current = it->second.get();
		}
	}
}

State & TaskStatus::GetState()
{
	{
		boost::lock_guard<boost::mutex> lock(_mutex);
		return *_current;
	}
}
