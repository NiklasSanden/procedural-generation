#include "InputBase.h"

using namespace Engine;

// Mouse movement
void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	inputPtr->mouseCallback();
}

// Scrolling
void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	inputPtr->scrollCallback();
}
