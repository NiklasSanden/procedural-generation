#pragma once

struct GLFWwindow;

#include "engine/misc/InputBase.h"

#include "glm/glm/vec2.hpp"

namespace ProceduralGeneration {
	class Input : public Engine::InputBase {
	private:
		// settings
		float sensitivity = 0.0005f;

		// member variables
		glm::vec2 mouseDelta = glm::vec2(0.0f, 0.0f); // stores the amount the mouse has moved - resets when accessed
		int scrollDelta = 0;						  // stores the scroll input			   - resets when accessed

		// windowPtr to ask for input
		GLFWwindow* window = nullptr;
	public:
		Input(GLFWwindow* _window) : window(_window) {};
		~Input() {};

		void mouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;

		void processInput(GLFWwindow* window) override;

		glm::vec2 getMouseDelta();
		int getScrollDelta();

		bool isKeyPressed(unsigned int key);
	};
}
