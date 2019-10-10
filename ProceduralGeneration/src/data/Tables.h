#pragma once

#include "glad/glad.h"

namespace ProceduralGeneration {
	class Tables {
	public:
		static unsigned char triangulationTable2D[256][16];
		static unsigned int* triangulationTable;
		static unsigned int textureTriangulationTableID;
		
		static int* permutations;
		
		Tables();
		~Tables();

		static void activateTriangulationTable(GLenum textureIndex);
	};
}
