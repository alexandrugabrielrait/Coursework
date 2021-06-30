#pragma once
#include <Component/SimpleScene.h>
#include "VehicleCamera.h"
#include "Tema2_Player.h"
#include "Tema2_Platform.h"
#include "Tema3_Item.h"
#include "Tema3_Decor.h"
#include "Tema2_MeshCreator.h"
#include <TextRenderer/TextRenderer.h>
#include <set>

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void UpdateObject(Tema2_GameObject* object, float deltaTimeSeconds);
	void FrameEnd() override;

	/*
		Renders a mesh in 2D
	*/
	void RenderSimpleMesh2D(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
	/*
		Renders a mesh in 3D
	*/
	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, int parameters, Texture2D* texture1, Texture2D* texture2 = NULL);
	/*
		Adds a new mesh to the list of meshes
	*/
	void AddMesh(Mesh* mesh);
	void AddColoredMesh(Mesh* mesh, glm::vec3 color);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	/*
		Resets the game
	*/
	void StartGame();

	/*
		Spawns a new platform
	*/
	Tema2_GameObject* SpawnPlatform(string meshName, int lane, float positionZ, float lengthZ);
	/*
		Spawns a new item
	*/
	Tema2_GameObject* SpawnItem(string meshName, int lane, float positionZ);
	/*
		Spawns a new decorative object
	*/
	Tema2_GameObject* SpawnDecor(string meshName, int lane, float positionZ);
	/*
		Spawns a table with chairs and a platform
	*/
	void SpawnTable(int lane, float positionZ);
	/*
		Spawns a desk with a chair, a laptop and a platform
	*/
	void SpawnDesk(int lane, float positionZ);
	/*
		Creates a new platform type and gives it a generation weight
	*/
	void AddPlatformType(string meshName, glm::vec3 color, int weight);

	float fov = 60;
	Vehicle::Camera* camera;
	glm::mat4 projectionMatrix;
	list<Tema2_GameObject*> gameObjects, hudElements;
	set<int> latestLanes;
	list<string> platformTypes;
	const int platformLength = 4;
	int latestDistance;
	unsigned int platformIndex = 0, platformItemSpawnIndex = 5;
	bool reverseDirections = true;
	std::unordered_map<std::string, Texture2D*> mapTextures;
	TextRenderer* tr;
};