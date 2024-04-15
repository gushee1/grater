#ifndef EVENTBUS_H
#define EVENTBUS_H

//STL
#include <string>
#include <unordered_map>
#include <vector>

//Third Party
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"

//Internal
#include "EngineUtils.h"

class EventBus {
public:
	static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> event_types_to_functions;

	static void Publish(std::string event_type, luabridge::LuaRef event_object) {
		for (auto& pair : event_types_to_functions[event_type]) {
			try {
				pair.second(pair.first, event_object);
			}
			catch (luabridge::LuaException e) {
				EngineUtils::ReportLuaError("", e);
			}
		}
	}

	static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
		event_types_to_functions[event_type].push_back({ component, function });
	}

	static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
		auto& pairs = event_types_to_functions[event_type];
		for (auto iter = pairs.begin(); iter != pairs.end(); ) {
			if (iter->first == component && iter->second == function) {
				iter = pairs.erase(iter);
			}
			else {
				++iter;
			}
		}
		if (pairs.empty()) {
			event_types_to_functions.erase(event_type);
		}
	}

};

#endif // !EVENTBUS_H
