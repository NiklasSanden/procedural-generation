#include "GameObject.h"

#include "Debug.h"
using namespace Engine;

GameObject::GameObject(std::string& _name) { 
	this->name = _name; 
}

GameObject::~GameObject() {
	// std::cout << "Deleting components from gameObject: " << name << std::endl;
	delete this->renderer;
	delete this->transform;
}
