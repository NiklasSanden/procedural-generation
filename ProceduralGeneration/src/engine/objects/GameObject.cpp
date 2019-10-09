#include "engine/objects/GameObject.h"

#include "engine/components/Renderer.h"
#include "engine/components/Transform.h"

#include <string>
#include <iostream>

#include "engine/misc/Debug.h"
using namespace Engine;

GameObject::GameObject(const std::string& _name) { 
	this->name = _name; 
}

GameObject::~GameObject() {
	delete this->renderer;
	delete this->transform;
}
