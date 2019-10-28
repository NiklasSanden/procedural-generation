#pragma once

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <thread>
#include <atomic>

namespace ProceduralGeneration {
	class CPUMarchingCubesChunk {
	private:
		unsigned int VAO;
		unsigned int VBOPos;
		unsigned int VBONormals;
		unsigned int EBO;

		std::atomic<bool> isGenerated;
		bool isSetup = false;
		
		static std::atomic<int> numberOfGeneratingChunks;

		std::thread generator;
	public:
		CPUMarchingCubesChunk(const std::string& _name, const glm::vec3& _coordinates, unsigned int _seed);
		~CPUMarchingCubesChunk();

		std::string name;
		glm::vec3 coordinates;
		unsigned int seed;

		std::vector<float> vertexPositions;
		std::vector<unsigned int> indexArray;
		std::vector<float> normals;

		// settings
		float surfaceLevel = 0.6f; // 0.6f

		void draw(int visibleChunks);
		
		void generateChunk(float chunkLength, int cellsPerAxis);
	private:
		void generateChunkThread(float chunkLength, int cellsPerAxis);
		void generateChunkThreadCubes(float chunkLength, int cellsPerAxis);
		
		void generateNoise(int cellsPerAxis, float cellLength, const glm::vec3& position, std::vector<std::vector<std::vector<long double>>>& noise);

		void calculateNormals(const std::vector<int>& indices, const std::vector<glm::vec3>& marginVertices);
		
		glm::vec3 lerpVector(glm::vec3 a, glm::vec3 b, float aValue, float bValue, float surfaceLevel);
		float lerpFloat(float aValue, float bValue, float surfaceLevel);
	};
}
