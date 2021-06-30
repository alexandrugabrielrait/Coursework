#pragma once
#include "GameObject.h"
#include "Player.h"

namespace Balloon
{
	static const float balloonLength = 30, riseVelocity = 50, fallVelocity = 300;
	enum class balloon_type {
		red,
		yellow,
		green,
		blue
	};

	class BalloonObject : public GameObject
	{
		balloon_type type;
		bool popped = false;

	public:
		BalloonObject(balloon_type type, string shaderName) : GameObject("balloon", shaderName, balloonLength)
		{
			this->type = type;
			switch (type)
			{
			case Balloon::balloon_type::yellow:
				meshName = "balloon_yellow";
				break;
			case Balloon::balloon_type::green:
				meshName = "balloon_green";
				break;
			case Balloon::balloon_type::blue:
				meshName = "balloon_blue";
				break;
			default:
				meshName = "balloon";
				break;
			}
			vY = riseVelocity;
			sY *= 1.4;
		}

		void OnCollision(GameObject* object) override
		{
			if (Arrow::ArrowObject* arrow = dynamic_cast<Arrow::ArrowObject*>(object))
			{
				Pop();
			}
		}

		/*
			Called when the ballon is hit by an arrow.
		*/
		void Pop()
		{
			if (!popped)
			{
				popped = true;
				vY = -fallVelocity;
				sX *= 0.25;
				sY *= 0.8;
				switch (type)
				{
				case Balloon::balloon_type::red:
					Player::IncreaseScore(10);
					break;
				case Balloon::balloon_type::yellow:
					Player::IncreaseScore(-10);
					break;
				case Balloon::balloon_type::green:
					Player::IncreaseHealth(1);
					break;
				case Balloon::balloon_type::blue:
					Player::multiShot = true;
					break;
				default:
					break;
				}
			}
		}
	};
}
