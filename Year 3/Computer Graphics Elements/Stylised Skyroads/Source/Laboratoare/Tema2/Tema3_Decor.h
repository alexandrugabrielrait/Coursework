#pragma once
#include "Tema2_GameObject.h"
#include <iostream>
#include "Tema2_Player.h"

namespace Tema3_Decor
{
	static const float destroyDistance = 3;

	class DecorObject : public Tema2_GameObject
	{
	public:
		DecorObject(string meshName, int lane, float positionZ) : Tema2_GameObject(meshName, "ComplexShader", 0)
		{
			texture1 = meshName;
			if (meshName == "table_top")
			{
				sX = sZ = 1;
				sY = 0.06;
				y = 0.5;
				texture1 = "black_metal";
			}
			else if (meshName == "desk_top")
			{
				sX = sZ = 0.6;
				sY = 0.06;
				y = 0.5;
				texture1 = "black_metal";
			}
			else if (meshName == "table_support")
			{
				sX = sZ = 0.25;
				sY = 0.35;
				y = sY * 0.9;
				texture1 = "black_metal";
			}
			else if (meshName == "chair_seat")
			{
				sX = sZ = 0.25;
				sY = 0.04;
				y = 0.25 * 1.5;
				texture1 = "wood";
			}
			else if (meshName == "chair_leg")
			{
				sX = sZ = 0.04;
				sY = 0.25;
				y = sY * 0.9;
				texture1 = "black_metal";
			}
			else if (meshName == "lab_chair_seat")
			{
				sX = sZ = 0.25;
				sY = 0.04;
				y = 0.25 * 1.5;
			}
			else if (meshName == "lab_chair_wheels")
			{
				sX = sZ = 0.4;
				sY = 0.01;
				y = 0.15;
			}
			else if (meshName == "laptop_component")
			{
				sX = 0.4;
				sY = 0.01;
				sZ = 0.25;
				y = 0.535;
			}
			else if (meshName == "laptop_screen")
			{
				sX = 0.3;
				sY = sX * 9 / 16;
				if (rand() % 4 == 0)
					texture1 = "laptop_screen_easter_egg";
			}
			else if (meshName == "pipe")
			{
				sX = sZ = 2;
				sY = 5;
				if (lane < Tema2_Player::vehicle->currentLane)
					rOZ = -M_PI / 2;
				else
					rOZ = M_PI / 2;
				texture1 = "black_metal";
			}
			else if (meshName == "walls")
			{
				sX = sY = sZ = 50;
			}
			else if (meshName == "sewage")
			{
				sX = sZ = 49;
				sY = 4;
				y = -2.25;
			}
			x = lane * Tema2_Player::laneLength;
			z = -positionZ;
		}

		void OnUpdate(float deltaTimeSeconds) override
		{
			if (meshName == "sewage" || meshName == "walls")
			{
				x = Tema2_Player::vehicle->x;
				z = Tema2_Player::vehicle->z;
			}
			// if the ball gets too far from the object, the object will be destroyed
			else if (z - sZ/2 - Tema2_Player::vehicle->z >= destroyDistance)
				Die();
		}
	};
}