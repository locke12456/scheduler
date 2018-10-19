#ifndef __COMMANDFACTORY_H__
#define __COMMANDFACTORY_H__ 
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/lambda/bind.hpp> // !
#include <boost/lambda/construct.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include <locale>
namespace Utilities {
	class String {
	public:
		typedef std::string u8string;

		static u8string ToUTF8(const std::u16string &s)
		{
			return boost::locale::conv::utf_to_utf<char>(s);
		}

		static u8string ToUTF8(const std::u32string &s)
		{
			return boost::locale::conv::utf_to_utf<char>(s);
		}

		static std::u16string ToUTF16(const u8string &s)
		{
			return boost::locale::conv::utf_to_utf<char16_t>(s);
		}

		static std::u16string ToUTF16(const std::u32string &s)
		{
			return boost::locale::conv::utf_to_utf<char16_t>(s);
		}

		static std::u32string ToUTF32(const u8string &s)
		{
			return boost::locale::conv::utf_to_utf<char32_t>(s);
		}

		static std::u32string ToUTF32(const std::u16string &s)
		{
			return boost::locale::conv::utf_to_utf<char32_t>(s);
		}
	};
	template<class Interface, class Type >
	class CommandFactory
	{
	public:
		typedef boost::shared_ptr< Interface > ComandPtr;
		typedef boost::function < ComandPtr() > Command;
		CommandFactory() {};
		~CommandFactory() {};
		Command Create() {
			namespace la = boost::lambda;
			return la::bind(
				la::constructor<ComandPtr>(),
				la::bind(la::new_ptr<Type>()));
		};
	};
	template<class Interface, class Type, class State>
	class MapStatus {
	public:
		typedef std::unordered_map<std::type_index, std::unique_ptr<Interface>> StateMap;
		static void toMap(StateMap & map) {
			map.insert(
				std::make_pair(
					std::type_index(typeid(State)),
					std::unique_ptr<Type>(new Type())
				)
			);
		}
	};

	template<typename Interface, class Type>
	class Mapping {
	public:
		typedef boost::shared_ptr< Interface > ComandPtr;
		typedef boost::function < ComandPtr() > Command;
		typedef std::unordered_map<std::string, std::type_index> IdentifierMap;
		typedef std::unordered_map<std::type_index, Command > CommandMap;
		typedef std::unordered_map<std::type_index, std::unique_ptr<Interface>> SharedPtrTypeMap;

		Mapping() {};
		virtual ~Mapping() {};
		template< typename type >
		static void toIdentifierMap(IdentifierMap & map, const type name) {
			map.insert(
				std::make_pair(
					name,
					std::type_index(typeid(Type))
				)
			);
		}
		static void toIdentifierMap(IdentifierMap & map, std::string name) {
			map.insert(
				std::make_pair(
					name,
					std::type_index(typeid(Type))
				)
			);
		}
		template< typename type >
		static void toTypeMap(std::unordered_map<std::type_index, type> & map, const type name) {
			map.insert(
				std::make_pair(
					std::type_index(typeid(Type)),
					name
				)
			);
		}

		static void toTypeMap(SharedPtrTypeMap & map) {
			map.insert(
				std::make_pair(
					std::type_index(typeid(Type)),
					std::unique_ptr<Type>(new Type())
				)
			);
		}
		template<class constructor_1 >
		static void toTypeMap(SharedPtrTypeMap & map, constructor_1 value) {
			map.insert(
				std::make_pair(
					std::type_index(typeid(Type)),
					std::unique_ptr<Type>(new Type(value))
				)
			);
		}
		static void toCommandMap(CommandMap & map) {
			CommandFactory<Interface, Type> factory;
			map.insert(
				std::make_pair(
					std::type_index(typeid(Type)),
					factory.Create()
				)
			);
		}
		static bool Exist(SharedPtrTypeMap & map) {
			return
				map.find(
					std::type_index(typeid(Type))
				) != map.end();
		}
		static Type& GetFrom(SharedPtrTypeMap & map) {
			return
				*(Type *)map.find(
					std::type_index(typeid(Type))
				)->second.get();
		}
	};
};
#endif