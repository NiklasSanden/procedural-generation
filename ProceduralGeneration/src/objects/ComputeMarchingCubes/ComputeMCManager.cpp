#include "objects/ComputeMarchingCubes/ComputeMCManager.h"
#include "objects/ComputeMarchingCubes/ComputeMCChunk.h"

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

ComputeMCManager::ComputeMCManager(const std::string& name) : GameObject(name) {
	// Shader program
	this->shaderProgram = Engine::ResourceManager::createShaderProgram({ "cpuMarchingCubes.vert", "cpuMarchingCubes.frag" }, "ComputeMCRenderShader");
	// Material
	this->material = new Engine::Material();
}

ComputeMCManager::~ComputeMCManager() {
	delete this->material;
	for (auto pair : this->generatedChunks) {
		delete pair.second;
	}
}

void ComputeMCManager::regenerateChunks() {
	for (auto pair : this->generatedChunks) {
		delete pair.second;
	}
	this->generatedChunks.clear();

	this->emptyChunks.clear();
}

void ComputeMCManager::update(float deltaTime) {
	//double tempTime = glfwGetTime();
	Camera* player = GameManager::getPlayer();


	// Clear old chunks
	this->activeChunks.clear();
	//std::unordered_map<std::string, ComputeMCChunk*> oldChunks = this->generatedChunks;
	//this->generatedChunks.clear();


	// Used by all chunks to determine if it can be seen or not
	// calculate horizontal fov from vertical: https://www.gamedev.net/forums/topic/361241-horizontal-fov/
	float halfVerticalFov = glm::radians(Camera::verticalFov / 2.0f);
	float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
	float halfHorizontalFov = 2.0f * glm::atan(glm::tan(glm::radians(Camera::verticalFov) * 0.5f) * aspectRatio) / 2.0f;
	glm::vec3 rightProjectionNormal = glm::normalize(glm::cross(player->transform->getUp(), glm::angleAxis(-halfHorizontalFov, player->transform->getUp()) * -player->transform->getDirection()));
	glm::vec3 leftProjectionNormal = glm::normalize(glm::cross(glm::angleAxis(halfHorizontalFov, player->transform->getUp()) * -player->transform->getDirection(), player->transform->getUp()));

	glm::vec3 upProjectionNormal = glm::normalize(glm::cross(glm::angleAxis(halfVerticalFov, player->transform->getRight()) * -player->transform->getDirection(), player->transform->getRight()));
	glm::vec3 downProjectionNormal = glm::normalize(glm::cross(player->transform->getRight(), glm::angleAxis(-halfVerticalFov, player->transform->getRight()) * -player->transform->getDirection()));

	float farthestViewDistanceFactor = (1.0f / glm::cos(halfHorizontalFov)) / glm::cos(halfVerticalFov);

	updateActiveChunks(this->chunkLength * 4, 4, Camera::viewDistance       , farthestViewDistanceFactor, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	updateActiveChunks(this->chunkLength * 2, 2, Camera::viewDistance / 3.0f, farthestViewDistanceFactor, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);
	updateActiveChunks(this->chunkLength    , 1, Camera::viewDistance / 6.0f, farthestViewDistanceFactor, rightProjectionNormal, leftProjectionNormal, upProjectionNormal, downProjectionNormal);


	this->oldActiveChunks.clear();
	for (auto chunk : this->activeChunks) {
		this->oldActiveChunks.insert(chunk.second->name);
	}

	/*for (auto chunk : oldChunks) {
		delete chunk.second;
	}*/

	//std::cout << (glfwGetTime() - tempTime) * 1000.0f << std::endl;
}

void ComputeMCManager::updateActiveChunks(float chunkLength, int LODIndex, float viewDistance, float farthestViewDistanceFactor, const glm::vec3& rightProjectionNormal, const glm::vec3& leftProjectionNormal, const glm::vec3& upProjectionNormal, const glm::vec3& downProjectionNormal) {
	Camera* player = GameManager::getPlayer();
	glm::vec3 playerPosition = player->transform->getPosition();
	glm::vec3 roundedPlayerPosition = glm::round(playerPosition / chunkLength);


	float chunkDistanceToCorner = glm::sqrt((chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f) + (chunkLength / 2.0f) * (chunkLength / 2.0f));
	// Since the view distance is how far away a plane is, we need the distance to the farthest point on that plane
	float farthestViewDistance = viewDistance * farthestViewDistanceFactor;
	float farthestViewDistanceMargin = (viewDistance + this->chunkLength) * farthestViewDistanceFactor;

	// Notice that left, down and front are negative. That is because they will be used in the for loop, and will be added to the roundedPlayerPosition
	int chunksInRangeUp = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(0.0f, 1.0f, 0.0f), playerPosition, farthestViewDistanceMargin, chunkLength, chunkDistanceToCorner);
	int chunksInRangeDown = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(0.0f, -1.0f, 0.0f), playerPosition, farthestViewDistanceMargin, chunkLength, chunkDistanceToCorner);
	int chunksInRangeRight = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(1.0f, 0.0f, 0.0f), playerPosition, farthestViewDistanceMargin, chunkLength, chunkDistanceToCorner);
	int chunksInRangeLeft = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(-1.0f, 0.0f, 0.0f), playerPosition, farthestViewDistanceMargin, chunkLength, chunkDistanceToCorner);
	int chunksInRangeFront = -chunksInRangeDirection(roundedPlayerPosition, glm::vec3(-1.0f, 0.0f, 0.0f), playerPosition, farthestViewDistanceMargin, chunkLength, chunkDistanceToCorner);
	int chunksInRangeBack = chunksInRangeDirection(roundedPlayerPosition, glm::vec3(1.0f, 0.0f, 0.0f), playerPosition, farthestViewDistanceMargin, chunkLength, chunkDistanceToCorner);

	// Loop through all of the chunks that could be in range
	for (int y = chunksInRangeDown; y <= chunksInRangeUp; y++) {
		for (int x = chunksInRangeLeft; x <= chunksInRangeRight; x++) {
			for (int z = chunksInRangeFront; z <= chunksInRangeBack; z++) {

				glm::vec3 currentChunkCoords = glm::vec3(roundedPlayerPosition.x + x, roundedPlayerPosition.y + y, roundedPlayerPosition.z + z);
				glm::vec3 currentChunkWorldPos = currentChunkCoords * chunkLength;
				std::string currentChunkName = std::to_string(currentChunkCoords.x) + ", " + std::to_string(currentChunkCoords.y) + ", " + std::to_string(currentChunkCoords.z) + ", " + std::to_string(LODIndex);

				float distanceToPlayerSqr = glm::length2(currentChunkWorldPos - playerPosition);

				if (this->emptyChunks.find(currentChunkName) != this->emptyChunks.end()) continue;

				float tempFarthestViewDistance = this->oldActiveChunks.find(currentChunkName) == this->oldActiveChunks.end() ? farthestViewDistance : farthestViewDistanceMargin;

				//if ((currentChunkCoords.x != 0 && currentChunkCoords.x != 1) || (currentChunkCoords.y != 0 && currentChunkCoords.y != 1) || (currentChunkCoords.z != 0 && currentChunkCoords.z != 1)) continue;
				//if (currentChunkCoords.x != 14 || currentChunkCoords.y != 0 || currentChunkCoords.z != 8) continue;
				if (distanceToPlayerSqr <= (tempFarthestViewDistance + chunkDistanceToCorner) * (tempFarthestViewDistance + chunkDistanceToCorner)) {
					// Check to see if possible that the chunk might be seen by the camera
					// check 1: behind
					glm::vec3 pointInView = playerPosition + -player->transform->getDirection() * glm::max(viewDistance, 1.0f);
					glm::vec3 positionInChunk = currentChunkWorldPos;
					positionInChunk.x = positionInChunk.x < pointInView.x ? glm::min(positionInChunk.x + chunkLength / 2.0f, pointInView.x) : glm::max(positionInChunk.x - chunkLength / 2.0f, pointInView.x);
					positionInChunk.y = positionInChunk.y < pointInView.y ? glm::min(positionInChunk.y + chunkLength / 2.0f, pointInView.y) : glm::max(positionInChunk.y - chunkLength / 2.0f, pointInView.y);
					positionInChunk.z = positionInChunk.z < pointInView.z ? glm::min(positionInChunk.z + chunkLength / 2.0f, pointInView.z) : glm::max(positionInChunk.z - chunkLength / 2.0f, pointInView.z);
					if (glm::dot(positionInChunk - playerPosition, -player->transform->getDirection()) <= 0.0f) {
						continue;
					}

					// check 2: View frustum culling (more expensive)
					glm::vec3 playerToChunkPos = currentChunkWorldPos - playerPosition;
					float rightProjectionDot = glm::dot(rightProjectionNormal, playerToChunkPos);
					float leftProjectionDot = glm::dot(leftProjectionNormal, playerToChunkPos);
					float upProjectionDot = glm::dot(upProjectionNormal, playerToChunkPos);
					float downProjectionDot = glm::dot(downProjectionNormal, playerToChunkPos);

					if (rightProjectionDot + chunkDistanceToCorner < 0.0f ||
						leftProjectionDot + chunkDistanceToCorner < 0.0f ||
						upProjectionDot + chunkDistanceToCorner < 0.0f ||
						downProjectionDot + chunkDistanceToCorner < 0.0f) {
						continue;
					}
					// -------------------------------------------------------------------

					if (generatedChunks.find(currentChunkName) == generatedChunks.end()) {
						this->generatedChunks[currentChunkName] = new ComputeMCChunk(currentChunkName, currentChunkCoords, this->seed);
						this->generatedChunks[currentChunkName]->generateChunk(chunkLength, this->cellsPerAxis);
					}
					this->activeChunks.insert(std::pair<float, ComputeMCChunk*>(distanceToPlayerSqr + (LODIndex - 1) * this->chunkLength * 2.0f, this->generatedChunks[currentChunkName]));
				}
			}
		}
	}
}

int ComputeMCManager::chunksInRangeDirection(const glm::vec3& startPosition, const glm::vec3& incrementVector, const glm::vec3& playerPosition, float farthestViewDistance, float chunkLength, float chunkDistanceToCorner) {
	glm::vec3 currentPosition = startPosition + incrementVector;

	int answer = 0;
	while (true) {
		if (glm::length2((currentPosition * chunkLength) - playerPosition) <= (farthestViewDistance + chunkDistanceToCorner) * (farthestViewDistance + chunkDistanceToCorner)) {
			answer++;
			currentPosition += incrementVector;
		}
		else {
			break;
		}
	}

	return answer;
}

void ComputeMCManager::render() {
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

	// Renderer stuff
	// Uniforms
	this->shaderProgram->setMat4("view", Camera::viewMatrix);
	this->shaderProgram->setMat4("projection", Camera::projectionMatrix);
	this->shaderProgram->setFloat("viewDistance", Camera::viewDistance);

	// material
	this->shaderProgram->setVec3("material.diffuse", this->material->diffuse);
	this->shaderProgram->setVec3("material.specular", this->material->specular);
	this->shaderProgram->setFloat("material.shininess", this->material->shininess);

	for (auto pair : this->activeChunks) {
		bool isEmpty = pair.second->draw(this->shaderProgram->ID);

		if (isEmpty) {
			std::string name = pair.second->name;
			this->emptyChunks.insert(name);
			delete this->generatedChunks[name];
			this->generatedChunks.erase(name);
		}
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->shaderProgram->unbind();

	// wireframe
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);*/

	//regenerateChunks();
}

void ComputeMCManager::renderImGui() {
	// Chunk settings
	{
		static float verticalFov = Camera::verticalFov;
		static float viewDistanceSlider = Camera::viewDistance;

		static int cellsPerAxisSlider = this->cellsPerAxis;
		static int oldCellsPerAxisSlider = cellsPerAxisSlider;

		static float chunkLengthSlider = this->chunkLength;
		static float oldChunkLengthSlider = chunkLengthSlider;

		static bool keepCellRatio = true;
		static float ratio = chunkLengthSlider / cellsPerAxisSlider;
		static bool oldKeepCellRatio = keepCellRatio;

		ImGui::Begin("Chunk Settings");

		ImGui::SliderFloat("Vertical FOV", &verticalFov, 1.0f, 179.0f);
		float aspectRatio = (float)Engine::Program::SCREEN_WIDTH / Engine::Program::SCREEN_HEIGHT;
		ImGui::Text("Horizontal Fov: %.3f", glm::degrees(2.0f * glm::atan(glm::tan(glm::radians(Camera::verticalFov) * 0.5f) * aspectRatio)));

		ImGui::SliderFloat("View distance", &viewDistanceSlider, 1.0f, 500.0f);
		ImGui::SliderFloat("Chunk length", &chunkLengthSlider, 1.0f, 100.0f);
		ImGui::SliderInt("Cells per axis", &cellsPerAxisSlider, 1, 200);
		ImGui::Checkbox("Keep ratio", &keepCellRatio);

		ImGui::Text(("Visible chunks: " + std::to_string(this->activeChunks.size())).c_str());

		ImGui::End();


		Camera::verticalFov = verticalFov;
		Camera::viewDistance = viewDistanceSlider; this->viewDistanceSqrd = viewDistanceSlider * viewDistanceSlider;

		// Update cells if settings were changed
		if (cellsPerAxisSlider != oldCellsPerAxisSlider || chunkLengthSlider != oldChunkLengthSlider) {
			if (keepCellRatio) {
				if (cellsPerAxisSlider != oldCellsPerAxisSlider) {
					chunkLengthSlider = glm::clamp(cellsPerAxisSlider * ratio, 1.0f, 100.0f);
				}
				else {
					cellsPerAxisSlider = glm::clamp((int)(chunkLengthSlider / ratio), 1, 100);
				}
			}

			this->chunkLength = chunkLengthSlider;
			this->cellsPerAxis = cellsPerAxisSlider;

			this->chunkDistanceToCorner = glm::sqrt((this->chunkLength / 2.0f) * (this->chunkLength / 2.0f) + (this->chunkLength / 2.0f) * (this->chunkLength / 2.0f) + (this->chunkLength / 2.0f) * (this->chunkLength / 2.0f));
			this->chunkDistanceToCornerSqrd = this->chunkDistanceToCorner * this->chunkDistanceToCorner;

			// Now we need to regenerate the chunks
			regenerateChunks();

			oldCellsPerAxisSlider = cellsPerAxisSlider;
			oldChunkLengthSlider = chunkLengthSlider;
		}

		// Update ratio if keepRatio was turned on
		if (keepCellRatio != oldKeepCellRatio) {
			if (keepCellRatio) {
				ratio = chunkLengthSlider / cellsPerAxisSlider;
			}
			oldKeepCellRatio = keepCellRatio;
		}
	}
	// Noise settings
	{
		ImGui::Begin("Noise Settings");

		static int seedSlider = this->seed;
		static int oldSeedSlider = seedSlider;

		ImGui::SliderInt("Seed", &seedSlider, 0, 1000000);

		glm::ivec3 playerPos = glm::round(GameManager::getPlayer()->transform->getPosition());
		ImGui::Text(("Coordinates: (" + std::to_string(playerPos.x) + ", " + std::to_string(playerPos.y) + ", " + std::to_string(playerPos.z) + ")").c_str());

		ImGui::End();

		if (oldSeedSlider != seedSlider) {
			this->seed = (unsigned int)seedSlider;

			// Now we need to regenerate the chunks
			regenerateChunks();

			oldSeedSlider = seedSlider;
		}
	}
}
