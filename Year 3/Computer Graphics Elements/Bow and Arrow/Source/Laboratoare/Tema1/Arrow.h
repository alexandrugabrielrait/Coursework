#pragma once
#include "GameObject.h"
#include <Core/GPU/Mesh.h>
#include "Player.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Arrow
{
	static const float arrowLength = 80, arrowHeight = 0.1 * arrowLength, tipLength = 0.5 * arrowLength;
	static const float arrowSpeedMin = 200, arrowSpeedMax = 500, arrowSpeedIncrement = 175;

	class ArrowObject : public GameObject
	{
	public:
		Mesh* arrowMesh;

		ArrowObject(Mesh* arrowMesh, string shaderName) : GameObject("arrow", shaderName, tipLength * 0.001, 0, arrowLength)
		{
			this->arrowMesh = arrowMesh;
		}

		void OnCollision(GameObject* object) override
		{
			// When colliding with a shuriken, the shuriken gets deactivated and the player gains 5 score.
			if (object->meshName == "shuriken" || object->meshName == "shuriken_reflective")
			{
				// If the shuriken is reflective, the arrow's direction is reversed
				if (object->meshName == "shuriken_reflective")
				{
					vX *= -1;
					vY *= -1;
					r += M_PI;
				}
				object->meshName = "shuriken_inactive";
				object->vR = 0;
				object->vX = 0;
				object->vY = -200;
				Player::IncreaseScore(5);
			}
		}
	};
}