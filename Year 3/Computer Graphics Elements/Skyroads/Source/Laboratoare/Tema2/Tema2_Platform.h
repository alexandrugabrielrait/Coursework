#pragma once
#include "Tema2_GameObject.h"
#include <iostream>
#include "Tema2_Player.h"

namespace Tema2_Platform
{
	static const float laneHeight = 0.25, destroyDistance = 3;

	class PlatformObject : public Tema2_GameObject
	{
	private:
		bool activated = false;

	public:
		PlatformObject(string meshName, int lane, float positionZ, float lengthZ) : Tema2_GameObject(meshName, "VertexColor")
		{
			sX = Tema2_Player::laneLength;
			sY = laneHeight;
			sZ = lengthZ;
			x = lane * Tema2_Player::laneLength;
			z = -(positionZ + sZ / 2 - 0.5);
		}

		void OnUpdate(float deltaTimeSeconds) override
		{
			// if the ball gets too far from the platform, the platform will be destroyed
			if (z - sZ/2 - Tema2_Player::vehicle->z >= destroyDistance)
				Die();
		}

		void OnCollision(Tema2_GameObject* object)
		{
			if (!activated && object->meshName == "vehicle")
			{
				Activate((Tema2_Player::VehicleObject *)object);
			}
		}

		/*
			Activates the platform and affects the player depending on the platform type
		*/
		void Activate(Tema2_Player::VehicleObject* player)
		{
			activated = true;
			if (meshName == "platform_red")
			{
				if (!player->invulnerable)
					player->Hit(1);
			}
			else if (meshName == "platform_pink")
			{
				player->Hit(-1);
			}
			else if (meshName == "platform_yellow")
			{
				player->SetInfiniteFuel(false);
				if (!player->invulnerable)
					player->AddFuel(-10);
			}
			else if (meshName == "platform_green")
			{
				player->SetInfiniteFuel(false);
				player->AddFuel(20);
			}
			else if (meshName == "platform_orange")
			{
				if (!player->invulnerable)
					player->Deform();
			}
			else if (meshName == "platform_brown")
			{
				if (!player->invulnerable)
					player->SetGear(0);
			}
			else if (meshName == "platform_black")
			{
				player->SetInfiniteFuel(true);
			}
			else if (meshName == "platform_teal")
			{
				player->SetInvulnerable(!player->invulnerable);
			}
			else if (meshName == "platform_blue")
			{
				player->SetInvulnerable(false);
			}
			meshName = "platform_purple";
		}
	};
}