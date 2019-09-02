#pragma once

namespace Engine {
	// Texture2D is able to store and configure a texture in OpenGL.
	// It also hosts utility functions for easy management.
	class Texture2D {
	public:
		// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
		unsigned int ID;
		// Texture image dimensions
		unsigned int width, height; // Width and height of loaded image in pixels
		// Texture Format
		unsigned int internalFormat; // Format of texture object
		unsigned int imageFormat; // Format of loaded image
		// Texture configuration
		unsigned int wrap_S; // Wrapping mode on S axis
		unsigned int wrap_T; // Wrapping mode on T axis
		unsigned int filterMin; // Filtering mode if texture pixels < screen pixels
		unsigned int filterMax; // Filtering mode if texture pixels > screen pixels
		// Constructor (sets default texture modes)
		Texture2D();
		// Generates texture from image data
		void generate(unsigned int width, unsigned int height, unsigned char* data);
		// Binds the texture as the current active GL_TEXTURE_2D texture object
		void bind() const;
	};
}
