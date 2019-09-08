#include "Chunk.h"

#include "engine/Transform.h"
#include "engine/MeshRenderer.h"
#include "engine/ResourceManager.h"
#include "engine/Texture2D.h"
#include "engine/Shader.h"

#include "Camera.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include <string>

#include "engine/Debug.h"
using namespace ProceduralGeneration;

Chunk::Chunk(std::string& name) : GameObject(name) {
	// setup components
	this->transform = new Engine::Transform();

	// renderer
	std::vector<std::string> shaderFiles = { "chunk.vert", "chunk.frag" };
	std::string shaderProgramName = "ChunkShader";
	std::vector<glm::vec3> vertexArray{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 1.0f, -1.0f) };
	this->renderer = new Engine::MeshRenderer(shaderFiles, shaderProgramName, vertexArray);

	this->renderer->setupVertexArrayObject();
}

Chunk::~Chunk() {

}

void Chunk::awake() {

}

void Chunk::update(float deltaTime) {

}

void Chunk::fixedUpdate() {

}

void Chunk::render() {
	glm::mat4 modelMatrix = this->transform->getModelMatrix();
	this->renderer->render(modelMatrix, Camera::viewMatrix, Camera::projectionMatrix, this->transform->getNormalMatrix(modelMatrix, Camera::viewMatrix));
}

void Chunk::renderImGui() {

}
