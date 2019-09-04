#include "ResourceManager.h"
#include "Texture2D.h"
#include "Shader.h"
#include "ShaderManager.h"

#include "stb_image.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace Engine;

// Instantiate static variables
std::map<std::string, Texture2D*> ResourceManager::textures;
std::map<std::string, Shader*>	  ResourceManager::shaderPrograms;

Texture2D* ResourceManager::loadTexture(const char* file, bool alpha, std::string name) {
	std::string filePath = "res/textures/" + std::string(file);
	textures[name] = loadTextureFromFile(filePath.c_str(), alpha);
	return textures[name];
}

Texture2D* ResourceManager::getTexture(std::string name) {
	return textures[name];
}

Shader* ResourceManager::createShaderProgram(std::vector<std::string> files, std::string name) {
	// If a shaderProgram with that name doesn't exist
	if (shaderPrograms.find(name) == shaderPrograms.end()) {
		// Get all of the shader IDs
		std::vector<unsigned int> shaderIDs(files.size());
		for (int i = 0; i < files.size(); i++) {
			shaderIDs[i] = ShaderManager::getShaderID(files[i]);
		}

		Shader* shaderProgram = new Shader(shaderIDs);
		shaderPrograms[name] = shaderProgram;
	}
	// We return this either way
	return shaderPrograms[name];
}

Shader* ResourceManager::getShaderProgram(std::string name) {
	return shaderPrograms[name];
}

void ResourceManager::cleanup() {
	// (Properly) delete all textures
	// The destructors will call glDeleteTextures(1, &this->ID);
	for (auto texture : textures) {
		std::cout << "Deleting texture: " << texture.first << std::endl;
		delete texture.second;
	}
	textures.clear();

	// Delete all shaderPrograms
	for (auto shaderProgram : shaderPrograms) {
		std::cout << "Deleting shaderProgram: " << shaderProgram.first << std::endl;
		delete shaderProgram.second;
	}
	shaderPrograms.clear();
}

Texture2D* ResourceManager::loadTextureFromFile(const char* file, bool alpha) {
	// Make sure to flip them
	stbi_set_flip_vertically_on_load(true);
	// Create Texture object
	Texture2D* texture = new Texture2D();
	if (alpha)
	{
		texture->internalFormat = GL_RGBA;
		texture->imageFormat = GL_RGBA;
	}

	// Load image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	// Now generate texture
	if (data) {
		texture->generate(width, height, data);
		// And finally free image data
		stbi_image_free(data);
	}
	else { // Failed to read the image
		std::cout << "stb failed to read image: " << file << std::endl;
	}
	return texture;
}
