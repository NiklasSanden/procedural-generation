#include "MarchingCubesManager.h"

#include "engine/Game.h"
#include "engine/ResourceManager.h"
#include "engine/Material.h"
#include "engine/Transform.h"
#include "engine/Shader.h"
#include "engine/Lights.h"
#include "engine/MeshRenderer.h"
#include "GameManager.h"
#include "Camera.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <vector>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

MarchingCubesManager::MarchingCubesManager(const std::string& name) : GameObject(name) {
	this->shaderProgram = Engine::ResourceManager::createShaderProgram({ "marchingCubes.vert", "marchingCubes.geom", "marchingCubes.frag" }, "MarchingCubesShader");
	this->material = new Engine::Material();

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->instancedVBO);

	// Setup attribute pointers
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, this->instancedVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// ----------------------

	calculateCellPositions();
}

MarchingCubesManager::~MarchingCubesManager() {
	delete this->material;

	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->instancedVBO);
}

void MarchingCubesManager::calculateCellPositions() {
	this->cellPositionVectors.clear();

	for (int x = 0; x < this->cellsPerAxis; x++) {
		for (int y = 0; y < this->cellsPerAxis; y++) {
			for (int z = 0; z < this->cellsPerAxis; z++) {
				float xFloat = x - this->cellsPerAxis / 2.0f;
				float yFloat = y - this->cellsPerAxis / 2.0f;
				float zFloat = -(z - this->cellsPerAxis / 2.0f); // invert since the positive z-axis is facing you - and we want to start in the bottom, left and back corner

				this->cellPositionVectors.push_back(xFloat * (this->chunkLength / (float)this->cellsPerAxis));
				this->cellPositionVectors.push_back(yFloat * (this->chunkLength / (float)this->cellsPerAxis));
				this->cellPositionVectors.push_back(zFloat * (this->chunkLength / (float)this->cellsPerAxis));
			}
		}
	}

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->cellPositionVectors.size() * sizeof(float), this->cellPositionVectors.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MarchingCubesManager::update(float deltaTime) {
	Camera* player = GameManager::getPlayer();

	glm::vec3 playerPosition = player->transform->getPosition();

	/*if (glm::length2(this->oldPlayerPosition - playerPosition) < this->updateChunksPlayerPositionThresholdSqrd) {
		return;
	}
	this->oldPlayerPosition = playerPosition;*/

	// Clear old chunks
	this->chunkPositionVectors.clear();

	glm::vec3 roundedPlayerPosition = glm::round(playerPosition / this->chunkLength);

	// Notice that left, down and front are negative. That is because they will be used in the for loop, and will be added to the roundedPlayerPosition
	int chunksInRangeUp = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(0.0f, 1.0f, 0.0f), playerPosition);
	int chunksInRangeDown = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(0.0f, -1.0f, 0.0f), playerPosition);
	int chunksInRangeRight = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(1.0f, 0.0f, 0.0f), playerPosition);
	int chunksInRangeLeft = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(-1.0f, 0.0f, 0.0f), playerPosition);
	int chunksInRangeFront = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(-1.0f, 0.0f, 0.0f), playerPosition);
	int chunksInRangeBack = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(1.0f, 0.0f, 0.0f), playerPosition);

	// Allocate memory (3 as a factor since we are storing the vec3s as floats)
	this->chunkPositionVectors.reserve((long long)3 * ((long long)chunksInRangeUp - chunksInRangeDown + 1) * ((long long)chunksInRangeRight - chunksInRangeLeft + 1) * ((long long)chunksInRangeBack - chunksInRangeFront + 1));

	// Loop through all of the chunks that could be in range
	for (int y = chunksInRangeDown; y <= chunksInRangeUp; y++) {
		for (int x = chunksInRangeLeft; x <= chunksInRangeUp; x++) {
			for (int z = chunksInRangeFront; z <= chunksInRangeBack; z++) {

				glm::vec3 currentChunkPos = glm::vec3(roundedPlayerPosition.x + x, roundedPlayerPosition.y + y, roundedPlayerPosition.z + z);
				glm::vec3 currentChunkWorldPos = currentChunkPos * this->chunkLength;

				if (glm::length2(currentChunkWorldPos - playerPosition) <= (this->viewDistance + this->chunkDistaceToCorner) * (this->viewDistance + this->chunkDistaceToCorner)) {

					// Check to see if possible that the chunk might be seen by the camera
					glm::vec3 margin = -player->transform->getDirection() * this->chunkDistaceToCorner;
					glm::vec3 difference = currentChunkWorldPos - playerPosition + margin;
					if (glm::dot(difference, -player->transform->getDirection()) <= 0.0f) {
						continue;
					}

					glm::vec3 chunkPositionWorld = currentChunkPos * this->chunkLength;
					this->chunkPositionVectors.push_back(chunkPositionWorld.x);
					this->chunkPositionVectors.push_back(chunkPositionWorld.y);
					this->chunkPositionVectors.push_back(chunkPositionWorld.z);
				}
			}
		}
	}

	//std::cout << "Visible chunks: " << std::to_string(this->chunkPositionVectors.size() / 3) << " - Amount of cubes: " << std::to_string(this->chunkPositionVectors.size() / 3 * this->cellPositionVectors.size() / 3) << std::endl;
}

int MarchingCubesManager::chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector, const glm::vec3& playerPosition) {
	glm::vec3 currentPosition = startPosition + incrementVector;

	int answer = 0;
	while (true) {
		if (glm::length2((currentPosition * this->chunkLength) - playerPosition) <= (this->viewDistance + this->chunkDistaceToCorner) * (this->viewDistance + this->chunkDistaceToCorner)) {
			answer++;
			currentPosition += incrementVector;
		}
		else {
			break;
		}
	}

	return answer;
}

void MarchingCubesManager::render() {
	// wireframe
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
	glDisable(GL_CULL_FACE);*/

	this->shaderProgram->use();
	// directional light
	if (GameManager::getDirectionalLight()) {
		GameManager::getDirectionalLight()->setUniform(this->shaderProgram, Camera::viewMatrix);
	}
	else { // let the shader know that a directional light doesn't exist
		this->shaderProgram->setBool("directionalLight.exists", false);
	}
	// point lights
	// we have to tell the shader that point lights don't exist
	for (int i = 0; i < 4; i++) {
		this->shaderProgram->setBool("pointLights[" + std::to_string(i) + "].exists", false);
	}


	// Renderer stuff
	// Uniforms
	this->shaderProgram->use();
	this->shaderProgram->setMat4("view", Camera::viewMatrix);
	this->shaderProgram->setMat4("projection", Camera::projectionMatrix);
	this->shaderProgram->setMat3("normal", this->transform->getNormalMatrix(glm::mat4(1.0f), Camera::viewMatrix));
	this->shaderProgram->setFloat("cellLength", this->chunkLength / this->cellsPerAxis);
	// material
	this->shaderProgram->setVec3("material.diffuse", this->material->diffuse);
	this->shaderProgram->setVec3("material.specular", this->material->specular);
	this->shaderProgram->setFloat("material.shininess", this->material->shininess);


	// Setup OpenGL objects
	glBindVertexArray(this->VAO);

	// the positions are done in calculateCellPositions() since they are static
	// ------------------------------------------------------------------------

	// instanced array
	glBindBuffer(GL_ARRAY_BUFFER, this->instancedVBO);
	glBufferData(GL_ARRAY_BUFFER, this->chunkPositionVectors.size() * sizeof(float), this->chunkPositionVectors.data(), GL_DYNAMIC_DRAW);

	// Draw instanced
	glDrawArraysInstanced(GL_POINTS, 0, (GLsizei)this->cellPositionVectors.size() / 3, (GLsizei)this->chunkPositionVectors.size() / 3);

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->shaderProgram->unbind();

	// wireframe
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);*/
}

void MarchingCubesManager::renderImGui() {
	{
		static float viewDistanceSlider = this->viewDistance;

		static int cellsPerAxisSlider = this->cellsPerAxis;
		static int oldCellsPerAxisSlider = cellsPerAxisSlider;

		static float chunkLengthSlider = this->chunkLength;
		static float oldChunkLengthSlider = chunkLengthSlider;

		ImGui::Begin("Settings");

		ImGui::SliderFloat("View distance", &viewDistanceSlider, 0.0f, 300.0f);
		ImGui::SliderFloat("Chunk length", &chunkLengthSlider, 1.0f, 100.0f);
		ImGui::SliderInt("Cells per axis", &cellsPerAxisSlider, 1, 100);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		
		this->viewDistance = viewDistanceSlider; this->viewDistanceSqrd = viewDistanceSlider * viewDistanceSlider;

		if (cellsPerAxisSlider != oldCellsPerAxisSlider || chunkLengthSlider != oldChunkLengthSlider) {
			this->chunkLength = chunkLengthSlider;
			this->cellsPerAxis = cellsPerAxisSlider;

			this->chunkDistaceToCorner = glm::sqrt((this->chunkLength / 2.0f) * (this->chunkLength / 2.0f) + (this->chunkLength / 2.0f) * (this->chunkLength / 2.0f) + (this->chunkLength / 2.0f) * (this->chunkLength / 2.0f));
			this->chunkDistanceToCornerSqrd = this->chunkDistaceToCorner * this->chunkDistaceToCorner;

			calculateCellPositions();

			oldCellsPerAxisSlider = cellsPerAxisSlider;
			oldChunkLengthSlider = chunkLengthSlider;
		}
	}
}
