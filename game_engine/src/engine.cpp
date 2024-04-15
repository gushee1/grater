#include "engine.h"

void Engine::Start()
{
	//check for resources folder
	if (!(std::filesystem::exists("resources") && std::filesystem::is_directory("resources"))) {
		std::cout << "error: resources/ missing";
		exit(0);
	}

	//check for game.config file
	if (!std::filesystem::exists("resources/game.config")) {
		std::cout << "error: resources/game.config missing";
		exit(0);
	}

	rapidjson::Document game_config;
	EngineUtils::ReadJsonFile("resources/game.config", game_config);

	EngineUtils::LuaInit();

	//set up scripting API
	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", EngineUtils::CppLog)
		.addFunction("LogError", EngineUtils::CppLogError)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::GetName)
		.addFunction("GetID", &Actor::GetID)
		.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
		.addFunction("GetComponent", &Actor::GetComponent)
		.addFunction("GetComponents", &Actor::GetComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Actor")
		.addFunction("Find", SceneDB::FindActor)
		.addFunction("FindAll", SceneDB::FindActors)
		.addFunction("Instantiate", SceneDB::InstantiateActor)
		.addFunction("Destroy", SceneDB::DestroyActor)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Application")
		.addFunction("Quit", EngineUtils::QuitApplication)
		.addFunction("Sleep", EngineUtils::Sleep)
		.addFunction("GetFrame", Helper::GetFrameNumber)
		.addFunction("OpenURL", EngineUtils::OpenURL)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Input")
		.addFunction("GetKey", Input::GetKey)
		.addFunction("GetKeyDown", Input::GetKeyDown)
		.addFunction("GetKeyUp", Input::GetKeyUp)
		.addFunction("GetMouseButton", Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", Input::GetMouseButtonUp)
		.addFunction("GetMousePosition", Input::GetMousePosition)
		.addFunction("GetMouseScrollDelta", Input::GetMouseScrollDelta)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Text")
		.addFunction("Draw", Renderer::QueueTextToDraw)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Audio")
		.addFunction("Play", AudioDB::PlayAudio)
		.addFunction("Halt", AudioDB::HaltChannel)
		.addFunction("SetVolume", AudioDB::SetVolume)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Image")
		.addFunction("DrawUI", Renderer::QueueUIToDraw)
		.addFunction("DrawUIEx", Renderer::QueueUIToDrawEx)
		.addFunction("Draw", Renderer::QueueImageToDraw)
		.addFunction("DrawEx", Renderer::QueueImageToDrawEx)
		.addFunction("DrawPixel", Renderer::QueuePixelToDraw)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Camera")
		.addFunction("SetPosition", Renderer::SetCameraPosition)
		.addFunction("GetPositionX", Renderer::GetCameraXPos)
		.addFunction("GetPositionY", Renderer::GetCameraYPos)
		.addFunction("SetZoom", Renderer::SetCameraZoom)
		.addFunction("GetZoom", Renderer::GetCameraZoom)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Scene")
		.addFunction("Load", SceneDB::LoadSceneOnNextFrame)
		.addFunction("GetCurrent", SceneDB::GetSceneName)
		.addFunction("DontDestroy", SceneDB::DontDestroyActor)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addProperty("x", &b2Vec2::x)
		.addProperty("y", &b2Vec2::y)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.endClass();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Vector2")
		.addFunction("Distance", b2Distance)
		.addFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginClass<Rigidbody>("Rigidbody")
		.addData("actor", &Rigidbody::actor)
		.addData("enabled", &Rigidbody::enabled)
		.addData("type", &Rigidbody::type)
		.addData("key", &Rigidbody::key)
		.addData("x", &Rigidbody::x)
		.addData("y", &Rigidbody::y)
		.addData("body_type", &Rigidbody::body_type)
		.addData("precise", &Rigidbody::precise)
		.addData("gravity_scale", &Rigidbody::gravity_scale)
		.addData("density", &Rigidbody::density)
		.addData("angular_friction", &Rigidbody::angular_friction)
		.addData("rotation", &Rigidbody::rotation)
		.addData("has_collider", &Rigidbody::has_collider)
		.addData("body_type", &Rigidbody::body_type)
		.addData("collider_type", &Rigidbody::collider_type)
		.addData("width", &Rigidbody::width)
		.addData("height", &Rigidbody::height)
		.addData("radius", &Rigidbody::radius)
		.addData("friction", &Rigidbody::friction)
		.addData("bounciness", &Rigidbody::bounciness)
		.addData("has_trigger", &Rigidbody::has_trigger)
		.addData("trigger_type", &Rigidbody::trigger_type)
		.addData("trigger_width", &Rigidbody::trigger_width)
		.addData("trigger_height", &Rigidbody::trigger_height)
		.addData("trigger_radius", &Rigidbody::trigger_radius)
		.addFunction("OnStart", &Rigidbody::OnStart)
		.addFunction("OnDestroy", &Rigidbody::OnDestroy)
		.addFunction("GetPosition", &Rigidbody::GetPosition)
		.addFunction("GetRotation", &Rigidbody::GetRotation)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.endClass();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("")
		.addFunction("OnCollisionEnter", ContactListener::OnCollisionEnter)
		.addFunction("OnCollisionExit", ContactListener::OnCollisionExit)
		.addFunction("OnTriggerEnter", ContactListener::OnTriggerEnter)
		.addFunction("OnTriggerExit", ContactListener::OnTriggerExit)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginClass<Collision>("collision")
		.addData("other", &Collision::other)
		.addData("point", &Collision::point)
		.addData("relative_velocity", &Collision::relative_velocity)
		.addData("normal", &Collision::normal)
		.endClass();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginClass<RaycastManager::HitResult>("HitResult")
		.addData("actor", &RaycastManager::HitResult::actor)
		.addData("point", &RaycastManager::HitResult::point)
		.addData("normal", &RaycastManager::HitResult::normal)
		.addData("is_trigger", &RaycastManager::HitResult::is_trigger)
		.endClass();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Physics")
		.addFunction("Raycast", RaycastManager::Raycast)
		.addFunction("RaycastAll", RaycastManager::RaycastAll)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Event")
		.addFunction("Publish", EventBus::Publish)
		.addFunction("Subscribe", EventBus::Subscribe)
		.addFunction("Unsubscribe", EventBus::Unsubscribe)
		.endNamespace();

	luabridge::getGlobalNamespace(EngineUtils::g_lua_state)
		.beginNamespace("Network")
		.addFunction("OpenServer", grater::network::OpenServer)
		.addFunction("JoinServer", grater::network::JoinServer)
		.endNamespace();
	//end set up scripting API

	is_running = true;

	//load from game_config
	const char* game_title = game_config.HasMember("game_title") ? game_config["game_title"].GetString() : "";

	//OPEN WINDOW AND CREATE RENDERER
	Renderer::Start(game_title);

	PhysicsManager::Initialize();

	AudioDB::Initialize();
	TextDB::Initialize();

	//LOAD STARTING SCENE
	if (!game_config.HasMember("initial_scene")) {
		std::cout << "error: initial_scene unspecified";
		exit(0);
	}
	std::string initial_scene = game_config["initial_scene"].GetString();
	SceneDB::LoadScene(initial_scene);
	//END LOAD STARTING SCENE
 }

void Engine::Update()
{
	if (SceneDB::should_change_scene) {
		SceneDB::UnloadScene();
		SceneDB::LoadScene(SceneDB::next_scene);
	}

	ProcessInput();

	switch (grater::network::GetMultiplayerState()) {
	case grater::network::MultiplayerState::SERVER:
		grater::network::server->Update();
		break;
	case grater::network::MultiplayerState::CLIENT:
		grater::network::client->Update();
		break;
	default:
		break;
	}

	SceneDB::RunStartEvents();  
	SceneDB::RunUpdateEvents();
	SceneDB::RunLateUpdateEvents();
	SceneDB::RunDestroyEvents();

	PhysicsManager::Step();

	//RENDER LOGIC STARTS HERE
	Renderer::ClearFrame();
	Render();

	Renderer::ShowFrame();
	Input::LateUpdate();
	SceneDB::LateUpdate();
}

void Engine::Render()
{
	SDL_RenderSetScale(Renderer::renderer, Renderer::GetCameraZoom(), Renderer::GetCameraZoom());
	Renderer::DrawImages();
	SDL_RenderSetScale(Renderer::renderer, 1.0, 1.0);
	Renderer::DrawUI();
	Renderer::GraterDrawText();
	Renderer::DrawPixels();
}

void Engine::ProcessInput()
{
	while (Helper::SDL_PollEvent498(&next_event)) {
		if (next_event.type == SDL_QUIT) {
			is_running = false;
			return;
		}
		Input::ProcessEvent(next_event);
	}
}

void Engine::Run()
{
	Start();
	while (is_running)
	{
		Update();
	}
	Destroy();
}

void Engine::Destroy()
{

}
