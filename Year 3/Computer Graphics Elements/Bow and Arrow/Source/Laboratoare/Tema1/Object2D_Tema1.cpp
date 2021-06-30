#include "Object2D_Tema1.h"

#include <Core/Engine.h>
#include "ColorManager.h"

Mesh* Object2D_Tema1::CreateSquare(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill)
{
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color)
	};

	Mesh* square = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		square->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// draw 2 triangles. Add the remaining 2 indices
		indices.push_back(0);
		indices.push_back(2);
	}

	square->InitFromData(vertices, indices);
	return square;
}

#define DISC_LINES 100

Mesh* Object2D_Tema1::CreateCircle(std::string name, float radius, glm::vec3 color, bool fill)
{

	std::vector<VertexFormat> vertices;
	if (fill)
		vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));

	for (int i = 0; i < DISC_LINES; ++i) {
		float angle = (2 * M_PI / DISC_LINES) * i;
		vertices.push_back(VertexFormat(glm::vec3(radius * cos(angle), radius * sin(angle), 0), color));
	}

	Mesh* circle = new Mesh(name);
	std::vector<unsigned short> indices;

	if (!fill) {
		circle->SetDrawMode(GL_LINE_LOOP);
		for (int i = 0; i < DISC_LINES - 1; ++i) {
			indices.push_back(i);
			indices.push_back(i + 1);
		}
		indices.push_back(DISC_LINES - 1);
		indices.push_back(0);
	}
	else
	{
		for (int i = 1; i < DISC_LINES; ++i) {
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}
		indices.push_back(0);
		indices.push_back(DISC_LINES);
		indices.push_back(1);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}

Mesh* Object2D_Tema1::CreateBow(std::string name, float radius, glm::vec3 color, glm::vec3 colorString, float pullDistance)
{

	std::vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(0, -pullDistance, 0), colorString)
	};

	for (int i = 0; i <= DISC_LINES / 2; ++i) {
		float angle = (2 * M_PI / DISC_LINES) * i;
		if (i == 0 || i == DISC_LINES / 2)
			vertices.push_back(VertexFormat(glm::vec3(radius * cos(angle), radius * sin(angle), 0), colorString));
		else
			vertices.push_back(VertexFormat(glm::vec3(radius * cos(angle), radius * sin(angle), 0), color));
	}

	Mesh* bow = new Mesh(name);
	std::vector<unsigned short> indices;

	bow->SetDrawMode(GL_LINE_LOOP);
	for (int i = 0; i <= DISC_LINES / 2; ++i) {
		indices.push_back(i);
		indices.push_back(i + 1);
	}
	indices.push_back(DISC_LINES / 2 + 1);
	indices.push_back(0);

	bow->InitFromData(vertices, indices);
	return bow;
}

Mesh* Object2D_Tema1::CreateArrow(std::string name, glm::vec3 color, bool fill)
{
	float length = Arrow::arrowLength;
	float height = Arrow::arrowHeight;
	float tip = Arrow::tipLength;

	glm::vec3 corner = glm::vec3(-length / 20, -length / 2, 0);

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, ColorManager::grey),
		VertexFormat(corner + glm::vec3(height, 0, 0), ColorManager::grey),
		VertexFormat(corner + glm::vec3(height, length, 0), ColorManager::grey),
		VertexFormat(corner + glm::vec3(0, length, 0), ColorManager::grey),
		VertexFormat(corner + glm::vec3(-height, length, 0), color),
		VertexFormat(corner + glm::vec3(2 * height, length, 0), color),
		VertexFormat(corner + glm::vec3(height / 2, length + tip, 0), color)
	};

	Mesh* arrow = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2 };

	if (!fill) {
		arrow->SetDrawMode(GL_LINE_LOOP);
		indices.push_back(4);
		indices.push_back(6);
		indices.push_back(5);
		indices.push_back(3);
	}
	else {
		indices.push_back(3);
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(6);
	}

	arrow->InitFromData(vertices, indices);
	return arrow;
}

Mesh* Object2D_Tema1::CreateBalloon(std::string name, float radius, glm::vec3 color, glm::vec3 colorString)
{

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(0, 0, 0), ColorManager::white)
	};

	for (int i = 0; i < DISC_LINES; ++i) {
		float angle = (2 * M_PI / DISC_LINES) * i;
		vertices.push_back(VertexFormat(glm::vec3(radius * cos(angle), radius * sin(angle), 0), color));
	}
	vertices.push_back(VertexFormat(glm::vec3(-0.05 * radius, -radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(0.05 * radius, -radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(-0.05 * radius, -1.07 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(0.05 * radius, -1.07 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(-0.3 * radius, -1.14 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(-0.2 * radius, -1.14 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(0 * radius, -1.36 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(0.1 * radius, -1.36 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(0 * radius, -1.71 * radius, 0), colorString));
	vertices.push_back(VertexFormat(glm::vec3(0.1 * radius, -1.71 * radius, 0), colorString));
	const int segments = 8;

	Mesh* circle = new Mesh(name);
	std::vector<unsigned short> indices;

	for (int i = 1; i < DISC_LINES; ++i) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
	indices.push_back(0);
	indices.push_back(DISC_LINES);
	indices.push_back(1);

	for (int i = 0; i < segments; i += 2) {
		indices.push_back(DISC_LINES + i + 2 );
		indices.push_back(DISC_LINES + i + 1);
		indices.push_back(DISC_LINES + i + 3);
		indices.push_back(DISC_LINES + i + 2);
		indices.push_back(DISC_LINES + i + 3);
		indices.push_back(DISC_LINES + i + 4);
	}

	circle->InitFromData(vertices, indices);
	return circle;
}

Mesh* Object2D_Tema1::CreateHeart(std::string name, float length, glm::vec3 color, bool fill)
{
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(0, 0, 0), color),
		VertexFormat(glm::vec3(-length, length, 0), color),
		VertexFormat(glm::vec3(-2 * length, 0, 0), color),
		VertexFormat(glm::vec3(length, length, 0), color),
		VertexFormat(glm::vec3(2 * length, 0, 0), color),
		VertexFormat(glm::vec3(0, -2 * length, 0), color)
	};

	Mesh* heart = new Mesh(name);
	std::vector<unsigned short> indices;

	if (!fill) {
		heart->SetDrawMode(GL_LINE_LOOP);
		indices = {0, 1, 2, 3, 4, 5};
	}
	else {
		indices = {0, 1, 2, 0, 2, 5, 0, 4, 3, 0, 4, 5};
	}

	heart->InitFromData(vertices, indices);
	return heart;
}

Mesh* Object2D_Tema1::CreateShuriken(std::string name, float length, glm::vec3 color)
{
	glm::vec3 colorInner = ColorManager::grey;
	glm::vec3 colorOutter = color;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(0, 0, 0), color),
		VertexFormat(glm::vec3(length, length, 0), colorInner),
		VertexFormat(glm::vec3(0, 2 * length, 0), colorOutter),
		VertexFormat(glm::vec3(-length, length, 0), colorInner),
		VertexFormat(glm::vec3(-2 * length, 0, 0), colorOutter),
		VertexFormat(glm::vec3(-length, -length, 0), colorInner),
		VertexFormat(glm::vec3(0, -2 * length, 0), colorOutter),
		VertexFormat(glm::vec3(length, -length, 0), colorInner),
		VertexFormat(glm::vec3(2 * length, 0, 0), colorOutter)
	};

	Mesh* heart = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 0, 3, 4, 0, 5, 6, 0, 7, 8};

	heart->InitFromData(vertices, indices);
	return heart;
}