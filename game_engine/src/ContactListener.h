#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

//STL

//Third Party
#include "box2d/box2d.h"

//Internal
#include "Actor.h"
#include "EngineUtils.h"

struct Collision {
	Actor* other = nullptr;
	b2Vec2 point;
	b2Vec2 relative_velocity;
	b2Vec2 normal;
};   

//TODO: why is relative velocity the same for each side of the collision?
//TODO: why don't colliders activate triggers?
class ContactListener : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact) override {
		b2Fixture* a = contact->GetFixtureA();
		b2Fixture* b = contact->GetFixtureB();

		bool ignoreA = a->GetFilterData().categoryBits == 0 && a->GetFilterData().maskBits == 0;
		bool ignoreB = b->GetFilterData().categoryBits == 0 && b->GetFilterData().maskBits == 0;

		if (ignoreA || ignoreB) return;

		Actor* a_ref = reinterpret_cast<Actor*>(a->GetUserData().pointer);
		Actor* b_ref = reinterpret_cast<Actor*>(b->GetUserData().pointer);

		if (!a->IsSensor() && !b->IsSensor()) {
			Collision collisionAB = GetCollisionData(contact, a, b, true);
			OnCollisionEnter(a_ref, collisionAB);

			Collision collisionBA = GetCollisionData(contact, b, a, true);
			collisionBA.relative_velocity = collisionAB.relative_velocity;
			OnCollisionEnter(b_ref, collisionBA);
		}
		else if (a->IsSensor() && b->IsSensor()) {
			Collision triggerAB = GetCollisionData(contact, a, b, false);
			OnTriggerEnter(a_ref, triggerAB);

			Collision triggerBA = GetCollisionData(contact, b, a, false);
			triggerBA.relative_velocity = triggerAB.relative_velocity;
			OnTriggerEnter(b_ref, triggerBA);
		}
	}

	void EndContact(b2Contact* contact) override {
		b2Fixture* a = contact->GetFixtureA();
		b2Fixture* b = contact->GetFixtureB();

		bool ignoreA = a->GetFilterData().categoryBits == 0 && a->GetFilterData().maskBits == 0;
		bool ignoreB = b->GetFilterData().categoryBits == 0 && b->GetFilterData().maskBits == 0;

		if (ignoreA || ignoreB) return;

		Actor* a_ref = reinterpret_cast<Actor*>(a->GetUserData().pointer);
		Actor* b_ref = reinterpret_cast<Actor*>(b->GetUserData().pointer);

		if (!a->IsSensor() && !b->IsSensor()) {
			Collision collisionAB = GetCollisionData(contact, a, b, false);
			OnCollisionExit(a_ref, collisionAB);

			Collision collisionBA = GetCollisionData(contact, b, a, false);
			collisionBA.relative_velocity = collisionAB.relative_velocity;
			OnCollisionExit(b_ref, collisionBA);
		}
		else if (a->IsSensor() && b->IsSensor()) {
			Collision triggerAB = GetCollisionData(contact, a, b, false);
			OnTriggerExit(a_ref, triggerAB);

			Collision triggerBA = GetCollisionData(contact, b, a, false);
			triggerBA.relative_velocity = triggerAB.relative_velocity;
			OnTriggerExit(b_ref, triggerBA);
		}
	}

	static void OnCollisionEnter(Actor* self, Collision collision) {
		for (auto &pair : self->components_by_key) {
			luabridge::LuaRef component_ref = *pair.second->ref;
			luabridge::LuaRef OnCollisionEnter = component_ref["OnCollisionEnter"];
			if (!OnCollisionEnter.isNil() && OnCollisionEnter.isFunction()) {
				try
				{ 
					OnCollisionEnter(component_ref, collision);
				}
				catch (luabridge::LuaException e)
				{
					EngineUtils::ReportLuaError(pair.second->owning_actor, e);
				}
			}
		}
	}

	static void OnCollisionExit(Actor* self, Collision collision) {
		for (auto &pair : self->components_by_key) {
			luabridge::LuaRef component_ref = *pair.second->ref;
			luabridge::LuaRef OnCollisionExit = component_ref["OnCollisionExit"];
			if (!OnCollisionExit.isNil() && OnCollisionExit.isFunction()) {
				try
				{
					OnCollisionExit(component_ref, collision);
				}
				catch (luabridge::LuaException e)
				{
					EngineUtils::ReportLuaError(pair.second->owning_actor, e);
				}
			} 
		}
	}

	static void OnTriggerEnter(Actor* self, Collision collision) {
		for (auto &pair : self->components_by_key) {
			luabridge::LuaRef component_ref = *pair.second->ref;
			luabridge::LuaRef OnTriggerEnter = component_ref["OnTriggerEnter"];
			if (!OnTriggerEnter.isNil() && OnTriggerEnter.isFunction()) {
				try
				{
					OnTriggerEnter(component_ref, collision);
				}
				catch (luabridge::LuaException e)
				{
					EngineUtils::ReportLuaError(pair.second->owning_actor, e);
				}
			}
		}
	}

	static void OnTriggerExit(Actor* self, Collision collision) {
		for (auto &pair : self->components_by_key) {
			luabridge::LuaRef component_ref = *pair.second->ref;
			luabridge::LuaRef OnTriggerExit = component_ref["OnTriggerExit"];
			if (!OnTriggerExit.isNil() && OnTriggerExit.isFunction()) {
				try
				{
					OnTriggerExit(component_ref, collision); 
				}
				catch (luabridge::LuaException e)
				{
					EngineUtils::ReportLuaError(pair.second->owning_actor, e);
				}
			}
		}
	}

	static Collision GetCollisionData(b2Contact* contact, b2Fixture* self, b2Fixture* other, bool want_all_data) {
		Collision collision;
		collision.other = reinterpret_cast<Actor*>(other->GetUserData().pointer);

		if (want_all_data) {
			b2WorldManifold world_manifold;
			contact->GetWorldManifold(&world_manifold);
			collision.point = world_manifold.points[0];
			collision.normal = world_manifold.normal;
		}
		else {
			b2Vec2 sentinel_value = b2Vec2(-999.0f, -999.0f);
			collision.point = sentinel_value;
			collision.normal = sentinel_value;
		}

		collision.relative_velocity = self->GetBody()->GetLinearVelocity() - other->GetBody()->GetLinearVelocity();

		return collision;
	}
};

#endif // !CONTACTLISTENER_H
