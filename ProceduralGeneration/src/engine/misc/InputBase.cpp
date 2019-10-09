#include "setup/Input.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/misc/Debug.h"
using namespace Engine;

// Pointer to the input class that inherits InputBase in the acutal game implementation
namespace Engine {
	InputBase* inputPtrCallback;
}

void Engine::setInputPtr(InputBase* _inputPtrCallback) {
	inputPtrCallback = _inputPtrCallback;
}

void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (inputPtrCallback)
		inputPtrCallback->mouseCallback(window, xpos, ypos);
}

void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (inputPtrCallback)
		inputPtrCallback->scrollCallback(window, xoffset, yoffset);
}
