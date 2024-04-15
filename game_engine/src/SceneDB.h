#ifndef SCENEDB_H
#define SCENEDB_H

//STL
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <unordered_set>

//Third Party
#include "SDL_image.h"

//Internal
#include "EngineUtils.h"
#include "ActorDB.h"
#include "Renderer.h"
#include "ComponentDB.h"

class SceneDB
{
public:

	static inline bool should_change_scene = false;
	static inline std::string current_scene = "";
	static inline std::string next_scene = "";

	static inline std::vector<std::shared_ptr<Actor>> actors_in_scene; 

	//allows searching within scenes by actor names
	static inline std::unordered_map<std::string, std::vector<std::shared_ptr<Actor>>> actors_by_name;

	static inline std::vector<std::shared_ptr<Actor>> global_scene_actors;

	static inline std::vector<std::shared_ptr<Component>> start_components;
	static inline std::vector<std::shared_ptr<Component>> update_components;
	static inline std::vector<std::shared_ptr<Component>> lateupdate_components;
	static inline std::vector<std::shared_ptr<Component>> destroy_components;

	static void LoadScene(std::string scene_name);

	static std::string GetSceneName();

	static void LoadSceneOnNextFrame(std::string scene_name);

	static void DontDestroyActor(luabridge::LuaRef actor);

	static void RunStartEvents();

	static void RunUpdateEvents();

	static void RunLateUpdateEvents();

	static void RunDestroyEvents();

	static void UnloadScene();

	static uint64_t MakeKey(int x, int y);

	static void LateUpdate();

	static luabridge::LuaRef FindActor(std::string name);

	static luabridge::LuaRef FindActors(std::string name);

	static luabridge::LuaRef InstantiateActor(std::string actor_template);

	static void DestroyActor(luabridge::LuaRef actor);

private:

	static inline int current_id = 0;

	static inline int collider_region_width = 0;
	static inline int collider_region_height = 0;

	static inline int trigger_region_width = 0;
	static inline int trigger_region_height = 0;

};

#endif
