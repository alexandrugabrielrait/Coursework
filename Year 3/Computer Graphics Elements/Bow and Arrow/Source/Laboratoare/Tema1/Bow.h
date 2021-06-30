#pragma once
#include "GameObject.h"
#include "Arrow.h"

namespace Bow
{
	static Arrow::ArrowObject* ammoDisplay[3];
	static GameObject* powerBar;
	static float cooldownMax = 1, cooldown = 0, additionalSpeed = 0;

	class BowObject : public GameObject
	{
	public:
		BowObject(string shaderName) : GameObject("bow", shaderName, Arrow::arrowLength)
		{
		}

		void OnCollision(GameObject* object) override
		{
			// When colliding with a shuriken, the shuriken gets deactivated and the player loses 1 heart.
			if (!hidden)
				if (object->meshName == "shuriken" || object->meshName == "shuriken_reflective")
				{
					object->meshName = "shuriken_inactive";
					object->vR = 0;
					object->vX = 0;
					object->vY = -200;
					Player::IncreaseHealth(-1);
				}
		}

		void OnUpdate(float deltaTimeSeconds)
		{
			// Checks if player is dead
			if (Player::lives <= 0 && !hidden)
			{
				Player::gameOver = true;
				Player::timeBar->hidden = true;
				additionalSpeed = 0;
				cout << "GAME OVER" << endl;
				cout << "Press R to restart level." << endl;
				hidden = true;
			}
		}
	};

	static BowObject* bow;
}
