#include "objects/Raytracing/Raytracing.h"

#include "engine/misc/Game.h"
#include "engine/resources/ResourceManager.h"
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

#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>

#include "engine/misc/Debug.h"
using namespace ProceduralGeneration;

Raytracing::Raytracing(const std::string& name) : GameObject(name) {
	// Shader program
	this->shaderProgram = Engine::ResourceManager::createShaderProgram({ "raytracing.vert", "raytracing.frag" }, "RaytracingRenderingShader");
	this->computeShader = Engine::ResourceManager::createShaderProgram({ "raytracing.comp" }, "RaytracingComputeShader");
	// Material
	this->material = new Engine::Material();

	// Textures
	glGenTextures(1, &this->rayTexture);
	glGenTextures(1, &this->renderedTexture);
	glGenTextures(1, &this->noiseTexture);

	// Buffers
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	float vertexData[] = { 0.0f, 0.0f, 
						   1.0f, 0.0f,
						   0.0f, 1.0f,
						   1.0f, 1.0f };
	unsigned int indices[] = { 0, 1, 2,
							   2, 1, 3 };

	glBindVertexArray(this->VAO);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertexData, GL_STATIC_DRAW);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // this AFTER glBindVertexArray(0)

	// Setup
	updateRayTexture();
	generateNoise();
}

Raytracing::~Raytracing() {
	delete this->material;

	glDeleteTextures(1, &this->rayTexture);
	glDeleteTextures(1, &this->renderedTexture);
	glDeleteTextures(1, &this->noiseTexture);

	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->EBO);
}

void Raytracing::generateNoise() {
	srand(this->seed);

	const int noiseSize = 16;
	const int noiesSizeCubes = noiseSize * noiseSize * noiseSize;
	std::vector<float> noise(noiesSizeCubes);

	/*for (int i = 0; i < noiesSizeCubes; i++) {
		noise[i] = glm::linearRand(0.0f, 1.0f);
	}*/
	int index = 0;
	for (int x = 0; x < noiseSize; x++) {
		for (int y = 0; y < noiseSize; y++) {
			for (int z = 0; z < noiseSize; z++) {
				noise[index] = (Noise::octavePerlin(x, y, z, 1, 0.5, 0.3, this->seed, 1.0) + 1.0f) / 2.0f;
				index++;
			}
		}
	}

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, this->noiseTexture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, noiseSize, noiseSize, noiseSize, 0, GL_RED, GL_FLOAT, noise.data());
}

void Raytracing::update(float deltaTime) {
	//double tempTime = glfwGetTime();
	


	//std::cout << (glfwGetTime() - tempTime) * 1000.0f << std::endl;
}

void Raytracing::updateRayTexture() {
	Camera* player = GameManager::getPlayer();

	// Used by all chunks to determine if it can be seen or not
	// calculate horizontal fov from vertical: https://www.gamedev.net/forums/topic/361241-horizontal-fov/
	float halfVerticalFov = glm::radians(Camera::verticalFov / 2.0f);
	float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
	float halfHorizontalFov = 2.0f * glm::atan(glm::tan(glm::radians(Camera::verticalFov) * 0.5f) * aspectRatio) / 2.0f;

	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	glm::vec3 start = (front * ((float)this->width / 2.0f) / glm::atan(halfHorizontalFov)) - up * ((float)this->height / 2.0f) - right * ((float)this->width / 2.0f);

	std::vector<glm::vec3> rayTextureData;
	rayTextureData.reserve((size_t)this->width * this->height);
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			rayTextureData.push_back(glm::normalize(start + up * (float)y + right * (float)x));
		}
	}

	// Save to texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->rayTexture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->width, this->height, 0, GL_RGB, GL_FLOAT, rayTextureData.data());

	// Resize the renderedTexture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->renderedTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
}

void Raytracing::render() {
	// wireframe
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);*/

	this->computeShader->use();
	// Uniforms
	this->computeShader->setMat3("ViewMatrix", glm::transpose(glm::mat3(Camera::viewMatrix)));
	this->computeShader->setVec3("PlayerPos", GameManager::getPlayer()->transform->getPosition());
	//this->computeShader->setVec3("offset", glm::vec3(0.0f, 0.0f, 0.0f));
	this->computeShader->setFloat("VoxelRes", this->voxelRes);
	this->computeShader->setFloat("ViewDistance", this->viewDistance);

	// Material
	this->computeShader->setVec3("material.diffuse", this->material->diffuse);
	this->computeShader->setVec3("material.specular", this->material->specular);
	this->computeShader->setFloat("material.shininess", this->material->shininess);

	// Light
	if (GameManager::getDirectionalLight()) {
		GameManager::getDirectionalLight()->setUniform(this->computeShader, glm::mat4(1.0f));
	}
	else { // let the shader know that a directional light doesn't exist
		this->shaderProgram->setBool("directionalLight.exists", false);
	}

	// Set textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->rayTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, this->noiseTexture);

	glBindImageTexture(0, this->renderedTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Compute
	glDispatchCompute(this->width, this->height, 1);


	// Render
	this->shaderProgram->use();
	// Uniforms

	// Set the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->renderedTexture);


	// actually draw
	glBindVertexArray(this->VAO);

	// Make sure the noise has finished generating
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	this->shaderProgram->unbind();

	// wireframe
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);*/

	// updateRayTexture();
}

void Raytracing::renderImGui() {
	// Raytracing Settings
	{
		static float verticalFov = Camera::verticalFov;
		static float oldVerticalFov = verticalFov;
		static float viewDistanceSlider = Camera::viewDistance;

		static float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
		static float oldAspectRatio = aspectRatio;

		static int width = this->width;
		static int height = this->height;
		static int oldWidth = width;
		static int oldHeight = height;

		static float voxelRes = this->voxelRes;

		ImGui::Begin("Raytracing Settings");

		ImGui::SliderFloat("Vertical FOV", &verticalFov, 1.0f, 179.0f);
		aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
		ImGui::Text("Horizontal Fov: %.3f", glm::degrees(2.0f * glm::atan(glm::tan(glm::radians(Camera::verticalFov) * 0.5f) * aspectRatio)));

		ImGui::SliderFloat("View distance", &viewDistanceSlider, 1.0f, 200.0f);
		ImGui::SliderFloat("Voxel resolution", &voxelRes, 0.01f, 20.0f);

		ImGui::SliderInt("Width", &width, 1, 2500);
		ImGui::SliderInt("Height", &height, 1, 1000);

		ImGui::End();

		Camera::verticalFov = verticalFov;
		Camera::viewDistance = viewDistanceSlider; 
		this->viewDistance = viewDistanceSlider;  this->viewDistanceSqrd = viewDistanceSlider * viewDistanceSlider;

		this->voxelRes = voxelRes;

		this->width = width;
		this->height = (int)(width / aspectRatio);
		height = this->height;

		// Check for changes
		if (width != oldWidth || height != oldHeight || glm::abs(oldVerticalFov - verticalFov) > 0.1f || glm::abs(oldAspectRatio - aspectRatio) > 0.1f) {
			oldWidth = width;
			oldHeight = height;
			oldVerticalFov = verticalFov;
			oldAspectRatio = aspectRatio;

			updateRayTexture();
		}
	}
	// Noise settings
	{
		ImGui::Begin("Noise Settings");

		static int seedSlider = this->seed;
		static int oldSeed = this->seed;

		ImGui::SliderInt("Seed", &seedSlider, 0, 1000000);

		glm::ivec3 playerPos = glm::round(GameManager::getPlayer()->transform->getPosition());
		ImGui::Text(("Coordinates: (" + std::to_string(playerPos.x) + ", " + std::to_string(playerPos.y) + ", " + std::to_string(playerPos.z) + ")").c_str());

		ImGui::End();

		if (seedSlider != oldSeed) {
			oldSeed = seedSlider;
			this->seed = seedSlider;

			generateNoise();
		}
	}
}
