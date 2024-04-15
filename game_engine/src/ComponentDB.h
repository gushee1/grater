#ifndef COMPONENTDB_H
#define COMPONENTDB_H

//STL
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <memory>
#include <unordered_set>

//Third Party
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "rapidjson/document.h"

//Internal
#include "EngineUtils.h"

//Core Components
#include "Rigidbody.h"
#include "Component.h"

class ComponentDB
{
public:

	static bool IsCoreComponent(std::string component_type);

	static std::shared_ptr<Component> CreateComponent(Actor* actor, const std::string& component_key, const std::string& component_type, const rapidjson::Value& properties);

	static void OverrideComponentProperties(luabridge::LuaRef& component_table, const rapidjson::Value& properties);

private:

	static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> user_components;

	static inline std::unordered_set<std::string> core_components = { "Rigidbody" };

	static void LoadUserComponent(const std::string& component_type);

	static std::shared_ptr<Component> CreateUserComponent(const std::string& component_key, const std::string& component_type, const rapidjson::Value& properties);

	static std::shared_ptr<Component> CreateCoreComponent(Actor* actor, const std::string& component_key, const std::string& component_type, const rapidjson::Value& properties);

	static void InstanceLuaComponent(luabridge::LuaRef& table_in, const luabridge::LuaRef& parent_table);

	//Core Components
	static std::shared_ptr<Component> CreateRigidbody(Actor* actor, const std::string& component_key, const rapidjson::Value& properties);

};

#endif
