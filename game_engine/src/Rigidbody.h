#ifndef RIGIDBODY_H
#define RIGIDBODY_H

//STL
#include <string>

//Third Party
#include "box2d/box2d.h"
#include "glm/glm.hpp"

//Internal
#include "PhysicsManager.h"
#include "ContactListener.h"
#include "EngineUtils.h"
#include "Actor.h"

class Rigidbody
{
public:
	Actor* actor;
	bool enabled;
	std::string type;
	std::string key;

	b2Body* body;

	float x;
	float y;

	std::string body_type;

	bool precise;
	float gravity_scale;
	float density;
	float angular_friction;
	float rotation;

	bool has_collider;
	std::string collider_type;
	float width;
	float height;
	float radius;
	float friction;
	float bounciness;

	bool has_trigger;
	std::string trigger_type;
	float trigger_width;
	float trigger_height;
	float trigger_radius;

	Rigidbody() {
		actor = nullptr;
		enabled = true;
		type = "Rigidbody";
		key = "";
		body = nullptr;
		x = 0.0f;
		y = 0.0f;
		body_type = "dynamic";
		precise = true;
		gravity_scale = 1.0f;
		density = 1.0f;
		angular_friction = 0.3f;
		rotation = 0.0f;
		has_collider = true;
		collider_type = "box";
		width = 1.0f;
		height = 1.0f;
		radius = 0.5f;
		friction = 0.3f;
		bounciness = 0.3f; 
		has_trigger = true;
		trigger_type = "box";
		trigger_width = 1.0f;
		trigger_height = 1.0f;
		trigger_radius = 1.0f;
	}

	void Initialize() {
		b2BodyDef bodyDef;

		bodyDef.position = b2Vec2(x, y);

		bodyDef.angle = EngineUtils::ConvertDegreesToRads(rotation);

		if (body_type == "dynamic") {
			bodyDef.type = b2_dynamicBody;
		}
		else if (body_type == "static") {
			bodyDef.type = b2_staticBody;
		}
		else if (body_type == "kinematic") {
			bodyDef.type = b2_kinematicBody;
		}

		bodyDef.bullet = precise;
		bodyDef.gravityScale = gravity_scale;
		bodyDef.angularDamping = angular_friction;

		body = PhysicsManager::world->CreateBody(&bodyDef);
	}

	void OnStart() {
		if (!body) Initialize();

		if (!has_collider && !has_trigger) {
			b2PolygonShape phantom_shape;
			phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);

			b2FixtureDef fixture;
			fixture.density = density;
			fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
			fixture.shape = &phantom_shape;
			fixture.isSensor = true;

			fixture.filter.categoryBits = 0x0000;
			fixture.filter.maskBits = 0x0000;

			body->CreateFixture(&fixture);
		}
		else {
			if (has_collider) {
				b2FixtureDef collider_fixture;
				collider_fixture.density = density;
				collider_fixture.friction = friction;
				collider_fixture.restitution = bounciness;
				collider_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
				collider_fixture.isSensor = false;

				if (collider_type == "box") {
					b2PolygonShape box_collider;
					box_collider.SetAsBox(width * 0.5f, height * 0.5f);
					collider_fixture.shape = &box_collider;
					body->CreateFixture(&collider_fixture);
				}
				else if (collider_type == "circle") {
					b2CircleShape circle_collider;
					circle_collider.m_radius = radius;
					collider_fixture.shape = &circle_collider;
					body->CreateFixture(&collider_fixture);
				}
			}
			if (has_trigger) {
				b2FixtureDef trigger_fixture;
				trigger_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
				trigger_fixture.density = density;
				trigger_fixture.isSensor = true;

				if (trigger_type == "box") {
					b2PolygonShape box_trigger;
					box_trigger.SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
					trigger_fixture.shape = &box_trigger;
					body->CreateFixture(&trigger_fixture);
				}
				else if (trigger_type == "circle") {
					b2CircleShape circle_trigger;
					circle_trigger.m_radius = trigger_radius;
					trigger_fixture.shape = &circle_trigger;
					body->CreateFixture(&trigger_fixture);
				}
			}
		}
	}

	void OnDestroy() {
		PhysicsManager::world->DestroyBody(body);
	}

	b2Vec2 GetPosition() {
		if (!body) Initialize();
		return body->GetPosition();
	}

	float GetRotation() {
		if (!body) Initialize();
		return EngineUtils::ConvertRadsToDegrees(body->GetAngle());
	}

	void AddForce(b2Vec2 force) {
		if (!body) Initialize();
		body->ApplyForceToCenter(force, true);
	}

	void SetVelocity(b2Vec2 velocity) {
		if (!body) Initialize();
		body->SetLinearVelocity(velocity);
	}

	void SetPosition(b2Vec2 position) {
		if (!body) Initialize();
		body->SetTransform(position, GetRotation());
	}

	void SetRotation(float degrees_clockwise) {
		if (!body) Initialize();
		body->SetTransform(GetPosition(), EngineUtils::ConvertDegreesToRads(degrees_clockwise));
	}

	void SetAngularVelocity(float degrees_clockwise) { 
		if (!body) Initialize();
		body->SetAngularVelocity(EngineUtils::ConvertDegreesToRads(degrees_clockwise));
	}

	void SetGravityScale(float scale) {
		if (!body) Initialize();
		body->SetGravityScale(scale);
	}

	void SetUpDirection(b2Vec2 direction) {
		if (!body) Initialize();

		if (direction.LengthSquared() > 0.0f) direction.Normalize();
		else return;

		float rotation_rads = body->GetAngle();

		b2Vec2 local_up = GetUpDirection();

		float angle_between = glm::atan(direction.x, -direction.y) - glm::atan(local_up.x, -local_up.y);

		float new_body_angle = rotation_rads + angle_between;

		body->SetTransform(body->GetPosition(), new_body_angle);
	}

	void SetRightDirection(b2Vec2 direction) {
		if (!body) Initialize();
		SetUpDirection(direction);
		body->SetTransform(body->GetPosition(), body->GetAngle() - (b2_pi / 2.0f));
	}

	b2Vec2 GetVelocity() {
		if (!body) Initialize();
		return body->GetLinearVelocity();
	}

	float GetAngularVelocity() {
		if (!body) Initialize();
		return EngineUtils::ConvertRadsToDegrees(body->GetAngularVelocity());
	}

	float GetGravityScale() {
		if (!body) Initialize();
		return body->GetGravityScale();
	}

	b2Vec2 GetUpDirection() {
		if (!body) Initialize();
		float rotation_rads = body->GetAngle();
		return b2Vec2(glm::sin(rotation_rads), -glm::cos(rotation_rads));
	}

	b2Vec2 GetRightDirection() {
		if (!body) Initialize();
		float rotation_rads = body->GetAngle();
		return b2Vec2(glm::cos(rotation_rads), glm::sin(rotation_rads));
	}
};

#endif
