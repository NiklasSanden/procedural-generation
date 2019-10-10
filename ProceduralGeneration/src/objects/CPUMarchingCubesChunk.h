#pragma once

#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace ProceduralGeneration {
	class CPUMarchingCubesChunk {
	private:
		unsigned int VAO;
		unsigned int VBOPos;
		unsigned int VBONormals;
		unsigned int EBO;

		bool isGenerated = false;
	public:
		CPUMarchingCubesChunk(const std::string& _name, const glm::vec3& _coordinates);
		~CPUMarchingCubesChunk();

		std::string name;
		glm::vec3 coordinates;
		std::vector<float> vertexPositions;
		std::vector<unsigned int> indexArray;
		std::vector<float> normals;

		// settings
		float surfaceLevel = 0.6f;

		void draw();
		
		void generateChunk(float chunkLength, int cellsPerAxis);
		void generateNoise(int cellsPerAxis, float cellLength, const glm::vec3& position, std::vector<std::vector<std::vector<float>>>& noise);
		void calculateNormals(const std::vector<int>& indices, const std::vector<glm::vec3>& marginVertices);
		glm::vec3 lerpVector(glm::vec3 a, glm::vec3 b, float aValue, float bValue, float surfaceLevel);
	};
}
