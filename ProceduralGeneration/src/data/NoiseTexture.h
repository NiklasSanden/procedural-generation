#pragma once

#include "glad/glad.h"

#include <vector>
#include <cstdlib> // Rand

namespace ProceduralGeneration {
	class NoiseTexture3D {
	public:
		unsigned int ID;

		NoiseTexture3D(int width, int height, int depth, const std::vector<float>& data) {
			createTexture(width, height, depth, data);
		}
		NoiseTexture3D(int width, int height, int depth) {
			std::vector<float> data;

			srand(static_cast<unsigned>(0));
			for (int i = 0; i < width; i++) {
				for (int j = 0; j < height; j++) {
					for (int k = 0; k < depth; k++) {
						data.push_back(static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
					}
				}
			}

			createTexture(width, height, depth, data);
		}
		~NoiseTexture3D() {
			glDeleteTextures(1, &ID);
		}

		void activate(GLenum textureIndex) {
			glActiveTexture(textureIndex);
			glBindTexture(GL_TEXTURE_3D, ID);
		}

		void createTexture(int width, int height, int depth, const std::vector<float>& data) {
			glGenTextures(1, &ID);
			glBindTexture(GL_TEXTURE_3D, ID);

			glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, width, height, depth, 0, GL_RED, GL_FLOAT, data.data());

			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glBindTexture(GL_TEXTURE_3D, 0);
		}
	};

	class NoiseTexture2D {
	public:
		unsigned int ID;

		NoiseTexture2D() {
			glGenTextures(1, &ID);
		}
		~NoiseTexture2D() {
			glDeleteTextures(1, &ID);
		}
	};

	class NoiseTexture1D {
	public:
		unsigned int ID;

		NoiseTexture1D() {
			glGenTextures(1, &ID);
		}
		~NoiseTexture1D() {
			glDeleteTextures(1, &ID);
		}
	};
}
