#ifndef __TASKSTATUS_H__
#define __TASKSTATUS_H__
#include <CommandFactory.hpp>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <boost/thread.hpp>
namespace Utility {
	class State {
	public:
		virtual void Initialize() = 0;
		virtual std::type_index & GetIndex() = 0;
	};

	template<class type>
	class AbstractState : public State
	{
	public:
		AbstractState() :
			_info(0), _base(typeid(*this)),
			_index(_base)
		{

		};

		virtual ~AbstractState() {};

		virtual void Initialize()
		{
			_info = &typeid(*this);
			_index = std::type_index(*_info);
		};

#ifdef HAS_LOG4CPP

		virtual void Log(type message) override {
			LOG_INFO("[" << _state_name() << "] " << message)
		};

#endif // HAS_LOG4CPP

		virtual std::type_index & GetIndex() override {
			return _index;
		};

	protected:
		const std::type_info& _base;
		const std::type_info* _info;
		std::type_index _index;
		type _state;

		std::string _state_name() {
			std::string sample("class ");
			std::string state = std::string(_info->name()).replace(0, sample.size(), "");
			return state;
		};
	};
	
	class TaskStatus 
	{
	public:
		typedef AbstractState<int> TaskState;
		class None		: public TaskState {};
		class Idle		: public TaskState {};
		class Start		: public TaskState {};
		class Stop		: public TaskState {};
		class Destroy	: public TaskState {};
		class Ready		: public TaskState {};
		class Running	: public TaskState {};
		class Stopped	: public TaskState {};
		class Terminated : public TaskState {};
	public:
		TaskStatus();
		virtual ~TaskStatus();

		virtual void Initialize();
		virtual void SetState(State & state);
		virtual void SetState(const std::type_index & state);
		virtual State & GetState();
	private:
		typedef std::unordered_map<std::type_index, std::unique_ptr<State>> Status;
		Status _modes;
		boost::mutex _mutex;
		State *_current;
		template<class state>
		inline void _init_state()
		{
			auto it = _modes.find(typeid(state));
			if (it == _modes.end()) {
				Utilities::Mapping<State, state >::toTypeMap(_modes);
				Utilities::Mapping<State, state >::GetFrom(_modes).Initialize();
			}
		}
	};
};
#endif
