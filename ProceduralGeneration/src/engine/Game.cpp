#include "Game.h"
#include "Input.h"
#include "ShaderManager.h"
#include "ResourceManager.h"

#include <iostream>
#include <string>

using namespace Engine;

Game::Game(Program* _program, InputBase* input) {
	this->program = _program;
	this->inputPtr = input;

	// Compile shaders
	ShaderManager::setupShaderSource();
}

Game::~Game() {
	// Cleanup shaders
	ShaderManager::cleanup();
	// Cleanup textures
	ResourceManager::cleanup();

	// release memory
	for (int i = 0; i < this->gameObjects.size(); i++) {
		std::cout << "Deleting gameObject: " << this->gameObjects[i]->name << std::endl;
		delete this->gameObjects[i];
	}
	this->gameObjects.clear();
}

void Game::addGameObject(GameObject* gameObject) {
	std::cout << "Adding gameObject: " << gameObject->name << std::endl;
	this->gameObjects.push_back(gameObject);
	gameObject->awake();
}

void Game::deleteGameObject(std::string gameObjectName) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (this->gameObjects[i]->name == gameObjectName) {
			std::cout << "Deleting gameObject: " << this->gameObjects[i]->name << std::endl,
	
			// release memory
			delete this->gameObjects[i];

			// erase
			this->gameObjects.erase(this->gameObjects.begin() + i);
		}
	}
}

GameObject* Game::findObjectWithName(std::string gameObjectName) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (this->gameObjects[i]->name == gameObjectName) {
			return this->gameObjects[i];
		}
	}
	return nullptr;
}

void Game::startGame() {
	// Runs awake() on all objects
	for (int i = 0; i < this->gameObjects.size(); i++) {
		this->gameObjects[i]->awake();
	}

	// Start the game loop
	gameLoop();
}

void Game::gameLoop() {
	bool shouldClose = false;
	while (!shouldClose) {
		// input
		this->inputPtr->processInput(this->program->window);

		// physics simulations
		while (this->physicsTimeSimulated < glfwGetTime()) {
			fixedUpdate();
			// TODO: physics step
			this->physicsTimeSimulated += this->physicsTimeStep;
		}

		// update (deltaTime)
		update((float)(glfwGetTime() - this->lastUpdateTime));

		// render
		shouldClose = render();

		// end of frame
		this->lastUpdateTime = glfwGetTime();
	}
}

void Game::update(float deltaTime) {
	// Runs update() on all objects
	for (int i = 0; i < this->gameObjects.size(); i++) {
		this->gameObjects[i]->update(deltaTime);
	}
}

void Game::fixedUpdate() {
	// Runs fixedUpdate() on all objects
	for (int i = 0; i < this->gameObjects.size(); i++) {
		this->gameObjects[i]->fixedUpdate();
	}
}

bool Game::render() {
	if (!glfwWindowShouldClose(this->program->window)) {
		// Runs render on all objects
		for (int i = 0; i < this->gameObjects.size(); i++) {
			this->gameObjects[i]->render();
		}

		// show what has been rendered
		glfwPollEvents();
		glfwSwapBuffers(this->program->window);
	}
	else { // if we should close
		return true;
	}

	return false;
}
