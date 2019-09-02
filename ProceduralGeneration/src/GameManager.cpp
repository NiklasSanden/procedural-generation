#include "GameManager.h"
#include "engine/Game.h"

#include <memory>

using namespace ProceduralGeneration;

std::shared_ptr<Engine::Game> GameManager::gamePtr = nullptr;

std::shared_ptr<Engine::Game> GameManager::getGamePtr() {
	return gamePtr;
}

void GameManager::setGamePtr(std::shared_ptr<Engine::Game> _gamePtr) {
	gamePtr = _gamePtr;
}
