#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace Tema2_Player;
using namespace Tema2_Platform;
using namespace Tema2_MeshCreator;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	camera = new Vehicle::Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

	glm::vec3 orange = glm::vec3(1, 0.5, 0);
	glm::vec3 teal = glm::vec3(0, 0.7, 0.7);

	AddMesh(CreateSquare("hud_background", glm::vec3(0.25, 0.25, 0.25)));
	AddMesh(CreateSquare("fuel_display", glm::vec3(0.5, 0.5, 0)));
	AddMesh(CreateSquare("fuel_display_infinite", glm::vec3(0, 0, 0)));
	AddMesh(CreateSquare("gear_display", glm::vec3(0, 0.2, 0.7)));
	AddMesh(CreateSquare("gear_display_locked", orange));
	AddMesh(CreateSquare("gear_display_recovery", glm::vec3(0, 0.8, 0.5)));
	AddMesh(CreateSquare("health_display", glm::vec3(0.9, 0, 0.1)));
	AddMesh(CreateSquare("health_display_invulnerable", teal));

	AddPlatformType("platform_blue", glm::vec3(0, 0, 0.5), 22);
	AddPlatformType("platform_red", glm::vec3(1, 0, 0), 3);
	AddPlatformType("platform_yellow", glm::vec3(1, 1, 0), 24);
	AddPlatformType("platform_orange", orange, 10);
	AddPlatformType("platform_green", glm::vec3(0, 1, 0), 19);
	AddPlatformType("platform_teal", teal, 1);
	AddPlatformType("platform_pink", glm::vec3(1, 0.5, 0.5), 1);
	AddPlatformType("platform_brown", glm::vec3(0.5, 0.2, 0), 4);
	AddPlatformType("platform_black", glm::vec3(0.1, 0.1, 0.1), 2);
	AddMesh(CreateCube("platform_purple", glm::vec3(0.25, 0, 0.3)));

	{
		Mesh* mesh = new Mesh("vehicle");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Shader* shader = new Shader("VehicleShader");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VehicleVS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VehicleFS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	InitHUD();

	hudElements.push_back(fuelDisplay);
	hudElements.push_back(fuelBackground);
	hudElements.push_back(gearDisplay);
	hudElements.push_back(gearBackground);
	hudElements.push_back(healthDisplay);
	hudElements.push_back(healthBackground);

	StartGame();
}


void Tema2::StartGame()
{
	while (!gameObjects.empty())
	{
		Tema2_GameObject* object = gameObjects.back();
		gameObjects.pop_back();
	}
	vehicle = new VehicleObject();
	gameObjects.push_back(vehicle);

	AddPlatform("platform_white", -1, 0, 4);
	AddPlatform("platform_white", 0, 0, 4);
	AddPlatform("platform_white", 1, 0, 4);

	latestDistance = 0;
	latestLanes = { -1, 0, 1 };
	gameOver = false;
}

void Tema2::FrameStart()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::UpdateObject(Tema2_GameObject* object, float deltaTimeSeconds)
{
	object->Update(deltaTimeSeconds);
	if (!object->hidden)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(object->x, object->y, object->z));
		modelMatrix = glm::rotate(modelMatrix, object->r, glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(object->sX, object->sY, object->sZ));
		RenderSimpleMesh(meshes[object->meshName], shaders[object->shaderName], modelMatrix);
	}
	object->DoCollisions(gameObjects);
}

void Tema2::Update(float deltaTimeSeconds)
{
	if (gameOver)
		deltaTimeSeconds = 0;
	else
		camera->MoveToVehicle();
	if (latestDistance + vehicle->z <= platformLength / 2)
	{
		latestDistance += platformLength;
		set<int> oldLanes = latestLanes, allLanes;
		latestLanes.clear();
		for (int l : oldLanes)
		{
			if (abs(l - vehicle->currentLane) > 3 || l == MAXINT32 || l == MININT32)
				continue;
			list<int> choiceLanes;
			for (int i = -1; i <= 1; ++i)
			{
				if (allLanes.find(l + i) == allLanes.end())
				{
					allLanes.insert(l + i);
					choiceLanes.push_back(l + i);
				}
			}
			int numberOfLanes;
			if (oldLanes.size() <= 2)
				numberOfLanes = 4 - oldLanes.size();
			else if (rand() % 10 == 0)
			{
				if (rand() % 100 == 0)
				{
					numberOfLanes = 3;
				}
				else
					numberOfLanes = 2;
			}
			else
				numberOfLanes = 1;
			numberOfLanes = MIN(numberOfLanes, choiceLanes.size());
			for (int i = 0; i < numberOfLanes; ++i)
			{
				int choiceIndex = rand() % choiceLanes.size(), chosenLane;
				for (auto it = choiceLanes.begin(); it != choiceLanes.end(); ++it)
				{
					if (choiceIndex == 0)
					{
						chosenLane = *it;
						choiceLanes.erase(it);
						break;
					}
					--choiceIndex;
				}
				latestLanes.insert(chosenLane);
			}
		}
		for (int l : latestLanes)
		{
			int chosenPlatform = rand() % platformTypes.size();
			for (string meshName : platformTypes)
			{
				if (chosenPlatform == 0)
				{
					if (rand() % 3 == 0)
						AddPlatform(meshName, l, latestDistance + 1, platformLength - 1);
					else
						AddPlatform(meshName, l, latestDistance, platformLength);
					break;
				}
				--chosenPlatform;
			}
		}
	}

	for (Tema2_GameObject* object : gameObjects)
	{
		UpdateObject(object, deltaTimeSeconds);
	}

	for (Tema2_GameObject* object : hudElements)
	{
		glm::mat4 modelMatrix = glm::mat3(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(object->x, object->y, object->z));
		modelMatrix = glm::rotate(modelMatrix, object->r, glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(object->sX, object->sY, 0));
		RenderSimpleMesh2D(meshes[object->meshName], shaders[object->shaderName], modelMatrix);
	}

	for (auto it = gameObjects.begin(); it != gameObjects.end();)
	{
		Tema2_GameObject* object = *it;
		if (!object->alive)
		{
			delete(object);
			it = gameObjects.erase(it);
			continue;
		}
		else
			++it;
	}
}

void Tema2::FrameEnd()
{
}

void Tema2::RenderSimpleMesh2D(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	int location = glGetUniformLocation(shader->program, "Model");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	location = glGetUniformLocation(shader->program, "View");

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	location = glGetUniformLocation(shader->program, "Projection");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	glUseProgram(shader->program);

	int location = glGetUniformLocation(shader->program, "Model");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	location = glGetUniformLocation(shader->program, "View");

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	location = glGetUniformLocation(shader->program, "Projection");

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	location = glGetUniformLocation(shader->program, "deformation");

	glUniform1f(location, vehicle->deformation);

	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only in free camera mode
	if (camera->cameraPerson == 4 && window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			camera->MoveForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			camera->MoveForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		if (!vehicle->gearLock)
		{
			if (key == GLFW_KEY_W)
			{
				vehicle->SetGear(vehicle->forwardGear + 1);
			}
			else if (key == GLFW_KEY_S)
			{
				vehicle->SetGear(vehicle->forwardGear - 1);
			}
		}
		if (vehicle->CanJump())
		{
			if (key == GLFW_KEY_A)
			{
				vehicle->SwitchLane(vehicle->currentLane - 1 + 2 *(camera->cameraPerson == 2 && reverseDirections));
			}
			else if (key == GLFW_KEY_D)
			{
				vehicle->SwitchLane(vehicle->currentLane + 1 - 2 * (camera->cameraPerson == 2 && reverseDirections));
			}
			else if (key == GLFW_KEY_SPACE)
			{
				vehicle->Jump();
			}
		}
		if (key == GLFW_KEY_1)
		{
			camera->cameraPerson = 1;
		}
		else if (key == GLFW_KEY_2)
		{
			camera->cameraPerson = 2;
		}
		else if (key == GLFW_KEY_3)
		{
			camera->cameraPerson = 3;
		}
		else if (key == GLFW_KEY_4)
		{
			// free camera mode
			camera->cameraPerson = 4;
		}
		else if (key == GLFW_KEY_R)
		{
			if (!gameOver)
				vehicle->Die();
			StartGame();
		}
		else if (key == GLFW_KEY_Y)
		{
			reverseDirections = !reverseDirections;
			if (reverseDirections)
				cout << "Second person directions are now reversed." << endl;
			else
				cout << "Second person directions are no longer reversed." << endl;
		}
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.002f;
		float sensivityOY = 0.002f;

		camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
		camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
	}
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}

void Tema2::AddMesh(Mesh* mesh)
{
	meshes[mesh->GetMeshID()] = mesh;
}

void Tema2::AddPlatform(string meshName, int lane, float positionZ, float lengthZ)
{
	gameObjects.push_back(new PlatformObject(meshName, lane, positionZ, lengthZ));
}

void Tema2::AddPlatformType(string meshName, glm::vec3 color, int weight)
{
	AddMesh(CreateCube(meshName, color));
	for (int i = 0; i < weight; ++i)
		platformTypes.push_back(meshName);
}
