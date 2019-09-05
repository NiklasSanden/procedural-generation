#pragma once

#include "engine/Game.h"
#include "engine/GameObject.h"
#include "engine/Renderer.h"
#include "Input.h"

#include <iostream>

namespace ProceduralGeneration {
	class GameManager : public Engine::GameObject {
	// static part
	private:
		static Engine::Game* gamePtr;
		static Input* inputPtr;
	public:
		static Engine::Game* getGamePtr();
		static void setGamePtr(Engine::Game* _gamePtr);
		static Input* getInputPtr();
		static void setInputPtr(Input* _inputPtr);

	// instanced part
	public:
		GameManager(std::string& _name);
		~GameManager();

		void awake() override;
		void update(float deltaTime) override;
		void fixedUpdate() override;
		void render() override;
	};
}
