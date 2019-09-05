#pragma once

#include "Renderer.h"
#include "Transform.h"

#include <string>
#include <iostream>

namespace Engine {
	class GameObject {
	public:
		Renderer* renderer = nullptr;
		Transform* transform = nullptr;
	public:
		std::string name = "null";

		GameObject(std::string& _name) { name = _name; }
		// delete components
		virtual ~GameObject() {
			std::cout << "Deleting components from gameObject: " << name << std::endl;
			delete renderer;
			delete transform;
		}

		virtual void awake() = 0;
		virtual void update(float deltaTime) = 0;
		virtual void fixedUpdate() = 0;
		virtual void render() = 0;
	};
}
