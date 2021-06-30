#include "Tema1.h"

#include <vector>
#include <iostream>
#include <Core/Engine.h>

using namespace std;
using namespace Player;
using namespace Bow;
using namespace Arrow;
using namespace Balloon;
using namespace LevelManager;

float maxProgress = arrowSpeedMax - arrowSpeedMin;
float bowLength = arrowLength, shurikenLength = 20;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::StartGame(int level)
{
	gameObjects.remove(bow);
	gameObjects.remove(powerBar);
	gameObjects.remove(timeBar);
	gameObjects.remove(cursorN);
	gameObjects.remove(cursorS);
	gameObjects.remove(cursorW);
	gameObjects.remove(cursorE);

	for (int i = 0; i < 3; ++i)
	{
		gameObjects.remove(heartObjects[i]);
	}

	for (GameObject* object : gameObjects)
	{
		delete(object);
	}
	gameObjects.clear();

	lives = 3;
	ShowHearts();
	score = 0;
	multiShot = false; bow->x = 100;
	bow->y = windowHeight / 2;
	powerBar->sX = 0;
	powerBar->x = bow->x;
	powerBar->y = bow->y - 1.5 * bowLength;
	gameObjects.push_back(bow);
	gameObjects.push_back(powerBar);
	gameObjects.push_back(timeBar);
	gameObjects.push_back(cursorN);
	gameObjects.push_back(cursorS);
	gameObjects.push_back(cursorW);
	gameObjects.push_back(cursorE);

	for (int i = 0; i < 3; ++i)
	{
		gameObjects.push_back(heartObjects[i]);
	}

	basicObjectCount = gameObjects.size();
	gameOver = false;
	scoreMax = 0;
	elapsedLevelTime = 0;
	cooldown = 0;
	cooldownTimeStop = 0;
	lengthTimeStop = 0;
	latestSpawn = -1;
	bow->hidden = false;
	timeBar->hidden = false;
	LoadLevel(level);
	cout << "Score: " << score << endl;
}

void Tema1::Fire(ArrowObject* arrowBase)
{
	ArrowObject* arrow = new ArrowObject(arrowBase->arrowMesh, "VertexColor");
	arrow->x = bow->x;
	arrow->y = bow->y;
	arrow->r = arrowBase->r;
	arrow->speed = MIN(arrowSpeedMin + additionalSpeed, arrowSpeedMax);
	arrow->vX -= sin(arrow->r);
	arrow->vY += cos(arrow->r);
	gameObjects.push_back(arrow);
	arrowBase->arrowMesh = meshes["arrow"];
}

void Tema1::TryFire()
{
	if (lives > 0 && cooldown <= 0)
	{
		cooldown = cooldownMax;
		Fire(ammoDisplay[1]);
		if (multiShot)
		{
			Fire(ammoDisplay[0]);
			Fire(ammoDisplay[2]);
		}
		for (int i = 0; i < 3; ++i)
		{
			ammoDisplay[i]->arrowMesh = meshes["arrow"];
		}
		powerBar->sX = 0;
		additionalSpeed = 0;
		multiShot = false;
	}
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	float squareSide = 100;
	glm::vec3 corner = glm::vec3(-squareSide / 2, -squareSide / 2, 0);
	windowWidth = resolution.x;
	windowHeight = resolution.y;

	Mesh* mesh = Object2D_Tema1::CreateSquare("cursor", corner, squareSide, ColorManager::white, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBow("bow", bowLength, ColorManager::wood, ColorManager::string);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBow("bow_pulled", bowLength, ColorManager::wood, ColorManager::string, arrowLength);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBow("bow_pulled_full", bowLength, ColorManager::wood, ColorManager::tension, arrowLength);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateSquare("power_bar", corner, squareSide, ColorManager::blue, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateSquare("power_bar_full", corner, squareSide, ColorManager::tension, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateSquare("time_bar_recharge", corner, squareSide, ColorManager::grey, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateSquare("time_bar_full", corner, squareSide, ColorManager::gold, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateHeart("heart", heartSize, ColorManager::red, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateArrow("arrow", ColorManager::yellow, true);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBalloon("balloon", balloonLength, ColorManager::red, ColorManager::string);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBalloon("balloon_yellow", balloonLength, ColorManager::yellow, ColorManager::string);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBalloon("balloon_green", balloonLength, ColorManager::green, ColorManager::red);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateBalloon("balloon_blue", balloonLength, ColorManager::blue, ColorManager::tension);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateShuriken("shuriken", shurikenLength, ColorManager::cyan);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateShuriken("shuriken_reflective", shurikenLength, ColorManager::magenta);
	AddMeshToList(mesh);

	mesh = Object2D_Tema1::CreateShuriken("shuriken_inactive", shurikenLength, ColorManager::red);
	AddMeshToList(mesh);

	bow = new BowObject("VertexColor");
	bow->sY /= 2;
	cursorN = new GameObject("cursor", "VertexColor", 0);
	cursorN->sX /= 20;
	cursorN->sY /= 10;
	cursorS = new GameObject("cursor", "VertexColor", 0);
	cursorS->sX /= 20;
	cursorS->sY /= 10;
	cursorW = new GameObject("cursor", "VertexColor", 0);
	cursorW->sX /= 10;
	cursorW->sY /= 20;
	cursorE = new GameObject("cursor", "VertexColor", 0);
	cursorE->sX /= 10;
	cursorE->sY /= 20;
	for (int i = 0; i < 3; ++i)
	{
		ammoDisplay[i] = new ArrowObject(meshes["arrow"], "VertexColor");
		ammoDisplay[i]->colliderRadius = 0;
	}
	powerBar = new GameObject("power_bar", "VertexColor", 0);
	powerBar->sY /= 8;
	timeBar = new GameObject("time_bar_full", "VertexColor", 0);
	timeBar->sY /= 8;
	timeBar->x = 7 * heartSize;
	timeBar->y = resolution.y - 5 * heartSize;

	for (int i = 0; i < 3; ++i)
	{
		heartObjects[i] = new GameObject("heart", "VertexColor", 0);
		heartObjects[i]->x = heartSize * (2.5 + 4.5 * i);
		heartObjects[i]->y = resolution.y - 1.5 * heartSize;
	}

	StartGame(1);
}

void Tema1::FrameStart()
{
	if (gamePaused)
		glClearColor(0, 0, 0.1, 1);
	else if (timeStopped)
		glClearColor(0, 0.1, 0, 1);
	else
		glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::UpdateObject(GameObject* object, float deltaTimeSeconds)
{
	object->Update(deltaTimeSeconds);
	if (object->x < -windowHeight * 0.25 || object->x > windowWidth * 1.25 || object->y < -windowHeight * 0.25 || object->y >= windowHeight * 1.25)
	{
		object->Die();
	}
	if (!object->hidden)
	{
		glm::mat3 modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D_Tema1::Translate(object->x, object->y);
		modelMatrix *= Transform2D_Tema1::Rotate(object->r);
		modelMatrix *= Transform2D_Tema1::Scale(object->sX, object->sY);
		if (Arrow::ArrowObject* arrow = dynamic_cast<Arrow::ArrowObject*>(object))
			RenderMesh2D(arrow->arrowMesh, shaders[arrow->shaderName], modelMatrix);
		else
			RenderMesh2D(meshes[object->meshName], shaders[object->shaderName], modelMatrix);
	}
	object->DoCollisions(gameObjects);
}

void Tema1::Update(float deltaTimeSeconds)
{
	if (gamePaused)
		deltaTimeSeconds = 0;
	else
	{
		bow->y = MAX(0, MIN(windowHeight, bow->y));
		float distanceX = MAX(0, mouseCoordX - bow->x);
		float distanceY = mouseCoordY - bow->y;
		bow->r = -atan2(distanceX, distanceY);
	}
	float progress = MIN(additionalSpeed, maxProgress) / maxProgress;
	powerBar->sX = progress;
	ammoDisplay[1]->arrowMesh = Object2D_Tema1::CreateArrow("arrow", glm::vec3(1, 1 - progress, 0), true);
	for (int i = 0; i < 3; i += 2)
		ammoDisplay[i]->arrowMesh = ammoDisplay[1]->arrowMesh;
	if (progress == 1)
	{
		bow->meshName = "bow_pulled_full";
		powerBar->meshName = "power_bar_full";
	}
	if (lives > 0 && cooldown <= 0)
	{
		if (bow->meshName == "bow")
		{
			bow->meshName = "bow_pulled";
			powerBar->meshName = "power_bar";
		}
		ammoDisplay[1]->x = bow->x;
		ammoDisplay[1]->y = bow->y;
		ammoDisplay[1]->r = bow->r;
		UpdateObject(ammoDisplay[1], deltaTimeSeconds);
		if (multiShot)
			for (int i = 0; i < 3; i += 2)
			{
				ammoDisplay[i]->x = bow->x;
				ammoDisplay[i]->y = bow->y;
				ammoDisplay[i]->r = bow->r + (i - 1) * M_PI / 12;
				UpdateObject(ammoDisplay[i], deltaTimeSeconds);
			}
	}
	else
		bow->meshName = "bow";
	cooldown -= deltaTimeSeconds;
	lengthTimeStop -= deltaTimeSeconds;
	if (lengthTimeStop > 0)
	{
		deltaTimeSeconds = 0;
		timeBar->sX = lengthTimeStop / lengthTimeStopMax;
	}
	else {
		if (timeStopped)
		{
			timeStopped = false;
			cout << "Time resumes!" << endl;
		}
		if (cooldownTimeStop <= 0)
		{
			timeBar->meshName = "time_bar_full";
			timeBar->sX = 1;
		}
		else
		{
			timeBar->meshName = "time_bar_recharge";
			timeBar->sX = 1 - cooldownTimeStop / cooldownTimeStopMax;
		}
	}
	float roundedElapsedTime = floor(elapsedLevelTime);
	if (latestSpawn < roundedElapsedTime && spawnData.find(roundedElapsedTime) != spawnData.end())
	{
		for (string name : spawnData[roundedElapsedTime])
		{
			SpawnByName(name);
		}
		latestSpawn = roundedElapsedTime;
	}
	elapsedLevelTime += deltaTimeSeconds;
	cooldownTimeStop -= deltaTimeSeconds;
	for (GameObject *object : gameObjects)
	{
		UpdateObject(object, deltaTimeSeconds);
	}
	for (auto it = gameObjects.begin(); it != gameObjects.end();)
	{
		GameObject* object = *it;
		if (!object->alive)
		{
			delete(object);
			it = gameObjects.erase(it);
			continue;
		}
		else
			++it;
	}
	if (!gameOver && elapsedLevelTime > lastSpawn&& basicObjectCount >= gameObjects.size())
	{
		gameOver = true;
		cout << "Level " << currentLevel << " finished! Score: " << score << "/" << scoreMax << endl;
		if (currentLevel < levelMax)
			cout << "Press N to continue." << endl;
		else
			cout << "You have finished the last level. Congratulations!" << endl;
	}
}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	if (gamePaused)
		deltaTime = 0;
	if (window->KeyHold(GLFW_KEY_W))
	{
		bow->y = MIN(windowHeight, bow->y + bowSpeed * deltaTime);
		powerBar->y = bow->y - 1.5 * bowLength;
	}
	if (window->KeyHold(GLFW_KEY_S))
	{
		bow->y = MAX(0, bow->y - bowSpeed * deltaTime);
		powerBar->y = bow->y - 1.5 * bowLength;
	}
	if (lives > 0 && cooldown <= 0 && window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
	{
		additionalSpeed += arrowSpeedIncrement * deltaTime;
	}
}

void Tema1::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_N && gameOver && !bow->hidden && currentLevel < levelMax)
	{
		StartGame(currentLevel + 1);
	}
	if (key == GLFW_KEY_K)
	{
		cheatMode = !cheatMode;
		if (cheatMode)
			cout << "CHEAT MODE ACTIVATED! USE NUMBER KEYS TO SWITCH LEVELS!" << endl;
		else
			cout << "CHEAT MODE DEACTIVATED!" << endl;
	}
	if (cheatMode)
		for (int i = 1; i <= levelMax; ++i)
		{
			if (key == GLFW_KEY_0 + i)
			{
				StartGame(i);
			}
		}
	if (key == GLFW_KEY_R)
	{
		StartGame(currentLevel);
	}
	if (key == GLFW_KEY_P)
	{
		gamePaused = !gamePaused;
	}
	if (gamePaused)
		return;
	if (key == GLFW_KEY_T)
	{
		if (lengthTimeStop > 0)
		{
			lengthTimeStop = 0;
		}
		else if (cooldownTimeStop <= 0)
		{
			cout << "Time has been stopped!" << endl;
			lengthTimeStop = lengthTimeStopMax;
			cooldownTimeStop = cooldownTimeStopMax;
			timeStopped = true;
		}
		else
		{
			cout << "Time Stop not ready! " << ceil(cooldownTimeStop) << " seconds remanining!" << endl;
		}
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
}

const float cursorDistance = 10;

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	mouseCoordX = mouseX * windowWidth / window->GetResolution().x;
	mouseCoordY = windowHeight - mouseY * windowHeight / window->GetResolution().y;
	cursorN->x = mouseCoordX;
	cursorN->y = mouseCoordY + cursorDistance;
	cursorS->x = mouseCoordX;
	cursorS->y = mouseCoordY - cursorDistance;
	cursorW->x = mouseCoordX - cursorDistance;
	cursorW->y = mouseCoordY;
	cursorE->x = mouseCoordX + cursorDistance;
	cursorE->y = mouseCoordY;
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (gamePaused)
	{
		additionalSpeed = 0;
		return;
	}
	TryFire();
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}

void Tema1::SpawnBalloon(balloon_type type, float x)
{
	GameObject* object = new BalloonObject(type, "VertexColor");
	object->x = x;
	object->y = -2 * balloonLength;
	gameObjects.push_back(object);
}

void Tema1::SpawnShuriken(string shuriken_name, float y)
{
	GameObject* object = new GameObject(shuriken_name, "VertexColor", balloonLength);
	object->vX = -200;
	object->vR = 2 * M_PI;
	object->x = windowWidth + 2 * balloonLength;
	object->y = y;
	gameObjects.push_back(object);
}

void Tema1::SpawnByName(string name)
{
	float coord;
	if (name.c_str()[0] == 'b')
		coord = windowWidth * 0.2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (windowWidth * 0.8)));
	else
		coord = windowHeight * 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (windowHeight * 0.8)));
	if (name == "br")
	{
		SpawnBalloon(balloon_type::red, coord);
		scoreMax += 10;
	}
	else if (name == "by")
	{
		SpawnBalloon(balloon_type::yellow, coord);
	}
	else if (name == "bg")
	{
		SpawnBalloon(balloon_type::green, coord);
	}
	else if (name == "bb")
	{
		SpawnBalloon(balloon_type::blue, coord);
	}
	else if (name == "sh")
	{
		SpawnShuriken("shuriken", coord);
		scoreMax += 5;
	}
	else if (name == "sr")
	{
		SpawnShuriken("shuriken_reflective", coord);
		scoreMax += 5;
	}
}
