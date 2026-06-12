#include "Game.h"
Game::Game() : physics(nullptr)
{
	ground = nullptr;
	wallLeft = nullptr;
	wallRight = nullptr;

}

void Game::inits() {
	//iniciamos el mundo fisico
	physics = new physicsManager();

	b2World* world = physics->GetWorld();
	//iniciamos los objetos estaticos
	// Piso
	ground = new staticObjects(world, { (float)SCREEN_WIDTH / 2.0f, (float)SCREEN_HEIGHT - 20.0f }, { (float)SCREEN_WIDTH, 40.0f });
	ground->init();

	// Pared Izquierda
	wallLeft = new staticObjects(world, { 10.0f, (float)SCREEN_HEIGHT / 2.0f }, { 20.0f, (float)SCREEN_HEIGHT });
	wallLeft->init();

	// Pared Derecha
	wallRight = new staticObjects(world, { (float)SCREEN_WIDTH - 10.0f, (float)SCREEN_HEIGHT / 2.0f }, { 20.0f, (float)SCREEN_HEIGHT });
	wallRight->init();

	//los pongo en la lista para dibujarlos dsp
	environment.push_back(ground);
	environment.push_back(wallLeft);
	environment.push_back(wallRight);
	//elevador
	elevator = new Elevator(world, { 700.0f, 550.0f }, { 100.0f, 20.0f });
	//cajas
	box = new DynamicObject(world, { 700.0f, 400.0f }, { 40.0f, 40.0f }, 0.0f, RED);
	box->init(ShapeType::BOX);
	//resorte
	bouncer = new SpringBouncer(world, { 700.0f, 300.0f }, { 80.0f, 20.0f }, 180.0f,BLUE);
}

void Game::inputs() {
	bool elevatorM = false;
	if (InputManager::Down()) {
		elevator->update(1); elevatorM = true; 
	}
	else if (InputManager::Up()) {
		elevator->update(2); elevatorM = true; 
	}
	if (!elevatorM) elevator->update(0); // Si no se presiona W ni S, el ascensor se detiene creo probar bien
}

void Game::load() {
	//actualizamos la fisica
	physics->Update();
	//draws
	DrawText("Use W and S to move the elevator", 20, 10, 18, BLUE);
	//statics
	for (auto obj : environment) obj->draw();
	//elevador
	elevator->draw();
	//cajas
	box->draw();
	//resorte
	bouncer->draw();
}

void Game::run() {

	while (!WindowShouldClose())
	{
		inputs();
		BeginDrawing();
		ClearBackground(RAYWHITE);

		load();

		EndDrawing();
	}

}
Game::~Game()
{
}