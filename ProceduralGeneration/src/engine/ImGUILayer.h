#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace Engine {
	class ImGuiLayer {
	public:
		ImGuiLayer(GLFWwindow* window);
		~ImGuiLayer();

		void begin();
		void end();
	private:
		float time = 0.0f;
	};
}
