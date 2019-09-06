#include "GameManager.h"
#include "engine/Game.h"
#include "engine/ResourceManager.h"
#include "Camera.h"
#include "Cube.h"

#include <iostream>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

// static
Engine::Game* GameManager::gamePtr = nullptr;
Input* GameManager::inputPtr = nullptr;

// GamePtr
Engine::Game* GameManager::getGamePtr() {
	return gamePtr;
}
void GameManager::setGamePtr(Engine::Game* _gamePtr) {
	gamePtr = _gamePtr;
}

// InputPtr
Input* GameManager::getInputPtr() {
	return inputPtr;
}
void GameManager::setInputPtr(Input* _inputPtr) {
	inputPtr = _inputPtr;
}

GameManager::GameManager(std::string& _name) : GameObject(_name) {
	// Load textures
	Engine::ResourceManager::loadAllTextures();

	// Create Objects
	std::string cameraName = "Camera";
	Camera* camera = new Camera(cameraName); // create a camera and add it to the game
	this->gamePtr->addGameObject(camera); // the game class will deallocate the memory

	// add cube
	Cube* cube = new Cube("Cube1");
	this->gamePtr->addGameObject(cube);
}

GameManager::~GameManager() {
	
}

// instance
void GameManager::awake() {
	// setup camera
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
