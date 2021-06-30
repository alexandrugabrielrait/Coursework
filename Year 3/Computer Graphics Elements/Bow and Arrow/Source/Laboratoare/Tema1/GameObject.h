#pragma once

#include <string>
#include <list>
#include <include/glm.h>
#include <algorithm>
#include <iostream>

using namespace std;

class GameObject
{
public:
	float x = 0, y = 0;
	float r = 0, speed = 1;
	float vX = 0, vY = 0, vR = 0;
	float sX = 1, sY = 1;
	const float colliderX = 0, colliderY = 0;
	float colliderRadius;
	string meshName, shaderName;
	bool alive = true, hidden = false;

	GameObject(string meshName, string shaderName, float colliderRadius, float colliderX = 0, float colliderY = 0) :
		colliderX(colliderX), colliderY(colliderY), colliderRadius(colliderRadius)
	{
		this->meshName = meshName;
		this->shaderName = shaderName;
	}

	/*
		Customizable Update.
	*/
	virtual void OnUpdate(float deltaTimeSconds)
	{

	}

	/*
		Periodic Update.
	*/
	void Update(float deltaTimeSeconds)
	{
		x += vX * speed * deltaTimeSeconds;
		y += vY * speed * deltaTimeSeconds;
		r += vR * deltaTimeSeconds;
		OnUpdate(deltaTimeSeconds);
	}

	/*
		Get collider coordinate x based on object rotation.
	*/
	float GetColliderX()
	{
		return x + (cos(r) * sX * colliderX - sin(r) * sY * colliderY);
	}

	/*
		Get collider coordinate y based on object rotation.
	*/
	float GetColliderY()
	{
		return y + sY * (sin(r) * sX * colliderX + cos(r) * sY * colliderY);
	}

	/*
		Get collider radius based on object rotation and size.
	*/
	float AngledColliderRadius(float deltaX, float deltaY)
	{
		float angle = atan2(deltaX, deltaY) - r;
		float a = sX * colliderRadius;
		float b = sY * colliderRadius;

		return (a * b) / sqrt(a * a * sin(angle) * sin(angle) + b * b * cos(angle) * cos(angle));
	}

	/*
		Check all collisions.
	*/
	void DoCollisions(list<GameObject*> gameObjects)
	{
		if (colliderRadius > 0)
			for (GameObject *object : gameObjects)
			{
				if (CheckCollision(object))
					OnCollision(object);
			}
	}

	/*
		Check if this object is colliding with the parameter object.
	*/
	bool CheckCollision(GameObject *object)
	{
		if (this == object || object->colliderRadius <= 0)
			return false;
		float deltaX = object->GetColliderX() - GetColliderX();
		float deltaY = object->GetColliderY() - GetColliderY();
		float distance = sqrt(deltaX * deltaX + deltaY * deltaY);
		return distance < AngledColliderRadius(deltaX, deltaY) + object->AngledColliderRadius(-deltaX, -deltaY);
	}

	/*
		Customizable Collision Trigger.
	*/
	virtual void OnCollision(GameObject *object)
	{

	}

	/*
		Kills the object.
	*/
	void Die()
	{
		alive = false;
	}
};