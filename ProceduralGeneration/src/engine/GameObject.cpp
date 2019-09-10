#include "GameObject.h"

#include "Renderer.h"
#include "Transform.h"

#include <string>
#include <iostream>

#include "Debug.h"
using namespace Engine;

GameObject::GameObject(const std::string& _name) { 
	this->name = _name; 
}

GameObject::~GameObject() {
	delete this->renderer;
	delete this->transform;
}
