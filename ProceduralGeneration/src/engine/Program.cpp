#include "Program.h"
#include "InputBase.h"

#include "ImGUILayer.h"

#include <iostream>

#include "Debug.h"
using namespace Engine;

unsigned int Program::SCREEN_WIDTH;
unsigned int Program::SCREEN_HEIGHT;

Program::Program(unsigned int screenWidth, unsigned int screenHeight) {
	std::cout << "Creating window" << std::endl;
	SCREEN_WIDTH = screenWidth;
	SCREEN_HEIGHT = screenHeight;

	// --------------------------------------------------------------------------
	// GLFW Setup
	// --------------------------------------------------------------------------
	glfwSetErrorCallback(glfwErrorCallback);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	this->window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ProceduralGeneration", NULL, NULL);
	if (this->window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw std::exception();
	}
	glfwMakeContextCurrent(this->window);

	// limit fps to refresh rate
	glfwSwapInterval(1);

	// capture mouse
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(this->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);


	// --------------------------------------------------------------------------
	// GLAD Setup
	// --------------------------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw std::exception();
	}


	// --------------------------------------------------------------------------
	// ImGUI Setup
	// --------------------------------------------------------------------------
	this->imGuiLayer = new ImGuiLayer(this->window);


	// --------------------------------------------------------------------------
	// Callback functions
	// --------------------------------------------------------------------------
	// error handling
#ifdef CFG_DEBUG
	if (glDebugMessageCallback) {
		std::cout << "Register OpenGL debug callback\n" << std::endl;
		glEnable(GL_DEBUG_OUTPUT); // GL_DEBUG_OUTPUT_SYNCHRONOUS
		glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}
	else
		std::cout << "glDebugMessageCallback not available\n" << std::endl;
#endif

	// window resizing
	glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
	framebuffer_size_callback(this->window, SCREEN_WIDTH, SCREEN_HEIGHT);

	// mouse input
	glfwSetCursorPosCallback(this->window, mouse_callback);
	glfwSetScrollCallback(this->window, scroll_callback);


	// --------------------------------------------------------------------------
	// OpenGL enable optional functionality
	// --------------------------------------------------------------------------
	glEnable(GL_DEPTH_TEST);
}

Program::~Program() {
	std::cout << "Destroying window" << std::endl;

	// cleanup ImGuiLayer
	delete this->imGuiLayer;

	// clears all previously allocated GLFW resources
	glfwDestroyWindow(window);
	glfwTerminate();
}

// GLFW error handling
void Engine::glfwErrorCallback(int error, const char* description) {
	std::cout << "--------------------\nGLFW Error: " << error << "\nDescription: " << description << "\n--------------------" << std::endl;
}

// OpenGL error handling
void APIENTRY Engine::GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {

	// https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/

	// filter
	// ---------------------------
	// Useless nvidia notification
	if (id == 131185) return;

	std::cout << "---------------------opengl-callback-start------------" << std::endl;
	std::cout << "message: " << msg << std::endl;
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}
	std::cout << std::endl;

	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "Notification";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	}
	std::cout << std::endl;
	std::cout << "---------------------opengl-callback-end--------------\n" << std::endl;

}

// Set the viewport size equal to the window size when the user resizes the window
void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	// This will be false when you minimize
	if (width > 0)
		Program::SCREEN_WIDTH = width;
	if (height > 0)
		Program::SCREEN_HEIGHT = height;
}
