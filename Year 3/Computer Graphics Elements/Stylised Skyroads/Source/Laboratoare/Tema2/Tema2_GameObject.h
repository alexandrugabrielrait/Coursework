#pragma once

#include <string>
#include <list>
#include <include/glm.h>
#include <algorithm>
#include <map>

using namespace std;
static map<string, glm::vec3> mapColors;

#define Tema3_par_deformation 1
#define Tema3_par_item 2

class Tema2_GameObject
{
public:
	float x = 0, y = 0, z = 0;
	float rOX = 0, rOY = 0, rOZ = 0, speed = 1;
	float vX = 0, vY = 0, vZ = 0, vROX = 0, vROY = 0, vROZ = 0;
	float sX = 1, sY = 1, sZ = 1;
	const float colliderScale = 1, colliderElevation = 0;
	string meshName, shaderName, texture1, texture2;
	glm::vec3 color;
	bool alive = true, hidden = false, flat = false;
	int parameters = 0;

	Tema2_GameObject(string meshName, string shaderName, float colliderScale = 1, float colliderElevation = 0) : colliderScale(colliderScale), colliderElevation(colliderElevation)
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
		z += vZ * speed * deltaTimeSeconds;
		rOX += vROX * deltaTimeSeconds;
		rOY += vROY * deltaTimeSeconds;
		rOZ += vROZ * deltaTimeSeconds;
		OnUpdate(deltaTimeSeconds);
	}

	/*
		Gets the y coordinate of the center of the collider
	*/
	float GetColliderElevation()
	{
		return y + sY * colliderElevation;
	}

	/*
		Check all collisions.
	*/
	void DoCollisions(list<Tema2_GameObject*> gameObjects)
	{
		if (colliderScale != 0)
			for (Tema2_GameObject *object : gameObjects)
			{
				if (CheckCollision(object))
					OnCollision(object);
			}
	}

	/*
		Check if this object is colliding with the parameter object.
	*/
	bool CheckCollision(Tema2_GameObject *object)
	{
		if (this == object || object->colliderScale == 0)
			return false;
		float minX = x - colliderScale * sX / 2, oMinX = object->x - object->colliderScale * object->sX / 2;
		float maxX = x + colliderScale * sX / 2, oMaxX = object->x + object->colliderScale * object->sX / 2;
		float minY = GetColliderElevation() - colliderScale * sY / 2, oMinY = object->GetColliderElevation() - object->colliderScale * object->sY / 2;
		float maxY = GetColliderElevation() + colliderScale * sY / 2, oMaxY = object->GetColliderElevation() + object->colliderScale * object->sY / 2;
		float minZ = z - colliderScale * sZ / 2, oMinZ = object->z - object->colliderScale * object->sZ / 2;
		float maxZ = z + colliderScale * sZ / 2, oMaxZ = object->z + object->colliderScale * object->sZ / 2;
		return (minX <= oMaxX && maxX >= oMinX) &&
			(minY <= oMaxY && maxY >= oMinY) &&
			(minZ <= oMaxZ && maxZ >= oMinZ);
	}

	/*
		Customizable Collision Trigger.
	*/
	virtual void OnCollision(Tema2_GameObject *object)
	{

	}

	/*
		Kills the object.
	*/
	void Die()
	{
		if (!alive)
			return;
		alive = false;
		OnDeath();
	}

	/*
		Customizable Death Trigger.
	*/
	virtual void OnDeath()
	{

	}
};