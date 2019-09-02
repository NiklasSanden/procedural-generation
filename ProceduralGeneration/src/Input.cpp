#include "Input.h"

#include "GLFW/glfw3.h"

#include <iostream>

using namespace ProceduralGeneration;

void ProceduralGeneration::Input::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	std::cout << "mouse movement" << std::endl;
}

void ProceduralGeneration::Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	std::cout << "sroll input" << std::endl;
}


// Process input
bool firstMouseCallback = true;

bool isMouseDisabled = true;
bool resetESCKey = true;
void ProceduralGeneration::Input::processInput(GLFWwindow* window) {
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
