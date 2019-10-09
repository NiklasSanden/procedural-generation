#pragma once

struct GLFWwindow;

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
