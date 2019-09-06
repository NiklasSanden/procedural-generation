#pragma once

namespace Engine { class Shader; }

#include <vector>

namespace Engine {
	class Renderer {
	public:
		Shader* shaderProgram = nullptr;
		unsigned int VAO = 0;
		unsigned int VBO = 0;
	public:
		Renderer(Shader* shaderProgram) { 
			this->shaderProgram = shaderProgram;
		};
		virtual ~Renderer() {} // Does not need to delete shaderProgram as ResourceManager does that

		virtual void Render() {};
	};
}
