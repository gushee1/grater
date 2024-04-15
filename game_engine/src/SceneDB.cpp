#include "SceneDB.h"

void Actor::RemoveComponent(luabridge::LuaRef component) {
    (*components_by_key[component["key"]]->ref)["enabled"] = false;
    if (components_by_key[component["key"]]->has_destroy) {
        SceneDB::destroy_components.push_back(components_by_key[component["key"]]);
    }
}

void SceneDB::LoadScene(std::string scene_name)
{
    should_change_scene = false;
    current_scene = scene_name;

    //check for existence of scene file
    std::string scene_file = "resources/scenes/" + scene_name + ".scene";
    if (!std::filesystem::exists(scene_file)) {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }

    current_id = 0;
    rapidjson::Document active_scene;
	EngineUtils::ReadJsonFile(scene_file, active_scene);

    //this is probably guaranteed, so may not need this line
    if (active_scene.HasMember("actors") && active_scene["actors"].IsArray()) {
        const rapidjson::Value& actorsArray = active_scene["actors"];

        //if losing reference pointer to actors investigate the actors_in_scene container
        //important to ensure actors do not move under the hood with a vector resize op
        actors_in_scene.reserve(actorsArray.Size());

        //add player objects to vector to represent scene
        for (rapidjson::SizeType i = 0; i < actorsArray.Size(); i++)
        {
            const rapidjson::Value& actorObject = actorsArray[i];
            std::shared_ptr<Actor> new_actor_ref = std::make_shared<Actor>();

            new_actor_ref->id = current_id;

            //if actor inherits from template, update it with template defaults
            if (actorObject.HasMember("template")) {
                ActorDB::UpdateActorWithTemplateDefaults(new_actor_ref, actorObject["template"].GetString());
            }

            //override name if given
            if (actorObject.HasMember("name")) new_actor_ref->name = actorObject["name"].GetString();

            //override components
            if (actorObject.HasMember("components")) {
                const rapidjson::Value& components_object = actorObject["components"];

                ActorDB::AddComponentsToActor(new_actor_ref, components_object);
            }

            for (const auto& pair : new_actor_ref->components_by_key) {
                //add lifecycle functions to their respective containers
                if (pair.second->has_start) {
                    start_components.push_back(pair.second);
                }

                if (pair.second->has_update) {
                    update_components.push_back(pair.second);
                }

                if (pair.second->has_lateupdate) {
                    lateupdate_components.push_back(pair.second);
                }
            }

            for (const auto& component : new_actor_ref->components) {
                (*component->ref)["actor"] = new_actor_ref.get();
            }

            actors_in_scene.push_back(new_actor_ref);

            current_id++;
        }

        for (const auto& actor : actors_in_scene) {
            actors_by_name[actor->name].push_back(actor);
        }
    }
}

std::string SceneDB::GetSceneName()
{
    return current_scene;
}

void SceneDB::LoadSceneOnNextFrame(std::string scene_name)
{
    should_change_scene = true;
    next_scene = scene_name;
}

void SceneDB::DontDestroyActor(luabridge::LuaRef actor_ref)
{
    Actor& actor = *actor_ref.cast<Actor*>();
    actor.is_global = true;
    global_scene_actors.push_back(std::make_shared<Actor>(actor));
}

void SceneDB::RunStartEvents()
{
    if (start_components.empty()) return;

    //start events is a vector of pairs where second is the start function, and first is the associated component
    for (auto component : start_components) {
        if (!(*component->ref)["enabled"]) continue;

        luabridge::LuaRef& lua_table = *component->ref;
        luabridge::LuaRef OnStart = lua_table["OnStart"];
        try
        {
            OnStart(*component->ref);
        }
        catch (luabridge::LuaException e)
        {
            EngineUtils::ReportLuaError(component->owning_actor, e);
        }
    }

    //only run a component's start event once
    start_components.clear();
}

void SceneDB::RunUpdateEvents() 
{
    if (update_components.empty()) return;

    for (auto component : update_components) {
        if (!(*component->ref)["enabled"]) continue;

        luabridge::LuaRef& lua_table = *component->ref;
        luabridge::LuaRef OnUpdate = lua_table["OnUpdate"];
        try
        {
            OnUpdate(*component->ref);
        }
        catch (luabridge::LuaException e)
        {
            EngineUtils::ReportLuaError(component->owning_actor, e);
        }
    }
}

void SceneDB::RunLateUpdateEvents()
{
    if (lateupdate_components.empty()) return;

    for (auto component : lateupdate_components) {
        if (!(*component->ref)["enabled"]) continue;

        luabridge::LuaRef& lua_table = *component->ref;
        luabridge::LuaRef OnLateUpdate = lua_table["OnLateUpdate"];
        try
        {
            OnLateUpdate(*component->ref);
        }
        catch (luabridge::LuaException e)
        {
            EngineUtils::ReportLuaError(component->owning_actor, e);
        }
    }
}

void SceneDB::RunDestroyEvents()
{
    if (destroy_components.empty()) return;

    for (auto& component : destroy_components) {
        luabridge::LuaRef& lua_table = *component->ref;
        luabridge::LuaRef OnDestroy = lua_table["OnDestroy"];
        try 
        {
            OnDestroy(*component->ref);
        }
        catch (luabridge::LuaException e) {
            EngineUtils::ReportLuaError(component->owning_actor, e);
        }
    }

    destroy_components.clear();
}

void SceneDB::UnloadScene()
{
    //see if i can move this to a destructor and construct new SceneDB object on new scene load
    actors_by_name.clear();
    start_components.clear();
    update_components.clear();
    lateupdate_components.clear();

    for (const auto& actor : actors_in_scene) {
        if (!actor->is_global) {
            for (const auto& pair : actor->components_by_key) {
                if (pair.second->has_destroy) {
                    destroy_components.push_back(pair.second);
                }
            }
        }
    }
    RunDestroyEvents();

    //must clear this last, as this is the ground truth location for all other pointers
    actors_in_scene.clear();
    for (const auto& actor : global_scene_actors) {
        actors_in_scene.push_back(actor);
        for (const auto& component : actor->components) {
            if (component->has_update) {
                update_components.push_back(component);
            }

            if (component->has_lateupdate) {
                lateupdate_components.push_back(component);
            }
        }
    }
}

uint64_t SceneDB::MakeKey(int x, int y)
{
    uint32_t ux = static_cast<uint32_t>(x);
    uint32_t uy = static_cast<uint32_t>(y);
    uint64_t hash = static_cast<uint64_t>(ux);
    hash = hash << 32;
    hash = hash | static_cast<uint64_t>(uy);
    return hash;
}

void SceneDB::LateUpdate()
{
    for (const auto& new_component : ActorDB::components_added_this_frame) {
        if (!(*new_component->ref)["enabled"]) continue;

        if (new_component->has_start) {
            start_components.push_back(new_component);
        }

        if (new_component->has_update) {
            update_components.push_back(new_component);
        }

        if (new_component->has_lateupdate) {
            lateupdate_components.push_back(new_component);
        }
    }

    ActorDB::components_added_this_frame.clear();
}

luabridge::LuaRef SceneDB::FindActor(std::string name)
{
    if (actors_by_name.find(name) == actors_by_name.end()) return luabridge::LuaRef(EngineUtils::g_lua_state);
    if (!(*actors_by_name[name].begin())->enabled) return luabridge::LuaRef(EngineUtils::g_lua_state);
    return luabridge::LuaRef(EngineUtils::g_lua_state, actors_by_name[name].begin()->get());
}

luabridge::LuaRef SceneDB::FindActors(std::string name) 
{
    luabridge::LuaRef actors_table = luabridge::newTable(EngineUtils::g_lua_state);

    if (actors_by_name.find(name) == actors_by_name.end()) return actors_table;

    int table_index = 1;

    for (const auto& actor : actors_by_name[name]) {
        if (!actor->enabled) continue;
        actors_table[table_index] = actor.get();
        table_index++;
    }

    return actors_table;
}

luabridge::LuaRef SceneDB::InstantiateActor(std::string actor_template)
{
    std::shared_ptr<Actor> new_actor_ref = std::make_shared<Actor>();

    new_actor_ref->id = current_id;

    ActorDB::UpdateActorWithTemplateDefaults(new_actor_ref, actor_template);

    for (const auto& pair : new_actor_ref->components_by_key) {
        //add lifecycle functions to their respective containers
        ActorDB::components_added_this_frame.push_back(pair.second);
    }

    for (const auto& component : new_actor_ref->components) {
        (*component->ref)["actor"] = *new_actor_ref;
    }

    actors_in_scene.push_back(new_actor_ref);

    actors_by_name[new_actor_ref->name].push_back(new_actor_ref);

    current_id++;

    return luabridge::LuaRef(EngineUtils::g_lua_state, new_actor_ref.get());
}

void SceneDB::DestroyActor(luabridge::LuaRef actor_ref)
{
    Actor& actor = *actor_ref.cast<Actor*>();
    actors_in_scene[actor.id]->enabled = false;

    for (const auto& pair : actor.components_by_key) {
        (*pair.second->ref)["enabled"] = false;
        if (pair.second->has_destroy) {
            destroy_components.push_back(pair.second);
        }
    }
}
