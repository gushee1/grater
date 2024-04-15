#ifndef ACTORDB_H
#define ACTORDB_H

//STL
#include <string>
#include <unordered_map>
#include <filesystem>
#include <optional>
#include <unordered_set>

//Third Party
#include "SDL_image.h"
#include "rapidjson/document.h"
#include "glm/glm.hpp"

//Internal
#include "EngineUtils.h"
#include "ComponentDB.h"
#include "Actor.h"

class Actor;

class ActorDB {
public:
	static inline int num_runtime_added_components = 0;

	static inline std::vector<std::shared_ptr<Component>> components_added_this_frame;

	static inline std::unordered_map<std::string, rapidjson::Document> TemplateDB;

	static void LoadTemplate(std::string template_to_load);

	static void UpdateActorWithTemplateDefaults(std::shared_ptr<Actor> actor, std::string base_template);

	static void AddComponentsToActor(std::shared_ptr<Actor> actor, const rapidjson::Value& components);

};

#endif
