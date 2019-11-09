#pragma once

#include "glad/glad.h"

#include <vector>

namespace ProceduralGeneration {
	struct VertexBuffer {
		GLuint VAO = 0;
		GLuint VBO = 0;

		int amountOfVertices = 0;
		int LOD = 0;
		double timeOnCreation = 0.0;

	public:
		VertexBuffer() {
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);

			glBindVertexArray(this->VAO);

			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		~VertexBuffer() {
			glDeleteVertexArrays(1, &this->VAO);
			glDeleteBuffers(1, &this->VBO);
		}

		void draw() {
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(this->amountOfVertices));
		}
	};

	struct VBOManager {
		int size;
		int end;
		std::vector<VertexBuffer*> vertexBuffers;

		VBOManager(int _size) {
			size = _size;
			end = size;

			vertexBuffers = std::vector<VertexBuffer*>(_size);
			for (int i = 0; i < _size; i++) {
				vertexBuffers[i] = new VertexBuffer();
			}
		}
		~VBOManager() {
			for (int i = 0; i < end; i++) {
				delete vertexBuffers[i];
			}
		}

		VertexBuffer* getUnoccupiedVBO() {
			end--;
			return vertexBuffers[end];
		}

		void storeUnoccupiedVBO(VertexBuffer* VBO) {
			vertexBuffers[end] = VBO;
			end++;
		}
	};
}
