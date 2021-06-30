#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>
#include <random>
#include "Transform2D_Tema1.h"
#include "GameObject.h"
#include "Object2D_Tema1.h"
#include "Player.h"
#include "Bow.h"
#include "Arrow.h"
#include "Balloon.h"
#include "ColorManager.h"
#include "LevelManager.h"

class Tema1 : public SimpleScene
{
public:
	Tema1();
	~Tema1();

	/*
		Gets called at the start of the program.
	*/
	void Init() override;

private:
	/*
		Gets called before Update.
	*/
	void FrameStart() override;
	/*
		Periodic Update.
	*/
	void Update(float deltaTimeSeconds) override;
	/*
		Gets called after Update.
	*/
	void FrameEnd() override;
	/*
		Checks for held keys and mouse buttons.
	*/
	void OnInputUpdate(float deltaTime, int mods) override;
	/*
		Checks for pressed keys.
	*/
	void OnKeyPress(int key, int mods) override;
	/*
		Checks for released keys.
	*/
	void OnKeyRelease(int key, int mods) override;
	/*
		Checks for mouse movements.
	*/
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	/*
		Checks for pressed mouse buttons.
	*/
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	/*
		Checks for released mouse buttons.
	*/
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	/*
		Checks for mouse scrolling.
	*/
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	/*
		Called when window is rezised.
	*/
	void OnWindowResize(int width, int height) override;
	/*
		Checks if object should die. If it doesn't it will be rendered.
	*/
	void UpdateObject(GameObject *object, float deltaTimeSeconds);
	/*
		Tries to fire arrows from the bow.
	*/
	void TryFire();
	/*
		Fires an arrow based of a given template.
	*/
	void Fire(Arrow::ArrowObject * arrowBase);
	/*
		Spawns a Ballon at the given x coordinate
	*/
	void SpawnBalloon(Balloon::balloon_type type, float x);
	/*
		Spawns a Shuriken at the given y coordinate
	*/
	void SpawnShuriken(string shuriken_name, float y);
	/*
		Spawns an object at a random coordinate
	*/
	void SpawnByName(string name);
	/*
		Resets the game and loads the specified level.
	*/
	void StartGame(int level);

protected:
	float mouseCoordX, mouseCoordY;
	float bowSpeed = 400;
	float windowWidth, windowHeight, elapsedLevelTime = 0, latestSpawn = -1;
	list<GameObject*> gameObjects;
	GameObject* cursorN, * cursorS, * cursorW, * cursorE;
	int basicObjectCount, scoreMax;
};