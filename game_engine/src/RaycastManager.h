#ifndef RAYCASTMANAGER_H
#define RAYCASTMANAGER_H

//STL
#include <vector>

//Third Party
#include "box2d/box2d.h"
#include "lua-5.4.6/lua.hpp"
#include "LuaBridge/LuaBridge.h"

//Internal
#include "PhysicsManager.h"
#include "Actor.h"
#include "EngineUtils.h"

class RaycastCallback : public b2RayCastCallback {
public:

	b2Fixture* m_fixture_hit;
	b2Vec2 m_point;
	b2Vec2 m_normal;
	float m_fraction;

	RaycastCallback() : m_fixture_hit(nullptr), m_fraction(1.0f) {}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		if ((fixture->GetFilterData().categoryBits == 0x0000) || (fixture->GetFilterData().maskBits == 0x0000)) {
			return 1.0f;
		}

		if (!m_fixture_hit) {
			m_fixture_hit = fixture;
			m_point = point;
			m_normal = normal;
			m_fraction = fraction;
		}

		return m_fraction;
	}
};

class RaycastAllCallback : public b2RayCastCallback {
public:
	struct Hit {
		b2Fixture* fixture;
		b2Vec2 point;
		b2Vec2 normal;
		float fraction;
	};

	std::vector<Hit> raycast_hits;

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		if ((fixture->GetFilterData().categoryBits == 0x0000) || (fixture->GetFilterData().maskBits == 0x0000)) {
			return 1.0f;
		}

		Hit hit = { fixture, point, normal, fraction };
		if (!fixture->GetUserData().pointer) return -1;
		raycast_hits.push_back(hit);
		return 1.0f;
	}
};

class RaycastManager {
public:
	struct HitResult {
		Actor* actor = nullptr;
		b2Vec2 point;
		b2Vec2 normal;
		bool is_trigger = false;
	};

	static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist) {
		if (dist <= 0.0f) return luabridge::LuaRef(EngineUtils::g_lua_state);

		RaycastCallback callback;
		b2Vec2 end = pos + (dist * dir);

		PhysicsManager::world->RayCast(&callback, pos, end);

		if(!callback.m_fixture_hit) return luabridge::LuaRef(EngineUtils::g_lua_state);

		HitResult hit_result;
		hit_result.actor = reinterpret_cast<Actor*>(callback.m_fixture_hit->GetUserData().pointer);
		hit_result.point = callback.m_point;
		hit_result.normal = callback.m_normal;
		hit_result.is_trigger = callback.m_fixture_hit->IsSensor();

		return luabridge::LuaRef(EngineUtils::g_lua_state, hit_result);
	}

	static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
		luabridge::LuaRef raycast_table = luabridge::newTable(EngineUtils::g_lua_state);

		if (dist <= 0.0f) return raycast_table;

		RaycastAllCallback callback;
		b2Vec2 end = pos + (dist * dir);

		PhysicsManager::world->RayCast(&callback, pos, end);

		if (callback.raycast_hits.empty()) return raycast_table;

		std::sort(callback.raycast_hits.begin(), callback.raycast_hits.end(),
			[](const RaycastAllCallback::Hit& a, const RaycastAllCallback::Hit& b) {
				return a.fraction < b.fraction;
			});

		for (int i = 0; i < callback.raycast_hits.size(); i++) {
			RaycastAllCallback::Hit hit = callback.raycast_hits[i];
			HitResult result = { reinterpret_cast<Actor*>(hit.fixture->GetUserData().pointer), hit.point, hit.normal, hit.fixture->IsSensor() };
			raycast_table[i + 1] = luabridge::LuaRef(EngineUtils::g_lua_state, result);
		}
		
		return raycast_table;
	}
};

#endif // !RAYCASTMANAGER_H

