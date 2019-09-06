#pragma once

#include "ImGUILayer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Engine {
	// Callbacks
	void glfwErrorCallback(int error, const char* description);
	void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	class Program {
	public:
		// Layers
		ImGuiLayer* imGuiLayer;
	public:
		GLFWwindow* window;

		Program(unsigned int screenWidth, unsigned int screenHeight);
		~Program();

		static unsigned int SCREEN_WIDTH;
		static unsigned int SCREEN_HEIGHT;
	};
}
