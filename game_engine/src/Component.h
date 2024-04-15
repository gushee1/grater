#ifndef COMPONENT_H
#define COMPONENT_H

//STL
#include <string>
#include <memory>

//Third Party
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component {
public:
	std::shared_ptr<luabridge::LuaRef> ref;

	//TODO: find a way to get rid of this variable
	std::string owning_actor;

	std::string key;
	std::string type;

	bool has_start;
	bool has_update;
	bool has_lateupdate;
	bool has_destroy;

	Component() {
		ref = nullptr;
		owning_actor = "";
		key = "";
		type = "";
		has_start = false;
		has_update = false;
		has_lateupdate = false;
		has_destroy = false;
	};
};

#endif // !COMPONENT_H
