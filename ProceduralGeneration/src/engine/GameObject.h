#pragma once

namespace Engine {
	class Renderer;
	class Transform;
}

#include <string>

namespace Engine {
	class GameObject {
	public:
		Renderer* renderer = nullptr;
		Transform* transform = nullptr;
	public:
		std::string name = "null";

		GameObject(std::string& _name);
		virtual ~GameObject();

		virtual void awake() {};
		virtual void update(float deltaTime) {};
		virtual void fixedUpdate() {};
		virtual void render() {};
		virtual void renderImGui() {};
	};
}
