#include "engine/Program.h"
#include "engine/Game.h"
#include "engine/InputBase.h"

#include "Input.h"
#include "GameManager.h"

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include <iostream>
#include <string>

#include "engine/Debug.h"
using namespace Engine;

int main() {
// Check for memory leaks
#ifdef _DEBUG
	#ifdef _CRTDBG_MAP_ALLOC
		// Automatically call _CrtDumpMemoryLeaks() when the application is done
		// All static variables will have been cleared by then as well
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		LogManager::Log("Memory leaks will be sent to output\n---------------------------");
	#endif
#endif

	// Window
	Program* program = new Program(600, 600);
	// Input
	ProceduralGeneration::Input* input = new ProceduralGeneration::Input(program->window);
	setInputPtr(input); // For callbacks
	ProceduralGeneration::GameManager::setInputPtr(input);

	// Main game
	Game* game = new Game(program, input);
	ProceduralGeneration::GameManager::setGamePtr(game);

	// add gameManager
	ProceduralGeneration::GameManager* gameManager = new ProceduralGeneration::GameManager("GameManager");
	game->addGameObject(gameManager);

	// start the game
	game->startGame();

	LogManager::Log("---------------------------\nCleanup\n---------------------------");

	// Cleanup
	delete game;
	delete program;
	delete input;
	

	// Done
	LogManager::Log("---------------------------\nDone - Press enter to close\n---------------------------");
	std::cin.get();
	return 0;
}

//// Other global variables
//float lastFrame = 0.0f;
//float deltaTime = 0.0f;
//
//float mouseLastX = 0.0f;
//float mouseLastY = 0.0f;
//float yaw = -90.0f;
//float pitch = 0.0f;
//float fov = 70.0f;
//
//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//
//int main() {
//	
//	// --------------------------------------------------------------------------
//	// Shaders
//	// --------------------------------------------------------------------------
//	// setup and compile shaders
//	ShaderManager::setupShaderSource();
//
//	// create shader program
//	Shader ourShader({ ShaderManager::getShaderID("shader.vert"), ShaderManager::getShaderID("shader.frag") });
//
//	
//	// --------------------------------------------------------------------------
//	// Rendering setup
//	// --------------------------------------------------------------------------
//	float vertices[] = {
//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//
//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//
//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//
//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//	};
//	unsigned int indices[]{
//		0, 1, 3,
//		1, 2, 3
//	};
//
//	// Generate buffers
//	unsigned int VAO, VBO, EBO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//	
//	// Bind Vertex Array Object
//	glBindVertexArray(VAO);
//	
//	// Vertices
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	// Element buffer
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	// Vertex attributes
//	// position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//
//	// texture coordinates
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//
//
//	// load and create a texture
//	// -------------------------
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	stbi_set_flip_vertically_on_load(true);
//	// texture1
//	unsigned int texture1;
//	glGenTextures(1, &texture1);
//	glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	int width, height, nrChannels;
//	unsigned char* data = stbi_load("res/textures/saurons_eye.jpg", &width, &height, &nrChannels, 0);
//	if (data)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//	}
//	stbi_image_free(data);
//
//	// texture2
//	unsigned int texture2;
//	glGenTextures(1, &texture2);
//	glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
//	// set the texture wrapping parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	// set texture filtering parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	// load image, create texture and generate mipmaps
//	int width2, height2, nrChannels2;
//	unsigned char* data2 = stbi_load("res/textures/fire.jpg", &width2, &height2, &nrChannels2, 0);
//	if (data2)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//	}
//	stbi_image_free(data2);
//
//
//	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	glBindVertexArray(0);
//	//glBindTexture(GL_TEXTURE_2D, 0);
//
//
//
//	// set uniforms
//	ourShader.use();
//	ourShader.setInt("texture0", 0);
//	ourShader.setInt("texture1", 1);
//	glUseProgram(0);
//
//
//	// wireframe mode
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
//
//	glm::vec3 cubePositions[] = {
//		glm::vec3(0.0f,  0.0f,  0.0f),
//		glm::vec3(2.0f,  5.0f, -15.0f),
//		glm::vec3(-1.5f, -2.2f, -2.5f),
//		glm::vec3(-3.8f, -2.0f, -12.3f),
//		glm::vec3(2.4f, -0.4f, -3.5f),
//		glm::vec3(-1.7f,  3.0f, -7.5f),
//		glm::vec3(1.3f, -2.0f, -2.5f),
//		glm::vec3(1.5f,  2.0f, -2.5f),
//		glm::vec3(1.5f,  0.2f, -1.5f),
//		glm::vec3(-1.3f,  1.0f, -1.5f)
//	};
//
//	// --------------------------------------------------------------------------
//	// Render Loop
//	// --------------------------------------------------------------------------
//	/*int count = 0;
//	double oldTime = glfwGetTime();*/
//	while (!glfwWindowShouldClose(window)) {
//		// FPS counter
//		/*count++;
//		if (glfwGetTime() - oldTime > 1.0) {
//			count = 0;
//			oldTime = glfwGetTime();
//		}*/
//
//		// deltaTime
//		float currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//
//		// input
//		processInput(window);
//
//		// rendering commands here
//		// -----------------------
//		// clear screen
//		glClearColor(0.2f, 0.25f, 0.4f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		
//		// set shader program
//		ourShader.use();
//
//		// uniforms
//		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
//
//		glm::mat4 projection;
//		projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
//
//		ourShader.setMat4("view", view);
//		ourShader.setMat4("projection", projection);
//
//		// textures
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, texture1);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, texture2);
//
//		// render
//		glBindVertexArray(VAO);
//		for (unsigned int i = 0; i < 10; i++) {
//			glm::mat4 model = glm::mat4(1.0f);
//			model = glm::translate(model, cubePositions[i]);
//			model = glm::rotate(model, (float)glfwGetTime() + glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
//			ourShader.setMat4("model", model);
//
//			glDrawArrays(GL_TRIANGLES, 0, 36);
//		}
//		//glDrawArrays(GL_TRIANGLES, 0, 36);
//		//glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
//		//glBindVertexArray(0);
//
//
//		// check all events and swap the buffers
//		glfwPollEvents();
//		glfwSwapBuffers(window);
//	}
//
//
//	// --------------------------------------------------------------------------
//	// Cleanup
//	// --------------------------------------------------------------------------
//	// de-allocate resources
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteBuffers(1, &EBO);
//
//	// delete shaders
//	ShaderManager::cleanup();
//
//	// clears all previously allocated GLFW resources
//	glfwTerminate();
//
//
//	std::cin.get();
//	return 0;
//
//}
//
//// Process input
//bool firstMouseCallback = true;
//
//bool isMouseDisabled = true;
//bool resetESCKey = true;
//void processInput(GLFWwindow* window) {
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && resetESCKey) {
//		if (isMouseDisabled) {
//			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//			isMouseDisabled = false;
//			firstMouseCallback = true;
//		}
//		else {
//			glfwSetWindowShouldClose(window, true);
//		}
//		resetESCKey = false;
//	}
//	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
//		resetESCKey = true;
//	}
//
//	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
//		if (!isMouseDisabled) {
//			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//			isMouseDisabled = true;
//		}
//	}
//
//	float cameraSpeed = 2.5f * deltaTime;
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		cameraPos += cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		cameraPos -= cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
//}
//
//// Set the viewport size equal to the window size when the user resizes the window
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//	SCREEN_WIDTH = width;
//	SCREEN_HEIGHT = height;
//}
//
//// Mouse movement
//void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//	if (firstMouseCallback) {
//		mouseLastX = xpos;
//		mouseLastY = ypos;
//		firstMouseCallback = false;
//	}
//
//	float xOffset = xpos - mouseLastX;
//	float yOffset = mouseLastY - ypos; // reversed since y-coordinates range from bottom to top
//	mouseLastX = xpos;
//	mouseLastY = ypos;
//
//	float sensitivity = 0.025f;
//	xOffset *= sensitivity;
//	yOffset *= sensitivity;
//
//	yaw   += xOffset;
//	pitch += yOffset;
//
//	if (pitch > 89.0f)
//		pitch = 89.0f;
//	if (pitch < -89.0f)
//		pitch = -89.0f;
//
//	glm::vec3 front;
//	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//	front.y = sin(glm::radians(pitch));
//	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//	cameraFront = glm::normalize(front);
//}
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//	if (fov >= 1.0f && fov <= 45.0f)
//		fov -= yoffset;
//	if (fov <= 1.0f)
//		fov = 1.0f;
//	if (fov >= 45.0f)
//		fov = 45.0f;
//}