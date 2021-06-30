#pragma once
#include "Tema2_GameObject.h"
#include <iostream>

namespace Tema2_Player
{
	static const float fuelMax = 100, fuelUsage = 1, gearSpeed = 0.5, deformationTimeMax = 4, deformationDecrement = 0.3, deformationIncrement = 2, displayRatio = 0.95;
	static const float laneLength = 1, jumpHeight = 1.25, platformY = 0.5, gearMin = 0, gearMax = 5, timeJumpMax = 1;
	static const int healthMax = 3;
	static bool gameOver = false;
	static float  longestDistance = 0;
	static int highScore = 0;
	static Tema2_GameObject* fuelBackground, * fuelDisplay, *gearBackground, *gearDisplay, *healthBackground, *healthDisplay;

	/*
		Creates the HUD element and background based on the given mesh name
	*/
	static void InitHudDisplay(Tema2_GameObject* &background, Tema2_GameObject* &display, string meshName)
	{
		background = new Tema2_GameObject("hud_background", "VertexColor");
		display = new Tema2_GameObject(meshName, "VertexColor");
		background->y = display->y = -0.8;
		background->sX /= 2;
		background->sY = background->sX / 2;
		display->sX = background->sX * displayRatio;
		display->sY = background->sY * 0.8;
	}

	/*
		Initializes HUD elements
	*/
	static void InitHUD()
	{
		InitHudDisplay(fuelBackground, fuelDisplay, "fuel_display");
		InitHudDisplay(gearBackground, gearDisplay, "gear_display");
		InitHudDisplay(healthBackground, healthDisplay, "health_display");
		fuelBackground->x = -0.7;
		gearBackground->x = -fuelBackground->x;
	}

	/*
		Updates a HUD element based on the given background and ratio
	*/
	static void UpdateHUD(Tema2_GameObject*& background, Tema2_GameObject*& display, float ratio)
	{
		display->x = background->x - 0.5 * displayRatio * background->sX * (1 - ratio);
		display->sX = background->sX * displayRatio * ratio;
	}

	class VehicleObject : public Tema2_GameObject
	{
	public:
		float fuel = fuelMax, forwardGear, deformation = 0, deformationTime = 0, timeJump = 0;
		int currentLane = 0, direction = 0, health = healthMax, score = 0;
		bool isFalling = false, gearLock = false, isJumping = false, invulnerable, infiniteFuel;

		VehicleObject() : Tema2_GameObject("vehicle", "ComplexShader", 0.25, -0.75)
		{
			sY = sZ = 0.6;
			sX = 1.6 * sZ;
			y = platformY;
			SetGear(gearMin);
			SetInvulnerable(false);
			SetInfiniteFuel(false);
			texture1 = "egg";
			texture2 = "biomechanical";
			parameters = Tema3_par_deformation;
		}

		void OnUpdate(float deltaTimeSconds) override
		{
			if (!infiniteFuel)
				fuel -= forwardGear * fuelUsage * deltaTimeSconds;
			if (gameOver)
				return;
			if (fuel > 0)
				z -= forwardGear * gearSpeed * deltaTimeSconds;
			else
			{
				Hit(1);
				fuel = fuelMax;
			}
			if (deformationTime > 0)
			{
				deformationTime -= deltaTimeSconds;
				deformation = MIN(deformation + deformationIncrement * deltaTimeSconds, 1);
			}
			else
			{
				deformation = MAX(deformation - deformationDecrement * deltaTimeSconds, 0);
			}
			if (deformation == 0)
				RemoveLock();

			if (isJumping)
			{
				timeJump += deltaTimeSconds;
				float mid = timeJumpMax / 2;
				float timeCoord = (1 - timeJump / mid);
				if (timeJumpMax >= timeJump)
					y = platformY + jumpHeight * (1 - timeCoord * timeCoord);
				else
					isJumping = false;
			}
			if (direction)
			{
				x += direction * laneLength * (deltaTimeSconds / timeJumpMax);
				float mid = laneLength / 2;
				float x1 = direction * (x - currentLane * laneLength);
				float x2 = (1 - x1 / mid);
				y = platformY + jumpHeight * (1 - x2 * x2);
				if (direction * (x - currentLane) >= laneLength)
				{
					currentLane += direction;
					direction = 0;
					x = currentLane * laneLength;
					y = platformY;
				}
			}
			else
			{
				if (isFalling)
				{
					vY = -2.5;
					if (y <= -2)
						Die();
				}
				else
					vY = 0;
				isFalling = true;
			}
			UpdateHUD(fuelBackground, fuelDisplay, MAX(fuel, 0) / fuelMax);
			if (gearLock)
			{
				if (deformationTime > 0)
					UpdateHUD(gearBackground, gearDisplay, deformationTime / deformationTimeMax);
				else
				{
					gearDisplay->meshName = "gear_display_recovery";
					UpdateHUD(gearBackground, gearDisplay, 1 - deformation);
				}
			}
			else
				UpdateHUD(gearBackground, gearDisplay, ((float)forwardGear) / (gearMax - gearMin));
			UpdateHUD(healthBackground, healthDisplay, ((float)health) / (healthMax));
		}

		void OnCollision(Tema2_GameObject* object)
		{
			isFalling = false;
		}

		/*
			Adds fuel to the vehicle (can be used to subtract fuel)
		*/
		void AddFuel(float amount)
		{
			fuel = MIN(MAX(fuel + amount, 0), fuelMax);
		}

		/*
			Sets the gear setting and updates the speed
		*/
		void SetGear(int newGear)
		{
			forwardGear = MIN(MAX(newGear, gearMin), gearMax);
			vROX = -4 * (forwardGear / (gearMax - gearMin)) * M_PI;
		}

		/*
			Locks the gear, so it can't be manually changed
		*/
		void LockGear()
		{
			gearLock = true;
			gearDisplay->meshName = "gear_display_locked";
		}

		/*
			Releases the gear lock
		*/
		void RemoveLock()
		{
			gearLock = false;
			gearDisplay->meshName = "gear_display";
		}

		void OnDeath()
		{
			if (gameOver)
				return;
			gameOver = true;
			cout << "GAME OVER" << endl;
			float distance = abs(z);
			highScore = MAX(highScore, score);
			longestDistance = MAX(longestDistance, distance);
			cout << "Score: " << score << " (High Score: " << highScore << ")" << endl;
			cout << "Forward Distance Travelled: " << distance << endl;
			cout << "Longest Distance Travelled: " << longestDistance << endl;
			healthDisplay->sX = 0;
		}

		/*
			Starts deformation, locks the gear and sets the gear to max
		*/
		void Deform()
		{
			deformationTime = max(deformationTime, deformationTimeMax);
			LockGear();
			SetGear(gearMax);
		}

		/*
			Checks if the vehicle can jump
		*/
		bool CanJump()
		{
			return !isFalling && !direction && !isJumping;
		}

		/*
			Makes the vehicle jump vertically
		*/
		void Jump()
		{
			timeJump = 0;
			isJumping = true;
		}

		/*
			Makes the vehicle jump vertically and horizontally towards the new lane
		*/
		void SwitchLane(int newLane)
		{
			if (!direction)
			{
				direction = newLane - currentLane;
			}
		}

		/*
			Damages the vehicle and checks for death
		*/
		void Hit(int amount)
		{
			health -= amount;
			if (health <= 0)
				Die();
			else if (health > healthMax)
				health = healthMax;
		}

		/*
			Returns the largest even integer lesser than the travelled distance
		*/
		int AnalogDistance()
		{
			int distance = floor(abs(z));
			return distance - distance % 2;
		}

		/*
			Sets the vehicle's invulnerability and changes the health display
		*/
		void SetInvulnerable(bool value)
		{
			invulnerable = value;
			if (invulnerable)
				healthDisplay->meshName = "health_display_invulnerable";
			else
				healthDisplay->meshName = "health_display";
		}

		/*
			Sets if infinite fuel is on or off and changes the fuel display
		*/
		void SetInfiniteFuel(bool value)
		{
			infiniteFuel = value;
			if (infiniteFuel)
				fuelDisplay->meshName = "fuel_display_infinite";
			else
				fuelDisplay->meshName = "fuel_display";
		}
	};

	static VehicleObject* vehicle;
}