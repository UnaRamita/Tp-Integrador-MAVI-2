#include "Game.h"
Game::Game() : physics(nullptr)
{
	ground = nullptr;
	wallLeft = nullptr;
	wallRight = nullptr;
	wallElevator = nullptr;
	Background = {0};

	amunition = 1;

}

void Game::inits() {
	Background = LoadTexture("assets/background_glacial_mountains.png");
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
	//pared para que no se salga la caja
	wallElevator = new staticObjects(world, { 625.0f, 350.0f }, { 20.0f, 600.0f });
	wallElevator->init();

	//los pongo en la lista para dibujarlos dsp
	environment.push_back(ground);
	environment.push_back(wallLeft);
	environment.push_back(wallRight);
	environment.push_back(wallElevator);	
	//pared objetivo
	targetBody = new staticObjects(world, { 600.0f, 150.0f }, { 10.0f, 50.0f });
	targetBody->init();
	environment.push_back(targetBody);
	//elevador
	elevator = new Elevator(world, { 700.0f, 550.0f }, { 100.0f, 20.0f });
	//cajas
	box = new DynamicObject(world, { 700.0f, 400.0f }, { 40.0f, 40.0f }, 0.0f, RED);
	box->init(ShapeType::BOX);
	//resorte
	bouncer = new SpringBouncer(world, { 700.0f, 300.0f }, { 80.0f, 20.0f }, 180.0f,BLUE);
	//preview
	preview = new Preview({ 100.0f, (float)SCREEN_HEIGHT - 60.0f });
}

void Game::inputs() {
	bool elevatorM = false;
	if (InputManager::Space() && amunition>0) {
		amunition--;
		// Paso el angulo de la preview
		float angleInRad = preview->GetAngle() * DEG2RAD;
		//para el size en vez de pasar el radio, paso el diametro (radio*2)
		float diameter = (PROYECTILE_RADIUS * SCALE) * 2.0f;

		DynamicObject* projectile = new DynamicObject(physics->GetWorld(), preview->GetPosition(), { diameter,diameter }, angleInRad, GRAY);
		//le digo que es un circulo
		projectile->init(ShapeType::CIRCLE);

		//aca ya esta creado el proyectil y le aplico el impulso para lanzarlo, dsp lo dibujo en el draw
		float launchPower = 15.0f; //si quisiera poner algun input para variar la potencia del lanzamiento, pondria esto en otro lado
		//creo el vector para el impulso, con coseno y seno para que sea en la direccion de la preview
		Vector2 impulse = { cosf(angleInRad) * launchPower, sinf(angleInRad) * launchPower };
		//aplico el impulso al proyectil
		projectile->applyImpulse(impulse);
		//lo agrego a la lista de proyectiles para dibujarlo dsp
		projectiles.push_back(projectile);

	}
	if (InputManager::Down()) {
		elevator->update(1); elevatorM = true; 
	}
	else if (InputManager::Up()) {
		elevator->update(2); elevatorM = true; 
	}
	if (InputManager::A()) {
		preview->Rotate(-1);
	}
	else if (InputManager::D()) {
		preview->Rotate(1);
	}
	if (!elevatorM) elevator->update(0); // Si no se presiona W ni S, el ascensor se detiene creo probar bien
}
void Game::collider() {
	// Recorremos todos los contactos del mundo
	b2World* world = physics->GetWorld();
	b2Body* targetB2Body = targetBody->GetBody();

	for (b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext()) {
		if (contact->IsTouching()) {
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// Chequeamos si alguno de los dos cuerpos involucrados en el choque es la pared roja
			if (bodyA == targetB2Body || bodyB == targetB2Body) {

				// Si A es la pared, la bala debe ser B. Si no, al revés.
				b2Body* hitBody = (bodyA == targetB2Body) ? bodyB : bodyA;

				// Revisamos si el objeto que chocó contra la pared es un proyectil
				for (DynamicObject* proj : projectiles) {
					if (proj->GetBody() == hitBody) {

						// Confirmamos que es una bala. Ahora chequeamos si YA sumó puntos antes.
						auto it = std::find(scoredProjectiles.begin(), scoredProjectiles.end(), proj);

						if (it == scoredProjectiles.end()) {
							// Como no está en la lista de los que ya sumaron, es un impacto nuevo.
							scoredProjectiles.push_back(proj); // Lo anotamos para que no vuelva a sumar

							if (!gameWon) {
								score++;
								if (score >= 5) gameWon = true;
							}
						}
						break; // Dejamos de buscar en la lista de proyectiles
					}
				}
			}
		}
	}
	Rectangle rechargeZone = { 650.0f, 275.0f, 100.0f, 50.0f }; 
	
	// Obtenemos la posición actual del cubo en píxeles
	b2Vec2 boxPosFeet = box->GetBody()->GetPosition();
	Vector2 boxPosPixels = { boxPosFeet.x * SCALE, boxPosFeet.y * SCALE };

	// Como Box2D tiene el origen en el CENTRO, para Raylib le restamos la mitad del ancho/alto (40 / 2 = 20)
	Rectangle boxRect = {boxPosPixels.x - 20.0f, boxPosPixels.y - 20.0f,40.0f, 40.0f };
	
	// CheckCollisionPointRec comprueba si el centro del cubo entró al rectángulo de la zona
	if (CheckCollisionRecs(boxRect, rechargeZone)) {
		if (!boxInZone) {
			amunition++;      // Suma 1 bala
			boxInZone = true; // Bloquea para que no sume en bucle
		}
	}
	else {
		boxInZone = false;    // Cuando se separan, se libera el candado
	}
}

void Game::load() {
	//fondo
	DrawTexture(Background, 0, 0, WHITE);
	//actualizamos la fisica
	physics->Update();
	//draws texts
	DrawText("Use W and S to move the elevator, A and D to rotate the direction", 20, 10, 18, BLUE);
	DrawText(TextFormat("Puntaje: %d / 5", score), 20, 40, 22, BLACK);
	DrawText(TextFormat("Balas: %d", amunition), 20, 70, 22, BLACK);
	if (gameWon) {
        DrawText("¡GANASTE!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 40, GREEN);
    }
	//statics
	for (auto obj : environment) obj->draw();
	//projectiles
	preview->Draw();
	for (auto proj : projectiles) proj->draw();
	//elevador
	elevator->draw();
	//cajas
	box->draw();
	//resorte
	bouncer->draw();
	//collision detect
	collider();
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