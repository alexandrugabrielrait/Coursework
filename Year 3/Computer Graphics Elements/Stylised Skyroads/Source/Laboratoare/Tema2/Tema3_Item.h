#pragma once
#include "Tema2_GameObject.h"
#include <iostream>
#include "Tema2_Player.h"

namespace Tema3_Item
{
	static const float destroyDistance = 3;

	class ItemObject : public Tema2_GameObject
	{
	public:
		ItemObject(string meshName, int lane, float positionZ) : Tema2_GameObject(meshName, "ComplexShader", 3, -1)
		{
			sX = sY = sZ = 1 / colliderScale;
			x = lane * Tema2_Player::laneLength;
			y = 0.5;
			z = -positionZ;
			vROY = 2;
			texture1 = meshName;
			parameters = Tema3_par_item;
		}

		void OnUpdate(float deltaTimeSeconds) override
		{
			// if the ball gets too far from the item, the item will be destroyed
			if (z - sZ/2 - Tema2_Player::vehicle->z >= destroyDistance)
				Die();
		}

		void OnCollision(Tema2_GameObject* object)
		{
			if (object->meshName == "vehicle")
			{
				PickUp((Tema2_Player::VehicleObject *)object);
			}
		}

		/*
			Activates the item and affects the player depending on the item type
		*/
		void PickUp(Tema2_Player::VehicleObject* player)
		{
			if (player->isJumping || player->direction)
				return;
			if (meshName == "dna")
			{
				cout << "Score: " << ++player->score << endl;
			}
			else if (meshName == "banana")
			{
				int offset = -1 + rand() % 3;
				if (!offset)
					player->Jump();
				else
					player->SwitchLane(player->currentLane + offset);
			}
			else if (meshName == "tnt")
			{
				player->Hit(1);
			}
			Die();
		}
	};
}