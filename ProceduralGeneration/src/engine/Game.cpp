#include "Game.h"
#include "Input.h"
#include "ShaderManager.h"

#include <iostream>
#include <string>
#include <memory>

using namespace Engine;

Game::Game(std::shared_ptr<Program> _program, std::shared_ptr<InputBase> input) {
	this->program = _program;
	this->inputPtr = input;

	// Compile shaders
	ShaderManager::setupShaderSource();
}

Game::~Game() {
	// Cleanup shaders
	ShaderManager::cleanup();
}

void Game::addGameObject(std::shared_ptr<GameObject> gameObject) {
	this->gameObjects.push_back(gameObject);
}

void Game::deleteGameObject(std::string gameObjectName) {

}

void Game::startGame() {
	// Runs awake() on all objects
	for (auto gameObject : this->gameObjects) {
		gameObject->awake();
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
	for (auto gameObject : this->gameObjects) {
		gameObject->update();
	}
}

void Game::fixedUpdate() {
	// Runs fixedUpdate() on all objects
	for (auto gameObject : this->gameObjects) {
		gameObject->fixedUpdate();
	}
}

bool Game::render() {
	if (!glfwWindowShouldClose(this->program->window)) {
		// Runs render on all objects
		for (auto gameObject : this->gameObjects) {
			gameObject->render();
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
