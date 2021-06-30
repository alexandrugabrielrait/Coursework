#pragma once

#include <Core/GPU/Mesh.h>
#include <Core/Engine.h>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Tema2_MeshCreator
{
	/*
		Creates a mesh from vertices and indices
	*/
	static Mesh* CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
	{
		unsigned int VAO = 0;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		unsigned int IBO;
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

		glBindVertexArray(0);

		CheckOpenGLError();

		Mesh* mesh = new Mesh(name);
		mesh->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
		mesh->vertices = vertices;
		mesh->indices = indices;
		return mesh;
	}

	/*
		Creates a cube mesh
	*/
	static Mesh* CreateCube(string meshName, glm::vec3 color)
	{
		std::vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-0.5, -0.5,  0.5), color),
			VertexFormat(glm::vec3(0.5, -0.5,  0.5), color),
			VertexFormat(glm::vec3(-0.5,  0.5,  0.5), color),
			VertexFormat(glm::vec3(0.5,  0.5,  0.5), color),
			VertexFormat(glm::vec3(-0.5, -0.5, -0.5), color),
			VertexFormat(glm::vec3(0.5, -0.5, -0.5), color),
			VertexFormat(glm::vec3(-0.5,  0.5, -0.5), color),
			VertexFormat(glm::vec3(0.5,  0.5, -0.5), color),
		};

		std::vector<unsigned short> indices =
		{
			0, 1, 2,		1, 3, 2,
			2, 3, 7,		2, 7, 6,
			1, 7, 3,		1, 5, 7,
			6, 7, 4,		7, 5, 4,
			0, 4, 1,		1, 4, 5,
			2, 6, 4,		0, 2, 4,
		};

		return CreateMesh(meshName.c_str(), vertices, indices);
	}

	/*
		Creates a 2D square mesh
	*/
	static Mesh* CreateSquare(string meshName, glm::vec3 color)
	{
		std::vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-0.5, -0.5,  0), color),
			VertexFormat(glm::vec3(0.5, -0.5,  0), color),
			VertexFormat(glm::vec3(-0.5,  0.5,  0), color),
			VertexFormat(glm::vec3(0.5,  0.5,  0), color),
		};

		std::vector<unsigned short> indices =
		{
			0, 1, 2, 1, 3, 2
		};

		return CreateMesh(meshName.c_str(), vertices, indices);
	}
}