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
	static Mesh* CreateCube(string meshName)
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(-0.5, 0.5, 0.5),
			glm::vec3(-0.5, 0.5, -0.5),
			glm::vec3(0.5, 0.5, -0.5),
			glm::vec3(0.5, 0.5, 0.5),

			glm::vec3(-0.5, -0.5, 0.5),
			glm::vec3(-0.5, -0.5, -0.5),
			glm::vec3(0.5, -0.5, -0.5),
			glm::vec3(0.5, -0.5, 0.5),
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, -1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, -1),
			glm::vec3(-1, 0, 0),
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),

			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
		};

		vector<unsigned short> indices =
		{
			0, 3, 1,
			1, 3, 2,
			4, 7, 5,
			5, 7, 6,

			0, 5, 1,
			0, 4, 5,

			1, 6, 2,
			1, 5, 6,

			2, 7, 3,
			2, 6, 7,

			3, 4, 0,
			3, 7, 4,
		};

		Mesh* mesh = new Mesh(meshName);
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		return mesh;
	}

	/*
		Creates a textured square mesh
	*/
	static Mesh* CreateQuad(string meshName)
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(-0.5, 0.5, 0),
			glm::vec3(0.5, 0.5, 0),
			glm::vec3(-0.5, -0.5, 0),
			glm::vec3(0.5, -0.5, 0),
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, -1, 0),
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
		};

		vector<unsigned short> indices =
		{
			0, 1, 3,
			0, 3, 2,
		};

		Mesh* mesh = new Mesh(meshName);
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		return mesh;
	}

	static const int discLines = round(10 * M_PI);

	/*
		Creates a cylinder mesh
	*/
	static Mesh* CreateCylinder(string meshName, bool open = false)
	{
		vector<glm::vec3> vertices;
		for (int h = 1; h >= -1; h -= 2) {
			vertices.push_back(glm::vec3(0, h * 0.5, 0));
			for (int i = 0; i < discLines; ++i) {
				float angle = (2 * M_PI / discLines) * i;
				vertices.push_back(glm::vec3(0.5 * cos(angle), h * 0.5, 0.5 * sin(angle)));
			}
		}

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 0),
			glm::vec3(0, -1, 0),
		};

		vector<glm::vec2> textureCoords;
		for (int h = 1; h >= -1; h -= 2) {
			textureCoords.push_back(glm::vec2(0.5, 0.5));
			for (int i = 0; i < discLines; ++i) {
				float angle = (2 * M_PI / discLines) * i;
				textureCoords.push_back(glm::vec2(0.5 + 0.5 * cos(angle), 0.5 + 0.5 * sin(angle)));
			}
		}

		vector<unsigned short> indices;
		for (int h = open; h <= 1; h++) {
			for (int i = 0; i < discLines; ++i) {
				indices.push_back(h * (discLines + 1));
				indices.push_back(h * (discLines + 1) + i);
				indices.push_back(h * (discLines + 1) + i + 1);
			}
			indices.push_back(h * (discLines + 1));
			indices.push_back(h * (discLines + 1) + discLines);
			indices.push_back(h * (discLines + 1) + 1);
		}
		for (int i = 1; i < discLines; ++i) {
			indices.push_back(i + discLines + 1);
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + discLines + 1);
			indices.push_back(i + 1);
			indices.push_back(i + discLines + 2);
		}
		indices.push_back(2 * discLines + 1);
		indices.push_back(discLines);
		indices.push_back(1);
		indices.push_back(2 * discLines + 1);
		indices.push_back(1);
		indices.push_back(discLines + 2);

		Mesh* mesh = new Mesh(meshName);
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		return mesh;
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