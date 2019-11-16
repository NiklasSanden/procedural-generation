#include "setup/GameManager.h"
#include "engine/objects/GameObject.h"
#include "engine/misc/Game.h"
#include "engine/resources/ResourceManager.h"
#include "engine/objects/Lights.h"
#include "setup/Camera.h"
#include "objects/Cube.h"
#include "objects/CPUChunks/ChunkManager.h"
#include "objects/MarchingCubesManager.h"
#include "objects/CPUMarchingCubes/CPUMarchingCubesManager.h"
#include "objects/ComputeMarchingCubes/ComputeMCManager.h"
#include "objects/Raytracing/Raytracing.h"
#include "objects/Complete/CompleteMCManager.h"

#include "imgui/imgui.h"

#include <iostream>
#include <vector>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

// static
Engine::Game* GameManager::gamePtr = nullptr;
Input* GameManager::inputPtr = nullptr;
Engine::DirectionalLight* GameManager::directionalLight = nullptr;
std::vector<Engine::PointLight*> GameManager::pointLights;
Camera* GameManager::player = nullptr;

Engine::Game* GameManager::getGamePtr() {
	return gamePtr;
}
void GameManager::setGamePtr(Engine::Game* _gamePtr) {
	gamePtr = _gamePtr;
}

Input* GameManager::getInputPtr() {
	return inputPtr;
}
void GameManager::setInputPtr(Input* _inputPtr) {
	inputPtr = _inputPtr;
}

Engine::DirectionalLight* GameManager::getDirectionalLight() {
	return directionalLight;
}

std::vector<Engine::PointLight*> GameManager::getPointLights() {
	return pointLights;
}

Camera* GameManager::getPlayer() {
	return player;
}

GameManager::GameManager(const std::string& _name) : GameObject(_name) {
	Engine::ResourceManager::loadAllTextures();

	player = new Camera("Camera");
	this->gamePtr->addGameObject(player); // the game class will deallocate the memory

	/*Cube* cube = new Cube("TestCube");
	this->gamePtr->addGameObject(cube);*/
	
	//CPUMarchingCubesManager* cpuCubesManager = new CPUMarchingCubesManager("CPUMarchingCubesManager");
	//this->gamePtr->addGameObject(cpuCubesManager);

	//MarchingCubesManager* cubesManager = new MarchingCubesManager("MarchingCubesManager");
	//this->gamePtr->addGameObject(cubesManager);

	//ComputeMCManager* compCubesManager = new ComputeMCManager("ComputeMCManager");
	//this->gamePtr->addGameObject(compCubesManager);

	//Raytracing* raytracer = new Raytracing("Raytracer");
	//this->gamePtr->addGameObject(raytracer);

	CompleteMCManager* completeMCManager = new CompleteMCManager("CompleteMCManager");
	this->gamePtr->addGameObject(completeMCManager);

	/*ChunkManager* chunkManager = new ChunkManager("ChunkManager");
	this->gamePtr->addGameObject(chunkManager);*/

	// Lights
	//this->directionalLight = new Engine::DirectionalLight("Directional Light", glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	this->directionalLight = new Engine::DirectionalLight("Directional Light", glm::vec3(-0.2f, -0.5f, -0.3f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	this->gamePtr->addGameObject(directionalLight);

	/*this->pointLights.push_back(new Engine::PointLight("Point Light1", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 50.0f));
	this->gamePtr->addGameObject(pointLights.back());*/
}

GameManager::~GameManager() {
	
}

// instance
void GameManager::update(float deltaTime) {
	
}

void GameManager::fixedUpdate() {
	
}

void GameManager::renderImGui() {
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//bool showDemoWindow = true;
	//ImGui::ShowDemoWindow(&showDemoWindow);

	//{
	//	static float f = 0.0f;
	//	static int counter = 0;

	//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//	bool yes = true;
	//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

	//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//		counter++;
	//	ImGui::SameLine();
	//	ImGui::Text("counter = %d", counter);

	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//	ImGui::End();
	//}

	/*ImGui::Begin("Test window");

	ImGui::Text(std::to_string(1.0f / savedDeltaTime).c_str());

	ImGui::End();*/
}
