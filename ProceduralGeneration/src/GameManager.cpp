#include "GameManager.h"
#include "engine/GameObject.h"
#include "engine/Game.h"
#include "engine/ResourceManager.h"
#include "engine/Lights.h"
#include "Camera.h"
#include "Cube.h"
#include "Chunk.h"

#include "imgui/imgui.h"

#include <iostream>
#include <vector>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

// static
Engine::Game* GameManager::gamePtr = nullptr;
Input* GameManager::inputPtr = nullptr;
Engine::DirectionalLight* GameManager::directionalLight = nullptr;
std::vector<Engine::PointLight*> GameManager::pointLights;

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

GameManager::GameManager(const std::string& _name) : GameObject(_name) {
	Engine::ResourceManager::loadAllTextures();

	Camera* camera = new Camera("Camera");
	this->gamePtr->addGameObject(camera); // the game class will deallocate the memory

	Cube* cube = new Cube("Cube1");
	this->gamePtr->addGameObject(cube);
	
	this->directionalLight = new Engine::DirectionalLight("Directional Light", glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	this->gamePtr->addGameObject(directionalLight);

	this->pointLights.push_back(new Engine::PointLight("Point Light1", glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 50.0f));
	this->gamePtr->addGameObject(pointLights.back());

	/*Chunk* chunk = new Chunk("Chunk1");
	this->gamePtr->addGameObject(chunk);*/
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
	/*bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);*/

	ImGui::Begin("Test window");

	ImGui::End();
}
