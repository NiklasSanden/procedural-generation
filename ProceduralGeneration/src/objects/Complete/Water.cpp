#include "objects/Complete/Water.h"

#include "engine/misc/Game.h"
#include "engine/resources/ResourceManager.h"
#include "engine/resources/Texture2D.h"
#include "engine/resources/Material.h"
#include "engine/components/Transform.h"
#include "engine/resources/Shader.h"
#include "engine/objects/Lights.h"
#include "engine/components/MeshRenderer.h"
#include "engine/misc/Program.h"
#include "setup/GameManager.h"
#include "setup/Camera.h"
#include "data/Tables.h"
#include "data/NoiseTexture.h"

#include "objects/Complete/VertexBuffer.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <queue>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

Water::Water(int pointsPerAxis) {
	this->pointsPerAxis = pointsPerAxis;

	this->material = new Engine::Material(glm::vec3(0.7f, 0.2f, 0.1f), glm::vec3(0.5f, 0.5f, 0.5f), 8);

	this->renderingShaders = Engine::ResourceManager::createShaderProgram({ "water.vert", "water.frag" }, "WaterRenderingShaders");
	this->noiseShaders = Engine::ResourceManager::createShaderProgram({ "water.comp" }, "WaterComputeShaders");

	this->waterTexture = Engine::ResourceManager::getTexture("Water.jpg");

	glGenTextures(1, &this->noiseTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, pointsPerAxis + 2, pointsPerAxis + 2, 0, GL_RED, GL_FLOAT, NULL);


	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	std::vector<float> bufferData;
	for (int y = 0; y < pointsPerAxis; y++) {
		for (int x = 0; x < pointsPerAxis; x++) {
			bufferData.push_back(x - (pointsPerAxis - 1.0f) / 2.0f);
			bufferData.push_back(-y + (pointsPerAxis - 1.0f) / 2.0f);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

	
	std::vector<unsigned int> EBOData;
	for (int y = 0; y < pointsPerAxis - 1; y++) {
		for (int x = 0; x < pointsPerAxis - 1; x++) {
			EBOData.push_back(x + y * pointsPerAxis);
			EBOData.push_back((x + 1) + y * pointsPerAxis);
			EBOData.push_back(x + (y + 1) * pointsPerAxis);

			EBOData.push_back((x + 1) + y * pointsPerAxis);
			EBOData.push_back((x + 1) + (y + 1) * pointsPerAxis);
			EBOData.push_back(x + (y + 1) * pointsPerAxis);
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBOData.size() * sizeof(unsigned int), EBOData.data(), GL_STATIC_DRAW);
	this->amountOfIndices = EBOData.size();

	glBindVertexArray(0);
}

Water::~Water() {
	glDeleteTextures(1, &this->noiseTexture);

	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);

	delete this->material;
}

void Water::regenerate(int pointsPerAxis) {
	this->pointsPerAxis = pointsPerAxis;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, pointsPerAxis + 2, pointsPerAxis + 2, 0, GL_RED, GL_FLOAT, NULL);


	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	std::vector<float> bufferData;
	for (int y = 0; y < pointsPerAxis; y++) {
		for (int x = 0; x < pointsPerAxis; x++) {
			bufferData.push_back(x - (pointsPerAxis - 1.0f) / 2.0f);
			bufferData.push_back(-y + (pointsPerAxis - 1.0f) / 2.0f);
		}
	}
	glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);

	std::vector<unsigned int> EBOData;
	for (int y = 0; y < pointsPerAxis - 1; y++) {
		for (int x = 0; x < pointsPerAxis - 1; x++) {
			EBOData.push_back(x + y * pointsPerAxis);
			EBOData.push_back((x + 1) + y * pointsPerAxis);
			EBOData.push_back(x + (y + 1) * pointsPerAxis);

			EBOData.push_back((x + 1) + y * pointsPerAxis);
			EBOData.push_back((x + 1) + (y + 1) * pointsPerAxis);
			EBOData.push_back(x + (y + 1) * pointsPerAxis);
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBOData.size() * sizeof(unsigned int), EBOData.data(), GL_STATIC_DRAW);
	this->amountOfIndices = EBOData.size();

	glBindVertexArray(0);
}

void Water::waterCoord(const glm::ivec3& coord) {
	this->waterToDraw.push_back(std::make_pair(std::to_string(coord.x) + " 0 " + std::to_string(coord.z), glm::ivec2(coord.x, coord.z)));
}

void Water::draw(float chunkLength, float cellLength) {
	this->renderingShaders->use();
	// directional light
	if (GameManager::getDirectionalLight()) {
		GameManager::getDirectionalLight()->setUniform(this->renderingShaders, Camera::viewMatrix);
	}
	else { // let the shader know that a directional light doesn't exist
		this->renderingShaders->setBool("directionalLight.exists", false);
	}

	// Renderer stuff
	// Uniforms
	this->renderingShaders->setMat4("view", Camera::viewMatrix);
	this->renderingShaders->setMat4("projection", Camera::projectionMatrix);
	this->renderingShaders->setFloat("viewDistance", Camera::viewDistance);
	this->renderingShaders->setInt("pointsPerAxis", this->pointsPerAxis);
	this->renderingShaders->setVec2("waterOffset", glm::vec2(glfwGetTime() / 20.0f, glfwGetTime() / 30.0f));
	this->renderingShaders->setVec3("playerPos", GameManager::getPlayer()->transform->getPosition());

	// material
	this->renderingShaders->setVec3("material.diffuse", this->material->diffuse);
	this->renderingShaders->setVec3("material.specular", this->material->specular);
	this->renderingShaders->setFloat("material.shininess", this->material->shininess);

	this->renderingShaders->setFloat("chunkLength", chunkLength);
	this->renderingShaders->setFloat("cellLength", cellLength);
	this->renderingShaders->setFloat("alpha", 0.75f);

	// Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);
	glBindImageTexture(0, this->noiseTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

	glActiveTexture(GL_TEXTURE3);
	Engine::ResourceManager::getTexture("Water.jpg")->bind();
	glActiveTexture(GL_TEXTURE4);
	
	this->noiseShaders->use();
	this->noiseShaders->setFloat("cellLength", cellLength);
	this->noiseShaders->setInt("pointsPerAxis", this->pointsPerAxis + 2);
	this->noiseShaders->setFloat("yValue", glm::sin(glfwGetTime() / 2.0f) / 1.5f);
	//this->noiseShaders->setFloat("yValue", 0.0f);
	this->noiseShaders->setVec2("waterOffset", glm::vec2(glfwGetTime() / 20.0f, glfwGetTime() / 30.0f));

	for (auto str : this->waterToDraw) {
		this->noiseShaders->use();
		this->noiseShaders->setVec3("position", glm::vec3(str.second.x, 0.0f, str.second.y) * chunkLength);
		// Compute shader
		glDispatchCompute((GLuint)(this->pointsPerAxis + 2), 1, (GLuint)(this->pointsPerAxis + 2));

		this->renderingShaders->use();
		this->renderingShaders->setVec2("chunkPosition", str.second);

		// Make sure the noise has finished generating
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
				
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, amountOfIndices, GL_UNSIGNED_INT, (GLsizei)0);
	}

	this->waterToDraw.clear();
}
