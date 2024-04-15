#include "ComponentDB.h"

void ComponentDB::LoadUserComponent(const std::string& component_type)
{
	std::string filepath = "resources/component_types/" + component_type + ".lua";

	if (!std::filesystem::exists(filepath)) {
		std::cout << "error: failed to locate component " << component_type;
		exit(0);
	}

	//if the component is not loaded in the DB
	if (user_components.find(component_type) == user_components.end()) {
		//put the lua component into the global lua state
		if (luaL_dofile(EngineUtils::g_lua_state, filepath.c_str()) != LUA_OK) {
			//TODO: return line number of issue to help with dev debugging
			std::cout << "problem with lua file " << component_type;
			exit(0);
		}

		//TODO: improve the runtime efficiency of accessing components -- 
		// is it fastest to keep in a map, with a map of pointers, or accessing the global state?

		//add a reference to the component to the internal componentDB for ease of access
		user_components[component_type] = std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(EngineUtils::g_lua_state, component_type.c_str()));
	}
}

bool ComponentDB::IsCoreComponent(std::string component_type)
{
	return core_components.find(component_type) != core_components.end();
}

std::shared_ptr<Component> ComponentDB::CreateComponent(Actor* actor, const std::string& component_key, const std::string& component_type, const rapidjson::Value& properties)
{
	if (core_components.find(component_type) != core_components.end()) {
		return CreateCoreComponent(actor, component_key, component_type, properties);
	}
	else {
		LoadUserComponent(component_type);
		return CreateUserComponent(component_key, component_type, properties);
	}
}

std::shared_ptr<Component> ComponentDB::CreateUserComponent(const std::string& component_key, const std::string& component_type, const rapidjson::Value& properties)
{
	std::shared_ptr<Component> new_component = std::make_shared<Component>();

	//create a new instance for the component table, and fill it with data from parent table
	luabridge::LuaRef new_component_table = luabridge::newTable(EngineUtils::g_lua_state);
	InstanceLuaComponent(new_component_table, *user_components[component_type]);
	OverrideComponentProperties(new_component_table, properties);

	//inject key into the lua table
	new_component_table["key"] = component_key;

	new_component->ref = std::make_shared<luabridge::LuaRef>(new_component_table);

	new_component->key = component_key;
	new_component->type = component_type;

	new_component_table["enabled"] = true;

	new_component->has_start = !new_component_table["OnStart"].isNil() && new_component_table["OnStart"].isFunction();
	new_component->has_update = !new_component_table["OnUpdate"].isNil() && new_component_table["OnUpdate"].isFunction();
	new_component->has_lateupdate = !new_component_table["OnLateUpdate"].isNil() && new_component_table["OnLateUpdate"].isFunction();
	new_component->has_destroy = !new_component_table["OnDestroy"].isNil() && new_component_table["OnDestroy"].isFunction();

	//return the component
	return new_component;
}

std::shared_ptr<Component> ComponentDB::CreateCoreComponent(Actor* actor, const std::string& component_key, const std::string& component_type, const rapidjson::Value& properties)
{
	if (component_type == "Rigidbody") {
		return CreateRigidbody(actor, component_key, properties);
	}
	else {
		std::cout << component_type << " is not a supported component";
		exit(0);
	}
}

//fill a blank new table with data from source truth parent table to create an instance for an actor
void ComponentDB::InstanceLuaComponent(luabridge::LuaRef& instance_table, const luabridge::LuaRef& parent_table)
{
	luabridge::LuaRef new_metatable = luabridge::newTable(EngineUtils::g_lua_state);
	new_metatable["__index"] = parent_table;

	instance_table.push(EngineUtils::g_lua_state);
	new_metatable.push(EngineUtils::g_lua_state);
	lua_setmetatable(EngineUtils::g_lua_state, -2);
	lua_pop(EngineUtils::g_lua_state, 1);
}

std::shared_ptr<Component> ComponentDB::CreateRigidbody(Actor* actor, const std::string& component_key, const rapidjson::Value& properties)
{
	std::shared_ptr<Component> component = std::make_shared<Component>();

	Rigidbody* rb = new Rigidbody();
	rb->actor = actor;
	rb->key = component_key;

	component->has_start = true;
	component->has_destroy = true;

	luabridge::LuaRef component_ref(EngineUtils::g_lua_state, rb);
	component->ref = std::make_shared<luabridge::LuaRef>(component_ref);

	OverrideComponentProperties(component_ref, properties);

	return component;
}

void ComponentDB::OverrideComponentProperties(luabridge::LuaRef& component_table, const rapidjson::Value& properties) {
	if (!properties.IsObject()) return;

	//inject component values to instance lua table
	for (rapidjson::Value::ConstMemberIterator it = properties.MemberBegin(); it != properties.MemberEnd(); it++)
	{
		//no need to inject component type into lua table
		if (it->name == "type") {
			continue;
		}
		if (it->value.IsString()) {
			component_table[it->name.GetString()] = it->value.GetString();
		}
		else if (it->value.IsFloat()) {
			component_table[it->name.GetString()] = it->value.GetFloat();
		}
		else if (it->value.IsInt()) {
			component_table[it->name.GetString()] = it->value.GetInt();
		}
		else if (it->value.IsBool()) {
			component_table[it->name.GetString()] = it->value.GetBool();
		}
		else {
			std::cout << "error: usage of unsupported property type within component";
			exit(0);
		}
	}
}
