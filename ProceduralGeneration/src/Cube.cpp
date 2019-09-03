#include "Cube.h"

#include "engine/GameObject.h"
#include "engine/Transform.h"
#include "engine/MeshRenderer.h"

#include <string>

using namespace ProceduralGeneration;

Cube::Cube(std::string name) : GameObject(name) {
	// setup components
	this->transform = new Engine::Transform();
	this->renderer = new Engine::MeshRenderer();
}

void Cube::awake() {

}

void Cube::update(float deltaTime) {

}

void Cube::fixedUpdate() {

}

void Cube::render() {

}
