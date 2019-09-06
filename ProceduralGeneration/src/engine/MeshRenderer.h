#pragma once

namespace Engine { class Shader; }

#include "Renderer.h"

#include <vector>
#include <string>

namespace Engine {
	class MeshRenderer : public Renderer {
	public:
		MeshRenderer(Shader* shaderProgram);
		void Render() override;
	};
}
