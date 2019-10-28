#include "Program.h"
#include "InputBase.h"

#include "ImGUILayer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "extensions/PxDefaultAllocator.h"	   // This implements the required PxAllocatorCallback for us

#include <iostream>

#include "engine/misc/Debug.h"
using namespace Engine;

// Static initialization
unsigned int Program::SCREEN_WIDTH;
unsigned int Program::SCREEN_HEIGHT;

Program::Program(unsigned int screenWidth, unsigned int screenHeight) {
	LogManager::Log("Creating window");
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
		LogManager::LogError("Failed to create GLFW window");
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
		LogManager::LogError("Failed to initialize GLAD");
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
		LogManager::Log("Register OpenGL debug callback\n");
		glEnable(GL_DEBUG_OUTPUT); // GL_DEBUG_OUTPUT_SYNCHRONOUS
		glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}
	else
		LogManager::LogError("glDebugMessageCallback not available");
#else
	if (glDebugMessageCallback) {
		glEnable(GL_DEBUG_OUTPUT); // GL_DEBUG_OUTPUT_SYNCHRONOUS
		glDebugMessageCallback(GLDebugMessageCallback, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	}
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
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Program::~Program() {
	LogManager::Log("Destroying window");

	delete this->imGuiLayer; // the destructor will clean ImGui up

	glfwDestroyWindow(window);
	glfwTerminate();
}

// GLFW error handling
void Engine::glfwErrorCallback(int error, const char* description) {
	LogManager::LogError("\nGLFW Error: " + std::to_string(error) + "\nDescription: " + description + "\n--------------------");
}

// OpenGL error handling
void APIENTRY Engine::GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {

	// https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/

	// filter
	// ---------------------------
	// Useless nvidia notification
	if (id == 131185) return;
	// Memory from buffer is being copied from video to host
	if (id == 131186) return;
	// Nvidia pixel synchronization
	if (id == 131154) return;

	std::string finalMessage = "OpenGL\n";
	finalMessage += "---------------------opengl-callback-start------------";
	finalMessage += "message: " + std::string(msg);
	finalMessage += "\ntype: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		finalMessage += "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		finalMessage += "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		finalMessage += "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		finalMessage += "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		finalMessage += "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		finalMessage += "OTHER";
		break;
	}

	finalMessage += "\nid: " + std::to_string(id);
	finalMessage += "\nseverity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		finalMessage += "Notification";
		break;
	case GL_DEBUG_SEVERITY_LOW:
		finalMessage += "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		finalMessage += "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		finalMessage += "HIGH";
		break;
	}
	finalMessage += "\n---------------------opengl-callback-end--------------\n\n";

	LogManager::LogError(finalMessage);
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	// This will be false when you minimize
	if (width > 0)
		Program::SCREEN_WIDTH = width;
	if (height > 0)
		Program::SCREEN_HEIGHT = height;
}
