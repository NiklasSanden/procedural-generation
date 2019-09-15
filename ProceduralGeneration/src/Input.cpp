
#include "Input.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

namespace ProceduralGeneration {
	bool firstMouseCallback = true;
	double mouseLastX = 0.0;
	double mouseLastY = 0.0;
	bool isMouseDisabled = true;
	bool resetESCKey = true;
}
void Input::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	// avoid big differences when starting the game and switching cursor modes
	// and don't do it when our mouse isn't being captured (so we don't look around when clicking UI)
	if (firstMouseCallback || !isMouseDisabled) {
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouseCallback = false;
	}

	float xOffset = (float)(xpos - mouseLastX);
	float yOffset = (float)(mouseLastY - ypos);
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
	this->scrollDelta -= (int)yoffset;
}
int Input::getScrollDelta() {
	int tempScrollDelta = this->scrollDelta;
	this->scrollDelta = 0; // reset mouseDelta
	return tempScrollDelta;
}

void Input::processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && resetESCKey) {
		// Uncapture mouse
		if (isMouseDisabled) {
			// Show in glfw
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Make sure that the mouse interacts with ImGUI
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags ^= ImGuiConfigFlags_NoMouse;

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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		if (!isMouseDisabled) {
			// Hide in glfw
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			// Make sure that the mouse doesn't interact with ImGUI
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;

			isMouseDisabled = true;
		}
	}
}

bool Input::isKeyPressed(unsigned int key) {
	if (!window) return false;
	return glfwGetKey(window, key) == GLFW_PRESS;
}