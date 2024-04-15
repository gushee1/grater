#include "ActorDB.h"

luabridge::LuaRef& Actor::AddComponent(std::string component_type) {
    std::string component_key = "r" + std::to_string(ActorDB::num_runtime_added_components);
    ActorDB::num_runtime_added_components++;

    std::shared_ptr<Component> new_component = ComponentDB::CreateComponent(this, component_key, component_type, rapidjson::Value());

    new_component->owning_actor = GetName();

    //each actor's "components" variable will store the authoritative copy for each component instance
    components.emplace_back(new_component);

    //add pointers to the component to actor's internal containers
    components_by_key[component_key] = new_component;
    components_by_type[component_type].emplace(std::make_pair(component_key, new_component));

    ActorDB::components_added_this_frame.emplace_back(new_component);

    (*new_component->ref)["actor"] = this;

    return *new_component->ref;
}

void ActorDB::LoadTemplate(std::string template_to_load)
{
    std::string template_path = "resources/actor_templates/" + template_to_load + ".template";

	if (!std::filesystem::exists(template_path)) {
		std::cout << "error: template " << template_to_load << " is missing";
		exit(0);
	}
	//if i already have the template, no need to load it in again from json
    if (TemplateDB.find(template_to_load) != TemplateDB.end()) {
        return;
    }

    rapidjson::Document template_doc;
    EngineUtils::ReadJsonFile(template_path, template_doc);

    TemplateDB.emplace(std::make_pair(template_to_load, std::move(template_doc)));
}

void ActorDB::UpdateActorWithTemplateDefaults(std::shared_ptr<Actor> actor, std::string base_template)
{
    //if template not loaded
    if (TemplateDB.find(base_template) == ActorDB::TemplateDB.end()) {
        LoadTemplate(base_template);
    }

    //ALTER ACTOR WITH TEMPLATE VALUES
    if (TemplateDB[base_template].HasMember("name")) actor->name = TemplateDB[base_template]["name"].GetString();
    
    if (TemplateDB[base_template].HasMember("components")) {
        const rapidjson::Value& components = TemplateDB[base_template]["components"];
        AddComponentsToActor(actor, components);
    }
}

void ActorDB::AddComponentsToActor(std::shared_ptr<Actor> actor, const rapidjson::Value& components)
{
    //TODO: could potentially save time by not checking for redundant components when loading from templates
    for (rapidjson::Value::ConstMemberIterator it = components.MemberBegin();
        it != components.MemberEnd(); it++) {

        std::string component_key = it->name.GetString();

        const rapidjson::Value& component_properties = it->value;

        //if this is a new component
        if (actor->components_by_key.find(component_key) == actor->components_by_key.end()) {
            std::string component_type = component_properties["type"].GetString();

            std::shared_ptr<Component> new_component = ComponentDB::CreateComponent(actor.get(), component_key, component_type, component_properties);

            new_component->owning_actor = actor->GetName();

            //each actor's "components" variable will store the authoritative copy for each component instance
            actor->components.emplace_back(new_component);

            //add pointers to the component to actor's internal containers
            actor->components_by_key[component_key] = actor->components.back();
            actor->components_by_type[component_type].emplace(std::make_pair(component_key, actor->components.back()));
        }
        else { //if overriding an inherited component
            luabridge::LuaRef inherited_component = *actor->components_by_key[component_key]->ref;
            ComponentDB::OverrideComponentProperties(inherited_component, component_properties);
        }
    }
}
