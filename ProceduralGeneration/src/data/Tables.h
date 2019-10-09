#pragma once

#include "glad/glad.h"

namespace ProceduralGeneration {
	class Tables {
	public:
		static signed char* triangulationTable;
		static unsigned int textureTriangulationTableID;
		
		static int* permutations;
		
		Tables();
		~Tables();

		static void activateTriangulationTable(GLenum textureIndex);
	};
}
