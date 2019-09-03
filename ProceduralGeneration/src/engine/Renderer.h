#pragma once

#include "Shader.h"

#include <vector>

namespace Engine {
	class Renderer {
	public:
		Shader* shaderProgram = nullptr;
		// VAO and buffers
		unsigned int VAO = 0;
		unsigned int VBO = 0;
	public:
		Renderer(std::vector<unsigned int> ids) { 
			this->shaderProgram = new Shader(ids);
		};
		virtual ~Renderer() {
			// cleanup
			delete shaderProgram;
		}

		virtual void Render() = 0;
	};
}
