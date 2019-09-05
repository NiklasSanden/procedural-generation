#include "Game.h"
#include "Input.h"
#include "ShaderManager.h"
#include "ResourceManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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

void Game::deleteGameObject(std::string& gameObjectName) {
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

GameObject* Game::findObjectWithName(std::string& gameObjectName) {
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
		float deltaTime = (float)(glfwGetTime() - this->lastUpdateTime);
		this->lastUpdateTime = glfwGetTime();
		update(deltaTime);

		// render
		shouldClose = render();
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


// Our state
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// wireframe
// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
bool Game::render() {
	if (!glfwWindowShouldClose(this->program->window)) {
		// clear screen
		glClearColor(0.2f, 0.25f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Runs render on all objects
		for (int i = 0; i < this->gameObjects.size(); i++) {
			this->gameObjects[i]->render();
		}

		// ----------------------------------
		// ImGUI
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// Render ImGUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// ----------------------------------

		// show what has been rendered
		glfwPollEvents();
		glfwSwapBuffers(this->program->window);
	}
	else { // if we should close
		return true;
	}

	return false;
}
