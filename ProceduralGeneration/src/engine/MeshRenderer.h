#pragma once

#include "Renderer.h"

#include <vector>
#include <string>

namespace Engine {
	class MeshRenderer : public Renderer {
	public:
		MeshRenderer(std::vector<std::string> shaders);
		void Render() override;
	};
}
