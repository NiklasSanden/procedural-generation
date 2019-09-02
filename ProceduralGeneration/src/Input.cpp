#include "Input.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <iostream>

using namespace ProceduralGeneration;

namespace ProceduralGeneration {
	bool firstMouseCallback = true;
	double mouseLastX = 0.0;
	double mouseLastY = 0.0;
}
void Input::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	// avoid big differences when starting the game and switching cursor modes
	if (firstMouseCallback) {
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouseCallback = false;
	}

	float xOffset = (float)(xpos - mouseLastX);
	float yOffset = (float)(mouseLastY - ypos); // reversed since y-coordinates range from bottom to top
	mouseLastX = xpos;
	mouseLastY = ypos;
	
	xOffset *= this->sensitivity;
	yOffset *= this->sensitivity;

	this->mouseDelta += glm::vec2(xOffset, yOffset);
}
glm::vec2 Input::getMouseDelta() {
	glm::vec2 tempMouseDelta = this->mouseDelta;
	this->mouseDelta = glm::vec2(0.0f, 0.0f); // reset mouseDelta
	return tempMouseDelta;
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	this->scrollDelta -= (int)yoffset; // we only care about the yoffset of the scrollwheel
}
int Input::getScrollDelta() {
	int tempScrollDelta = this->scrollDelta;
	this->scrollDelta = 0; // reset mouseDelta
	return tempScrollDelta;
}


// Process input
namespace ProceduralGeneration {
	bool isMouseDisabled = true;
	bool resetESCKey = true;
}
void Input::processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && resetESCKey) {
		// Uncapture mouse
		if (isMouseDisabled) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			isMouseDisabled = false;
			firstMouseCallback = true;
		}
		// Capture mouse
		else {
			glfwSetWindowShouldClose(window, true);
		}
		resetESCKey = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		resetESCKey = true;
	}

	// Capture mouse
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (!isMouseDisabled) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			isMouseDisabled = true;
		}
	}
}

bool Input::isKeyPressed(unsigned int key) {
	if (!window) return false;
	return glfwGetKey(window, key) == GLFW_PRESS;
}