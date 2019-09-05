#include "Input.h"

#include "Debug.h"
using namespace Engine;

// Pointer to the input class that inherits InputBase in the acutal game
namespace Engine {
	InputBase* inputPtrCallback;
}

void Engine::setInputPtr(InputBase* _inputPtrCallback) {
	inputPtrCallback = _inputPtrCallback;
}

// Mouse movement
void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (inputPtrCallback)
		inputPtrCallback->mouseCallback(window, xpos, ypos);
}

// Scrolling
void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (inputPtrCallback)
		inputPtrCallback->scrollCallback(window, xoffset, yoffset);
}
