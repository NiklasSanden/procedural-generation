#pragma once

namespace Engine {
	class Program;
	class InputBase;
	class GameObject;
}

#include <string>
#include <vector>

namespace Engine {
	class Game {
	private:
		Program* program = nullptr;
		InputBase* inputPtr = nullptr;
		std::vector<GameObject*> gameObjects;

		// deltaTime etc
		double physicsTimeSimulated = 0.0;
		double lastUpdateTime = 0.0f;
		float physicsTimeStep = 0.2f;

	public:
		Game(Program* _program, InputBase* input);
		~Game();

		void addGameObject(GameObject* gameObject);
		void deleteGameObject(std::string& gameObjectName);
		GameObject* findObjectWithName(std::string& gameObjectName);
		
		void startGame();
	private:
		void gameLoop();
		void update(float deltaTime);
		void fixedUpdate();
		bool render();
	};
}
