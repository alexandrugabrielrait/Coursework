#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace Tema2_Player;
using namespace Tema2_Platform;
using namespace Tema2_MeshCreator;
using namespace Tema3_Item;
using namespace Tema3_Decor;

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

	glm::ivec2 resolution = window->GetResolution();
	tr = new TextRenderer(resolution.x, resolution.y);

	tr->Load("Source/TextRenderer/Fonts/ROBOTECH GP.ttf", 100);

	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);

	glm::vec3 orange = glm::vec3(1, 0.5, 0);
	glm::vec3 teal = glm::vec3(0, 0.7, 0.7);
	glm::vec3 white = glm::vec3(1, 1, 1);

	const string textureLoc = "Source/Laboratoare/Tema2/Textures/";
	list<string> textureNames = { "grid", "dirt", "fuel", "plus", "hazard", "leak", "road", "oil", "circuits",
								"egg", "biomechanical", "dna", "wood", "black_metal", "sewage", "lab_chair_wheels",
								"lab_chair_seat", "laptop_screen", "laptop_screen_easter_egg", "banana", "tnt", "walls", "laptop_component" };

	for (string textureName : textureNames)
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + textureName + ".png").c_str(), GL_REPEAT);
		mapTextures[textureName] = texture;
	}

	// UI
	AddMesh(CreateSquare("hud_background", glm::vec3(0.25, 0.25, 0.25)));
	AddMesh(CreateSquare("fuel_display", glm::vec3(0.5, 0.5, 0)));
	AddMesh(CreateSquare("fuel_display_infinite", glm::vec3(0, 0, 0)));
	AddMesh(CreateSquare("gear_display", glm::vec3(0, 0.2, 0.7)));
	AddMesh(CreateSquare("gear_display_locked", orange));
	AddMesh(CreateSquare("gear_display_recovery", glm::vec3(0, 0.8, 0.5)));
	AddMesh(CreateSquare("health_display", glm::vec3(0.9, 0, 0.1)));
	AddMesh(CreateSquare("health_display_invulnerable", teal));

	// platforms
	AddPlatformType("platform_blue", glm::vec3(0, 0, 1), 22);
	AddPlatformType("platform_red", glm::vec3(1, 0, 0), 2);
	AddPlatformType("platform_yellow", glm::vec3(1, 1, 0), 24);
	AddPlatformType("platform_orange", orange, 10);
	AddPlatformType("platform_green", glm::vec3(0, 1, 0), 19);
	AddPlatformType("platform_teal", teal, 1);
	AddPlatformType("platform_pink", glm::vec3(1, 0.5, 0.5), 2);
	AddPlatformType("platform_brown", glm::vec3(0.5, 0.2, 0), 4);
	AddPlatformType("platform_black", glm::vec3(0.1, 0.1, 0.1), 2);
	AddPlatformType("platform_purple", glm::vec3(0.3, 0, 0.5), 0);
	AddPlatformType("platform_brown_purple", glm::vec3(0.3, 0, 0.5), 0);
	AddPlatformType("platform_white", white, 0);

	// items
	AddMesh(CreateQuad("dna"));
	AddMesh(CreateQuad("banana"));
	AddMesh(CreateCube("tnt"));

	// decor
	AddMesh(CreateCylinder("table_top"));
	AddMesh(CreateCylinder("table_support"));
	AddMesh(CreateCylinder("chair_leg"));
	AddMesh(CreateCube("chair_seat"));
	AddMesh(CreateCylinder("lab_chair_seat"));
	AddMesh(CreateCylinder("lab_chair_wheels"));
	AddMesh(CreateCube("laptop_component"));
	AddMesh(CreateQuad("laptop_screen"));
	AddMesh(CreateCube("desk_top"));
	AddMesh(CreateCylinder("pipe", true));
	AddMesh(CreateCube("sewage"));
	AddMesh(CreateCube("walls"));

	{
		Mesh* mesh = new Mesh("vehicle");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	{
		Shader* shader = new Shader("ComplexShader");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
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

	SpawnPlatform("platform_blue", -1, 0, 4);
	SpawnPlatform("platform_blue", 0, 0, 4);
	SpawnPlatform("platform_blue", 1, 0, 4);
	SpawnDecor("walls", 0, 0);
	SpawnDecor("sewage", 0, 0);

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
		modelMatrix = glm::rotate(modelMatrix, object->rOX, glm::vec3(1, 0, 0));
		modelMatrix = glm::rotate(modelMatrix, object->rOY, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, object->rOZ, glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(object->sX, object->sY, object->sZ));
		RenderSimpleMesh(meshes[object->meshName], shaders[object->shaderName], modelMatrix, object->parameters, mapTextures[object->texture1], mapTextures[object->texture2]);
	}
	object->DoCollisions(gameObjects);
}

void Tema2::Update(float deltaTimeSeconds)
{
	if (gameOver)
	{
		deltaTimeSeconds = 0;
		tr->RenderText("High Score: " + to_string(highScore), 1000, 5.0f, 0.4f, glm::vec3(1.0, 0.0, 0.0));
	}
	else
	{
		camera->MoveToVehicle();
		tr->RenderText("Score: " + to_string(vehicle->score), 1080, 5.0f, 0.4f, glm::vec3(0.0, 1.0, 0.0));
	}
	if (latestDistance + vehicle->z <= platformLength / 2)
	{
		latestDistance += platformLength;
		if (latestDistance % 8 == 0 && rand() % 3 != 0)
		{
			int side = -1 + 2 * (rand() % 2);
			switch (rand() % 4)
			{
			case 0:
				SpawnTable(vehicle->currentLane + side * (6 + rand() % 2), latestDistance + 2);
				break;
			case 1:
				SpawnDesk(vehicle->currentLane + side * (6 + rand() % 2), latestDistance + 2);
				break;
			default:
				SpawnDecor("pipe", vehicle->currentLane + side * (8 + rand() % 2), latestDistance + 2);
				break;
			}
		}
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
						SpawnPlatform(meshName, l, latestDistance + 1, platformLength - 1);
					else
						SpawnPlatform(meshName, l, latestDistance, platformLength);
					if (platformIndex++ % platformItemSpawnIndex == 0)
						SpawnItem("dna", l, latestDistance + (rand() % 10) / 10 + 2);
					else if (rand() % 20 < vehicle->score)
					{
						switch (rand() % 6)
						{
						case 0:
						case 1:
							SpawnItem("banana", l, latestDistance + (rand() % 10) / 10 + 2);
							break;
						case 2:
							SpawnItem("tnt", l, latestDistance + (rand() % 10) / 10 + 2);
							break;
						default:
							break;
						}
					}
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

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int parameters, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform1f(glGetUniformLocation(shader->program, "elapsed_time"), Engine::GetElapsedTime());

	glUniform1i(glGetUniformLocation(shader->program, "parameters"), parameters);

	glUniform1f(glGetUniformLocation(shader->program, "deformation"), vehicle->deformation);

	if (mapColors.find(mesh->GetMeshID()) != mapColors.end())
	{
		glm::vec3 color = mapColors[mesh->GetMeshID()];
		glUniform4f(glGetUniformLocation(shader->program, "color"), color.r, color.g, color.b, 1);
	}
	else
		glUniform4f(glGetUniformLocation(shader->program, "color"), 0, 0, 0, 0);

	if (texture1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
	}

	if (!texture2)
		texture2 = texture1;

	if (texture2)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
	}

	// Draw the object
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
				vehicle->SwitchLane(vehicle->currentLane - 1 + 2 * (camera->cameraPerson == 2 && reverseDirections));
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
			cout << 1 << endl;
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

void Tema2::AddColoredMesh(Mesh* mesh, glm::vec3 color)
{
	AddMesh(mesh);
	mapColors[mesh->GetMeshID()] = color;
}

Tema2_GameObject* Tema2::SpawnPlatform(string meshName, int lane, float positionZ, float lengthZ)
{
	Tema2_GameObject* object = new PlatformObject(meshName, lane, positionZ, lengthZ);
	gameObjects.push_back(object);
	return object;
}

Tema2_GameObject* Tema2::SpawnItem(string meshName, int lane, float positionZ)
{
	Tema2_GameObject* object = new ItemObject(meshName, lane, positionZ);
	gameObjects.push_back(object);
	return object;
}

Tema2_GameObject* Tema2::SpawnDecor(string meshName, int lane, float positionZ)
{
	Tema2_GameObject* object = new DecorObject(meshName, lane, positionZ);
	gameObjects.push_back(object);
	return object;
}

void Tema2::SpawnTable(int lane, float positionZ)
{
	int size = 3;
	SpawnPlatform("platform_white", lane, positionZ - size / 2, size);
	SpawnDecor("table_top", lane, positionZ);
	SpawnDecor("table_support", lane, positionZ);
	const float chairDistance = 0.75, legDistance = 0.1;
	for (int direction = -1; direction <= 1; direction += 2)
	{
		for (int i = -1; i <= 1; i += 2)
			for (int j = -1; j <= 1; j += 2)
			{
				Tema2_GameObject* leg = SpawnDecor("chair_leg", lane, positionZ + direction * chairDistance);
				leg->x += i * legDistance;
				leg->z += j * legDistance;
			}
		SpawnDecor("chair_seat", lane, positionZ + direction * chairDistance);
		Tema2_GameObject* back = SpawnDecor("chair_seat", lane, positionZ + direction * (chairDistance + legDistance));
		back->rOX = M_PI / 2;
		back->y += 0.1;
	}
}

void Tema2::SpawnDesk(int lane, float positionZ)
{
	int size = 2;
	SpawnPlatform("platform_white", lane, positionZ - size / 2, size);
	SpawnDecor("desk_top", lane, positionZ);
	SpawnDecor("laptop_component", lane, positionZ);
	Tema2_GameObject* object = SpawnDecor("laptop_component", lane, positionZ + 0.18);
	object->rOX = -M_PI / 1.5;
	object->y += 0.1;
	Tema2_GameObject* screen = SpawnDecor("laptop_screen", lane, positionZ + 0.17);
	screen->rOX = M_PI / 2 + object->rOX;
	screen->y = object->y;
	const float chairDistance = 0.5, legDistance = 0.2;
	for (int i = -1; i <= 1; i += 2)
		for (int j = -1; j <= 1; j += 2)
		{
			Tema2_GameObject* leg = SpawnDecor("chair_leg", lane, positionZ);
			leg->x += i * legDistance;
			leg->z += j * legDistance;
			leg->sY = 0.35;
			leg->y = leg->sY * 0.9;
		}

	SpawnDecor("chair_leg", lane, positionZ - chairDistance);
	SpawnDecor("lab_chair_wheels", lane, positionZ - chairDistance);
	SpawnDecor("lab_chair_seat", lane, positionZ - chairDistance);
	object = SpawnDecor("lab_chair_seat", lane, positionZ - chairDistance - 0.1);
	object->rOX = M_PI / 2;
	object->sZ *= 1.5;
	object->y += 0.2;
}

void Tema2::AddPlatformType(string meshName, glm::vec3 color, int weight)
{
	if (meshName == "platform_brown" || meshName == "platform_brown_purple")
		AddColoredMesh(CreateCylinder(meshName), color);
	else
		AddColoredMesh(CreateCube(meshName), color);
	for (int i = 0; i < weight; ++i)
		platformTypes.push_back(meshName);
}
