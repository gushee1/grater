#ifndef ENGINE_H
#define ENGINE_H

//STL
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <filesystem>

//Third Party
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"

//Internal
#include "SceneDB.h"
#include "Helper.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "Input.h"
#include "PhysicsManager.h"
#include "RaycastManager.h"
#include "EventBus.h"
#include "networking/network.h"

class Engine
{
private:
	
	bool is_running = true;
	bool change_scene = false;

	SDL_Event next_event;

public:

	void Start();

	void Update();

	void Render();

	void ProcessInput();

	void Run();

	void Destroy();
};

#endif
