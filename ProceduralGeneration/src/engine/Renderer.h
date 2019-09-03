#pragma once

#include "Shader.h"

#include <vector>

namespace Engine {
	class Renderer {
	protected:
		Shader* shaderProgram = nullptr;
	public:
		Renderer(std::vector<unsigned int>& shaders) { 
			shaderProgram = new Shader(shaders); 
		}
		virtual ~Renderer() {
			// cleanup
			delete shaderProgram;
		}

		virtual void Render() = 0;
	};
}
