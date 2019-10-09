#include "Game.h"
#include "setup/Input.h"
#include "engine/resources/ShaderManager.h"
#include "engine/resources/ResourceManager.h"
#include "ImGUILayer.h"
#include "Program.h"
#include "engine/objects/GameObject.h"
#include "InputBase.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "engine/misc/Debug.h"
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
		LogManager::Log("Deleting gameObject: " + this->gameObjects[i]->name);
		delete this->gameObjects[i];
	}
	this->gameObjects.clear();
}

void Game::addGameObject(GameObject* gameObject) {
	LogManager::Log("Adding gameObject: " + gameObject->name);
	this->gameObjects.push_back(gameObject);
}

void Game::deleteGameObject(const std::string& gameObjectName) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (this->gameObjects[i]->name == gameObjectName) {
			LogManager::Log("Deleting gameObject: " + this->gameObjects[i]->name);
	
			delete this->gameObjects[i];

			this->gameObjects.erase(this->gameObjects.begin() + i);
		}
	}
}

void Game::deleteGameObjects(const std::unordered_set<std::string>& gameObjectNames) {
	std::vector<GameObject*> newGameObjectList;

	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (gameObjectNames.find(this->gameObjects[i]->name) == gameObjectNames.end()) {
			newGameObjectList.push_back(this->gameObjects[i]);
		}
		else {
			LogManager::Log("Deleting gameObject: " + this->gameObjects[i]->name);
			delete this->gameObjects[i];
		}
	}

	this->gameObjects = newGameObjectList;
}

GameObject* Game::findObjectWithName(const std::string& gameObjectName) {
	for (int i = 0; i < this->gameObjects.size(); i++) {
		if (this->gameObjects[i]->name == gameObjectName) {
			return this->gameObjects[i];
		}
	}
	return nullptr;
}

void Game::startGame() {
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
double time = 0.0;
bool Game::render() {
	if (!glfwWindowShouldClose(this->program->window)) {
		double tempTime = glfwGetTime();
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
		ImGui::Begin("Game Performance");
		ImGui::Text("Rendering time %.3f ms/frame", time * 1000.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		this->program->imGuiLayer->end();
		time = glfwGetTime() - tempTime;
		// show what has been rendered
		glfwPollEvents();
		glfwSwapBuffers(this->program->window);
	}
	else { // if we should close
		return true;
	}

	return false;
}
