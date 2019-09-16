#include "MarchingCubesManager.h"

#include "engine/Game.h"
#include "engine/ResourceManager.h"
#include "engine/Material.h"
#include "engine/Transform.h"
#include "engine/Shader.h"
#include "engine/Lights.h"
#include "engine/MeshRenderer.h"
#include "engine/Program.h"
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
	//double tempTime = glfwGetTime();
	Camera* player = GameManager::getPlayer();

	glm::vec3 playerPosition = player->transform->getPosition();

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

	// Used by all chunks to determine if it can be seen or not
	// calculate horizontal fov from vertical: https://www.gamedev.net/forums/topic/361241-horizontal-fov/
	float verticalFov = glm::radians(Camera::verticalFov);
	float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
	float horizontalFov = 2.0f * glm::atan(glm::tan(verticalFov * 0.5f) * aspectRatio);
	glm::vec3 rightProjectionNormal = glm::normalize(glm::cross(player->transform->getUp(), glm::angleAxis(-horizontalFov, player->transform->getUp()) * -player->transform->getDirection()));
	glm::vec3 leftProjectionNormal = glm::normalize(glm::cross(glm::angleAxis(horizontalFov, player->transform->getUp()) * -player->transform->getDirection(), player->transform->getUp()));

	glm::vec3 upProjectionNormal = glm::normalize(glm::cross(glm::angleAxis(verticalFov, player->transform->getRight()) * -player->transform->getDirection(), player->transform->getRight()));
	glm::vec3 downProjectionNormal = glm::normalize(glm::cross(player->transform->getRight(), glm::angleAxis(-verticalFov, player->transform->getRight()) * -player->transform->getDirection()));

	// Loop through all of the chunks that could be in range
	for (int y = chunksInRangeDown; y <= chunksInRangeUp; y++) {
		for (int x = chunksInRangeLeft; x <= chunksInRangeUp; x++) {
			for (int z = chunksInRangeFront; z <= chunksInRangeBack; z++) {

				glm::vec3 currentChunkPos = glm::vec3(roundedPlayerPosition.x + x, roundedPlayerPosition.y + y, roundedPlayerPosition.z + z);
				glm::vec3 currentChunkWorldPos = currentChunkPos * this->chunkLength;

				if (currentChunkPos.x != 0 || currentChunkPos.y != 0 || currentChunkPos.z != 0) continue;
				if (glm::length2(currentChunkWorldPos - playerPosition) <= (this->viewDistance + this->chunkDistaceToCorner) * (this->viewDistance + this->chunkDistaceToCorner)) {
					// Check to see if possible that the chunk might be seen by the camera
					// check 1: behind
					glm::vec3 pointInView = playerPosition + -player->transform->getDirection() * glm::max(this->viewDistance, 1.0f);
					glm::vec3 positionInChunk = currentChunkWorldPos;
					positionInChunk.x = positionInChunk.x < pointInView.x ? glm::min(positionInChunk.x + this->chunkLength / 2.0f, pointInView.x) : glm::max(positionInChunk.x - this->chunkLength / 2.0f, pointInView.x);
					positionInChunk.y = positionInChunk.y < pointInView.y ? glm::min(positionInChunk.y + this->chunkLength / 2.0f, pointInView.y) : glm::max(positionInChunk.y - this->chunkLength / 2.0f, pointInView.y);
					positionInChunk.z = positionInChunk.z < pointInView.z ? glm::min(positionInChunk.z + this->chunkLength / 2.0f, pointInView.z) : glm::max(positionInChunk.z - this->chunkLength / 2.0f, pointInView.z);
					if (glm::dot(positionInChunk - playerPosition, -player->transform->getDirection()) <= 0.0f) {
						continue;
					}

					// check 2: fov angle (more expensive)
					//glm::vec4 chunkMiddle = glm::vec4(currentChunkWorldPos, 1.0f);
					glm::vec3 playerToChunkPos = currentChunkPos - playerPosition;
					float rightProjectionDot = glm::dot(rightProjectionNormal, playerToChunkPos);
					float leftProjectionDot = glm::dot(leftProjectionNormal, playerToChunkPos);
					float upProjectionDot = glm::dot(upProjectionNormal, playerToChunkPos);
					float downProjectionDot = glm::dot(downProjectionNormal, playerToChunkPos);
					
					/*chunkMiddle = Camera::projectionMatrix * Camera::viewMatrix * chunkMiddle;
					chunkMiddle.x /= chunkMiddle.w;
					chunkMiddle.y /= chunkMiddle.w;*/
					//std::cout << chunkMiddle.x << " - " << newMarginScaleX << ", " << chunkMiddle.y << " - " << newMarginScaleY << std::endl;
					if (rightProjectionDot + this->chunkDistaceToCorner < 0.0f ||
						leftProjectionDot + this->chunkDistaceToCorner < 0.0f ||
						upProjectionDot + this->chunkDistaceToCorner < 0.0f ||
						downProjectionDot + this->chunkDistaceToCorner < 0.0f) {
						continue;
					}
					/*if (glm::length2(currentChunkWorldPos - playerPosition) > this->chunkDistanceToCornerSqrd && 
						(chunkMiddle.x < -1.0f - newMarginScaleX || chunkMiddle.x > 1.0f + newMarginScaleX || chunkMiddle.y < -1.0f - newMarginScaleY || chunkMiddle.y > 1.0f + newMarginScaleY)) {
						continue;
					}*/
					// -------------------------------------------------------------------

					this->chunkPositionVectors.push_back(currentChunkWorldPos.x);
					this->chunkPositionVectors.push_back(currentChunkWorldPos.y);
					this->chunkPositionVectors.push_back(currentChunkWorldPos.z);
				}
			}
		}
	}

	//std::cout << (glfwGetTime() - tempTime) * 1000.0f << std::endl;
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

		std::string chunkInfo = "Visible chunks: " + std::to_string(this->chunkPositionVectors.size() / 3) + " - Amount of cubes: " + std::to_string(this->chunkPositionVectors.size() / 3 * this->cellPositionVectors.size() / 3);
		ImGui::Text(chunkInfo.c_str());

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
