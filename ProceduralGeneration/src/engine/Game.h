#pragma once

#include "Program.h"
#include "GameObject.h"
#include "InputBase.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <memory>

namespace Engine {
	class Game {
	private:
		std::shared_ptr<Program> program = nullptr;
		std::shared_ptr<InputBase> inputPtr = nullptr;
		std::vector<std::shared_ptr<GameObject>> gameObjects;

		// deltaTime etc
		double physicsTimeSimulated = 0.0;
		double lastUpdateTime = 0.0f;
		float physicsTimeStep = 0.2f;

	public:
		Game(std::shared_ptr<Program> _program, std::shared_ptr<InputBase> input);
		~Game();

		void addGameObject(std::shared_ptr<GameObject> gameObject);
		void deleteGameObject(std::string gameObjectName);
		void startGame();
	private:
		void gameLoop();
		void update(float deltaTime);
		void fixedUpdate();
		bool render();
	};
}
