#pragma once

#include "engine/Game.h"

#include <memory>

namespace ProceduralGeneration {
	class GameManager {
	private:
		static std::shared_ptr<Engine::Game> gamePtr;
	public:
		static std::shared_ptr<Engine::Game> getGamePtr();
		static void setGamePtr(std::shared_ptr<Engine::Game> _gamePtr);
	};
}
