#pragma once

namespace Engine {
	class Game;
	class Renderer;
	class DirectionalLight;
	class PointLight;
}
namespace ProceduralGeneration { 
	class Input;
	class Camera;
}

#include "engine/GameObject.h"

#include <vector>

namespace ProceduralGeneration {
	class GameManager : public Engine::GameObject {
	// static part
	private:
		static Engine::Game* gamePtr;
		static Input* inputPtr;
		static Engine::DirectionalLight* directionalLight;
		static std::vector<Engine::PointLight*> pointLights;

		static Camera* player;
	public:
		static Engine::Game* getGamePtr();
		static void setGamePtr(Engine::Game* _gamePtr);
		static Input* getInputPtr();
		static void setInputPtr(Input* _inputPtr);

		static Engine::DirectionalLight* getDirectionalLight();
		static std::vector<Engine::PointLight*> getPointLights();

		static Camera* getPlayer();

	// instanced part
	public:
		GameManager(const std::string& _name);
		~GameManager();

		void update(float deltaTime) override;
		void fixedUpdate() override;
		void renderImGui() override;
	};
}
