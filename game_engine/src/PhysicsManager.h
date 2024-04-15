#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

//STL

//Third Party
#include "box2d/box2d.h"
#include "ContactListener.h"

class PhysicsManager
{
public:
	static inline bool simulating = false;

	const static inline float update_frequency = 1.0f / 60.0f;

	static inline b2World* world = nullptr;
	static inline b2Vec2 gravity = b2Vec2(0.0f, 9.8f);

	static void Initialize() {
		world = new b2World(gravity);
		ContactListener* listener = new ContactListener();
		PhysicsManager::world->SetContactListener(listener);
		simulating = true;
	}

	static void Step() {
		world->Step(update_frequency, 8, 3);
	}
};

#endif

