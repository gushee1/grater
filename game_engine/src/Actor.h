#ifndef ACTOR_H
#define ACTOR_H

//STL
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

//Third Party
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "EngineUtils.h"

//Internal
#include "Component.h"

class Actor {
public:
	int id;
	bool enabled;
	bool is_global;
	std::string name;
	std::string base_template;

	std::vector<std::shared_ptr<Component>> components;

	std::map<std::string, std::shared_ptr<Component>> components_by_key;
	std::unordered_map<std::string, std::map<std::string, std::shared_ptr<Component>>> components_by_type;

	Actor() {
		id = 0;
		enabled = true;
		is_global = false;
		name = "";
		base_template = "";
	}

	Actor(int actor_id, std::string actor_name, std::string base_template) : id(actor_id), enabled(true), is_global(false), name(actor_name), base_template(base_template) {}

	//Actor(const Actor&) = delete;
	//Actor& operator=(const Actor&) = delete;

	std::string GetName() { 
		return name; 
	}

	int GetID() { 
		return id; 
	}

	luabridge::LuaRef GetComponentByKey(std::string key)
	{
		if (components_by_key.find(key) == components_by_key.end()) return luabridge::LuaRef(EngineUtils::g_lua_state);
		if (!(*components_by_key[key]->ref)["enabled"]) return luabridge::LuaRef(EngineUtils::g_lua_state);
		return *components_by_key[key]->ref;
	}

	luabridge::LuaRef GetComponent(std::string type)
	{
		if (components_by_type.find(type) == components_by_type.end()) return luabridge::LuaRef(EngineUtils::g_lua_state);
		if (!(*components_by_type[type].begin()->second->ref)["enabled"]) return luabridge::LuaRef(EngineUtils::g_lua_state);
		return *components_by_type[type].begin()->second->ref;
	}

	luabridge::LuaRef GetComponents(std::string type) {
		luabridge::LuaRef components_table = luabridge::newTable(EngineUtils::g_lua_state);

		if (components_by_type.find(type) == components_by_type.end()) return components_table;

		int table_index = 1;
		for (const auto& pair : components_by_type[type]) {
			if (!(*pair.second->ref)["enabled"]) continue;
			components_table[table_index] = *pair.second->ref;
			table_index++;
		}

		return components_table;
	}

	luabridge::LuaRef& AddComponent(std::string component_type);

	void RemoveComponent(luabridge::LuaRef component);
};

#endif // !ACTOR_H
