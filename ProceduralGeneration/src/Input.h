#pragma once

#include "engine/InputBase.h"

namespace ProceduralGeneration {
	class Input : public Engine::InputBase {
	public:
		void mouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;

		void processInput(GLFWwindow* window) override;
	};
}
