#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "Shader.h"
#include "ShaderFilesHandler.h"

#include <iostream>

// Error handling
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {
	
	// https://blog.nobel-joergensen.com/2013/02/17/debugging-opengl-part-2-using-gldebugmessagecallback/

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


// Signatures
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

int main() {
	
	// --------------------------------------------------------------------------
	// GLFW Setup
	// --------------------------------------------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// --------------------------------------------------------------------------
	// GLAD Setup
	// --------------------------------------------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// --------------------------------------------------------------------------
	// Callback functions
	// --------------------------------------------------------------------------
	// error handling
#if _DEBUG
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	framebuffer_size_callback(window, SCREEN_WIDTH, SCREEN_HEIGHT);


	// --------------------------------------------------------------------------
	// Shaders
	// --------------------------------------------------------------------------
	// setup and compile shaders
	ShaderFileHandler::setupShaderSource();

	// create shader program
	Shader ourShader({ ShaderFileHandler::getShaderID("shader.vert"), ShaderFileHandler::getShaderID("shader.frag") });


	// --------------------------------------------------------------------------
	// Rendering setup
	// --------------------------------------------------------------------------
	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
	};
	unsigned int indices[]{
		0, 1, 2
	};

	// Generate buffers
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	// Bind Vertex Array Object
	glBindVertexArray(VAO);
	
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Vertex attributes
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	// wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

	// --------------------------------------------------------------------------
	// Render Loop
	// --------------------------------------------------------------------------
	while (!glfwWindowShouldClose(window)) {
		// input
		processInput(window);

		// rendering commands here
		// -----------------------
		// clear screen
		glClearColor(0.2f, 0.25f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// set shader program
		ourShader.use();

		// render
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);


		// check all events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}


	// --------------------------------------------------------------------------
	// Cleanup
	// --------------------------------------------------------------------------
	// de-allocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// delete shaders
	ShaderFileHandler::cleanup();

	// clears all previously allocated GLFW resources
	glfwTerminate();


	std::cout << "---------------------------\nDone - Press enter to close\n---------------------------" << std::endl;
	std::cin.get();
	return 0;

}

// Process input
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// Set the viewport size equal to the window size when the user resizes the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}