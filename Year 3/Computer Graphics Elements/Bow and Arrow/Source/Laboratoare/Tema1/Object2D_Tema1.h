#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include "Arrow.h"

// Used to create specific Shapes
namespace Object2D_Tema1
{
	Mesh* CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateCircle(std::string name, float radius, glm::vec3 color, bool fill = false);
	Mesh* CreateBow(std::string name, float radius, glm::vec3 color, glm::vec3 colorString, float pullDistance = 0);
	Mesh* CreateArrow(std::string name, glm::vec3 color, bool fill = false);
	Mesh* CreateBalloon(std::string name, float radius, glm::vec3 color, glm::vec3 colorString);
	Mesh* CreateHeart(std::string name, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateShuriken(std::string name, float length, glm::vec3 color);
}

