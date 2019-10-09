#pragma once

#include "Noise.h"

#include "glad/glad.h"

#include "glm/glm.hpp"

#include <vector>

namespace ProceduralGeneration {
	class NoiseTexture3D {
	public:
		unsigned int ID;

		NoiseTexture3D(int width, int height, int depth, const std::vector<float>& data) {
			createTexture(width, height, depth, data);
		}
		NoiseTexture3D(const glm::vec3& position, int width, float cellLength) {
			std::vector<float> data;
			data.reserve((long long)width * width * width);

			for (int x = 0; x < width; x++) {
				for (int y = 0; y < width; y++) {
					for (int z = 0; z < width; z++) {
						float xFloat = x - (width - 1) / 2.0f; xFloat *= cellLength;
						float yFloat = y - (width - 1) / 2.0f; yFloat *= cellLength;
						float zFloat = z - (width - 1) / 2.0f; zFloat *= cellLength;

						data.push_back(Noise::octavePerlin(xFloat + position.x, yFloat + position.y, zFloat + position.z, 1, 0.5));
					}
				}
			}

			createTexture(width, width, width, data);
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

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
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
