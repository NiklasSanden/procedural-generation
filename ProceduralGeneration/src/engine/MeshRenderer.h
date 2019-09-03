#pragma once

#include "Renderer.h"

#include <vector>
#include <string>

namespace Engine {
	class MeshRenderer : public Renderer {
	public:
		MeshRenderer(std::vector<unsigned int> ids);
		void Render() override;
	};
}
