#pragma once

namespace Engine {
	// Texture2D is able to store and configure a texture in OpenGL.
	// It also hosts utility functions for easy management.
	class Texture2D {
	public:
		unsigned int ID;
		unsigned int width, height;
		// Texture Format
		unsigned int internalFormat;
		unsigned int imageFormat;
		// Texture configuration
		unsigned int wrap_S; 
		unsigned int wrap_T; 
		unsigned int filterMin; 
		unsigned int filterMax;
		
		Texture2D();
		~Texture2D();

		void generate(unsigned int width, unsigned int height, unsigned char* data);
		void bind() const;
	};
}
