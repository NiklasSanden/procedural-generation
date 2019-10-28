#pragma once

namespace Engine {
	class Shader;
}

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <thread>
#include <atomic>

namespace ProceduralGeneration {
	class ComputeMCChunk {
	private:
		unsigned int VAO;
		unsigned int VBOPos;
		unsigned int VBONormals;
		unsigned int tempVBOPos;
		unsigned int tempVBONormals;
		unsigned int atomicCounter;

		unsigned int amountOfVertices = 0;

		unsigned int noiseTextureID = 0;
		Engine::Shader* noiseComputeShader = nullptr;
		Engine::Shader* mcComputeShader = nullptr;

		bool hasSetup = false;

	public:
		ComputeMCChunk(const std::string& _name, const glm::vec3& _coordinates, unsigned int _seed);
		~ComputeMCChunk();

		std::string name;
		glm::vec3 coordinates;
		unsigned int seed;

		// settings
		float surfaceLevel = 0.0f; // 0.6f

		bool draw(unsigned int shaderID);
		
		void generateChunk(float chunkLength, int cellsPerAxis);
	private:
		void generateNoise(int cellsPerAxis, float cellLength, const glm::vec3& position);
		void generateMesh(int cellsPerAxis, float cellLength, const glm::vec3& position);
	};
}
