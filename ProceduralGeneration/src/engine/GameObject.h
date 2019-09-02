#pragma once

#include "Renderer.h"

#include <string>
#include <memory>

namespace Engine {
	class GameObject {
	private:
		std::shared_ptr<Renderer> renderer;
	public:
		std::string name;

		virtual void awake() = 0;
		virtual void update() = 0;
		virtual void fixedUpdate() = 0;
		virtual void render() = 0;
	};
}
