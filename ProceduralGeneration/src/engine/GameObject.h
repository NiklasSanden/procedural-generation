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

		GameObject(std::string& _name);
		// delete components
		virtual ~GameObject();

		virtual void awake() {};
		virtual void update(float deltaTime) {};
		virtual void fixedUpdate() {};
		virtual void render() {};
		virtual void renderImGui() {};
	};
}
