#pragma once

#include "Shader.h"

namespace Engine {
	class Renderer {
	private:
		Shader* shaderProgram = nullptr;
	public:
		Renderer() {};
		virtual ~Renderer() {
			// cleanup
			delete shaderProgram;
		}

		virtual void Render() = 0;
	};
}
