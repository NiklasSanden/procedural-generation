#pragma once

#include "Renderer.h"
#include "Shader.h"

#include <vector>
#include <string>

namespace Engine {
	class MeshRenderer : public Renderer {
	public:
		MeshRenderer(Shader* shaderProgram);
		void Render() override;
	};
}
