#pragma once
#include <list>
#include <map>
#include <string>
#include "Player.h"

namespace LevelManager
{
	static std::map<int, std::list<std::string>> spawnData;
	static int lastSpawn;
	static int currentLevel = 0, levelMax = 6;

	/*
		Fills the spawn data map with Level data. Also prints level specific hints.
	*/
	static void LoadLevel(int level)
	{
		if (level < 1 || level > levelMax)
			return;
		currentLevel = level;

		cout << "Loading level " << currentLevel << endl;

		spawnData.clear();
		
		switch (currentLevel)
		{
		case 2:
			cout << "Avoid hitting yellow balloons!" << endl;
			spawnData[0] = { "br", "br" };
			spawnData[2] = { "by" };
			spawnData[4] = { "by", "br" };
			break;
		case 3:
			cout << "Hit or dodge shuriken!" << endl;
			spawnData[0] = { "sh" , "sh", "sh" };
			spawnData[3] = { "sh" , "sh" };
			spawnData[6] = { "sh" , "sh", "sh" };
			spawnData[9] = { "sh" , "sh" };
			break;
		case 4:
			cout << "Press T to stop or resume time! Time Stop has a limit of " << Player::lengthTimeStopMax << " seconds." << endl;
			cout << "Hit Green Balloons to regain health!" << endl;
			spawnData[0] = { "sh" , "sh", "sh", "sh" , "sh", "sh", "sh" , "sh", "sh",};
			spawnData[1] = { "sh" , "sh", "sh", "sh" , "sh", "sh", "sh" , "sh", "sh", };
			spawnData[3] = { "by" , "bg", "bg", "bg"};
			spawnData[10] = { "sh" , "sh", "sh", "sh" , "sh", "sh", "sh" , "sh", "sh", };
			spawnData[11] = { "sh" , "sh", "sh", "sh" , "sh", "sh", "sh" , "sh", "sh", };
			break;
		case 5:
			cout << "Reflective Shuriken will redirect your arrow on contact!" << endl;
			cout << "Redirected arrows are not harmful to you." << endl;
			spawnData[0] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh"};
			spawnData[1] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh" };
			spawnData[3] = { "bg" , "br", "br", "br" };
			spawnData[5] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg", "bg" };
			spawnData[6] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh" };
			break;
		case 6:
			cout << "Hit blue balloons to gain Multi-Shot.!" << endl;
			cout << "Multi-Shot is lost after 1 use." << endl;
			spawnData[0] = { "bb", "bb" };
			spawnData[2] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg", "bb" };
			spawnData[4] = { "bb", "bb", "bb", "bg" };
			spawnData[6] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg" , "bb"};
			spawnData[10] = { "bb", "bb" };
			spawnData[12] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg", "bb" };
			spawnData[14] = { "bb", "bb", "bb", "bg" };
			spawnData[16] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg" , "bb" };
			spawnData[20] = { "bb", "bb" };
			spawnData[22] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg", "bb" };
			spawnData[24] = { "bb", "bb", "bb", "by" };
			spawnData[26] = { "sr" , "sr", "sr", "sh" , "sh", "sh", "sh" , "sh", "sh", "sh", "sh", "bg" , "bb" };
			spawnData[30] = { "br", "by", "bg", "bb", "sh", "sr"};
			break;
		default:
			cout << "Hit red balloons!" << endl;
			cout << "Press W/S to move. Hold and release mouse to shoot arrows." << endl;
			cout << "Hold mouse for a longer period to increase arrow speed!" << endl;
			cout << "Press P to pause. Press R to restart." << endl;
			spawnData[0] = { "br" };
			spawnData[1] = { "br" };
			break;
		}
		lastSpawn = spawnData.rbegin()->first;
	}
}