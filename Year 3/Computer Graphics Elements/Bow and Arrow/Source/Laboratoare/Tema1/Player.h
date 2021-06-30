#pragma once
#include "GameObject.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace Player
{
	static int score = 0;
	static int lives = 3;
	static float heartSize = 15;
	static GameObject* heartObjects[3];
	static GameObject* timeBar;
	static bool multiShot = false, timeStopped = false, gameOver = false, gamePaused = false, cheatMode = false;
	static const float cooldownTimeStopMax = 8, lengthTimeStopMax = 5;
	static float cooldownTimeStop = 0, lengthTimeStop = 0;

	/*
		Updates Hidden/Shown hearts based on current lives counter.
	*/
	static void ShowHearts()
	{
		for (int i = 0; i < 3; ++i)
		{
			heartObjects[i]->hidden = (i + 1 > lives);
		}
	}

	/*
		Increases/Decreases Player Health.
	*/
	static void IncreaseHealth(int amount)
	{
		if (amount != 0 && lives > 0)
		{
			lives = min(max(lives + amount, 0), 3);
			ShowHearts();
		}
	}

	/*
		Increases/Decreases Player Score.
	*/
	static void IncreaseScore(int amount)
	{
		if (amount > 0)
		{
			if (score + amount > score)
				score += amount;
		}
		else
		{
			if (score + amount < score)
				score += amount;
		}
		printf("Score: %d\n", score);
	}
}
