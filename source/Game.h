#pragma once
#ifndef NDEBUG
#include <vld.h>
#endif
#include "raylib.h"
#include <box2d.h>
#include <vector>
#include "PhysicsManager.h"
#include "StaticObjects.h"
#include "Config.h"
#include "Input.h"
#include "Elevator.h"
#include "DynamicObject.h"
#include "SpringBouncer.h"
class Game
{
public:
	Game();
	~Game();

	void inits();

	void run();

private:

	staticObjects* ground;
	staticObjects* wallLeft;
	staticObjects* wallRight;

	DynamicObject* box;
	DynamicObject* box2;
	Elevator* elevator;
	SpringBouncer* bouncer;

	void load();
	void inputs();

	physicsManager* physics;
	std::vector<staticObjects*> environment;//ambiente, suelo y paredes, dsp capaz le cambio el nombre de esto
};
