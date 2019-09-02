#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace Engine {
	// Callbacks
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	// Abstract class
	class InputBase {
	public:
		virtual void mouseCallback(GLFWwindow* window, double xpos, double ypos) = 0;
		virtual void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;

		virtual void processInput(GLFWwindow* window) = 0;
	};

	void setInputPtr(std::shared_ptr<InputBase> _inputPtrCallback);
}
