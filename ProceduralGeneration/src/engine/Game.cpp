#include "Game.h"
#include "Input.h"
#include "ShaderManager.h"
#include "ResourceManager.h"
#include "ImGUILayer.h"
#include "Program.h"
#include "GameObject.h"
#include "InputBase.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <string>

#include "Debug.h"
using namespace Engine;

Game::Game(Program* _program, InputBase* input) {
	this->program = _program;
	this->inputPtr = input;

	ShaderManager::setupShaderSource();
}

Game::~Game() {
	ShaderManager::cleanup();
	ResourceManager::cleanup();

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

void Game::deleteGameObject(std::string& gameObjectName) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (this->gameObjects[i]->name == gameObjectName) {
			std::cout << "Deleting gameObject: " << this->gameObjects[i]->name << std::endl,
	
			delete this->gameObjects[i];

			this->gameObjects.erase(this->gameObjects.begin() + i);
		}
	}
}

GameObject* Game::findObjectWithName(std::string& gameObjectName) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (this->gameObjects[i]->name == gameObjectName) {
			return this->gameObjects[i];
		}
	}
	return nullptr;
}

void Game::startGame() {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		this->gameObjects[i]->awake();
	}

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
		float deltaTime = (float)(glfwGetTime() - this->lastUpdateTime);
		this->lastUpdateTime = glfwGetTime();
		update(deltaTime);

		// render
		shouldClose = render();
	}
}

void Game::update(float deltaTime) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		this->gameObjects[i]->update(deltaTime);
	}
}

void Game::fixedUpdate() {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		this->gameObjects[i]->fixedUpdate();
	}
}

// wireframe
// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
bool Game::render() {
	if (!glfwWindowShouldClose(this->program->window)) {
		// setup new frame for ImGui
		this->program->imGuiLayer->begin();

		// clear screen
		glClearColor(0.2f, 0.25f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Runs render and renderImGui on all objects
		for (int i = 0; i < this->gameObjects.size(); i++) {
			this->gameObjects[i]->render();
			this->gameObjects[i]->renderImGui();
		}

		// render ImGui
		this->program->imGuiLayer->end();

		// show what has been rendered
		glfwPollEvents();
		glfwSwapBuffers(this->program->window);
	}
	else { // if we should close
		return true;
	}

	return false;
}
