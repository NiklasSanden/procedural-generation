#include "GameManager.h"
#include "engine/GameObject.h"
#include "engine/Game.h"
#include "engine/ResourceManager.h"
#include "Camera.h"
#include "Cube.h"

#include "imgui/imgui.h"

#include <iostream>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

// static
Engine::Game* GameManager::gamePtr = nullptr;
Input* GameManager::inputPtr = nullptr;

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

GameManager::GameManager(std::string& _name) : GameObject(_name) {
	Engine::ResourceManager::loadAllTextures();

	std::string cameraName = "Camera";
	Camera* camera = new Camera(cameraName);
	this->gamePtr->addGameObject(camera); // the game class will deallocate the memory

	std::string cubeName = "Cube1";
	Cube* cube = new Cube(cubeName);
	this->gamePtr->addGameObject(cube);
}

GameManager::~GameManager() {
	
}

// instance
void GameManager::awake() {

}

void GameManager::update(float deltaTime) {

}

void GameManager::fixedUpdate() {
	
}

void GameManager::renderImGui() {
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	bool showDemoWindow = true;
	ImGui::ShowDemoWindow(&showDemoWindow);
}
