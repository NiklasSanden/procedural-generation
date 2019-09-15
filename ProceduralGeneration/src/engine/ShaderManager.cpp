#include "ShaderManager.h"
#include "shaders/ExtractSourceCode/extractSourceCode.h"

#include "glad/glad.h"

#include <unordered_map>
#include <string>
#include <iostream>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "Debug.h"
using namespace Engine;

ShaderManager::ShaderManager() {
	this->sourceCode = { "#version 450 core\nout vec4 FragColour;\n\nin vec3 FragPosView;\nin vec3 Normal;\n\nstruct Material {\n	vec3 diffuse;   // diffuse and ambient is the same on the material\n	vec3 specular;  \n	vec3 emission;  // the diffuse, specular, and emission can be turned into textures for more detail\n	float shininess;\n};\nuniform Material material;\n\nstruct DirectionalLight {\n	bool exists;\n    vec3 direction; // this is in viewcoordinates\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \nuniform DirectionalLight directionalLight;\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);\n\nvoid main()\n{\n    vec3 normal = normalize(Normal);\n    vec3 viewDirection = normalize(-FragPosView);\n\n    // phase 1: Directional lighting\n    vec3 result = vec3(0.0, 0.0, 0.0);\n	if (directionalLight.exists) {\n		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);\n	}\n    \n	// emission\n	result += material.emission;\n\n    FragColour = vec4(result, 1.0);\n}\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {\n	vec3 lightDirection = normalize(-light.direction);\n	// diffuse\n	float difference = max(dot(normal, lightDirection), 0.0);\n	// specular\n	vec3 reflectDirection = reflect(-lightDirection, normal);\n	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n	// combine\n	vec3 ambient  = light.ambient * material.diffuse; \n	vec3 diffuse  = light.diffuse * difference * material.diffuse;\n	vec3 specular = light.specular * spec * material.specular;\n\n	return (ambient + diffuse + specular);\n}\n", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aNormal;\n\nout vec3 FragPosView;\nout vec3 Normal;\n\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat3 normal;\n\nvoid main()\n{\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n\n	FragPosView = vec3(view * model * vec4(aPos, 1.0));\n	Normal = normal * aNormal;\n}\n", "#version 450 core\nout vec4 FragColour;\n\nin vec3 FragPosView;\nin vec3 Normal;\n\nstruct Material {\n	vec3 diffuse;   // diffuse and ambient is the same on the material\n	vec3 specular;  \n	vec3 emission; // the diffuse, specular, and emission can be turned into textures for more detail\n	float shininess;\n};\nuniform Material material;\n\nstruct DirectionalLight {\n	bool exists;\n    vec3 direction; // this is in viewcoordinates\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \nuniform DirectionalLight directionalLight;\n\nstruct PointLight {    \n	bool exists;\n    vec3 position; // this is in viewcoordinates\n    \n    float constant;\n    float linear;\n    float quadratic;  \n\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \n// Calculate the four closest point lights\n#define NR_POINT_LIGHTS 4\nuniform PointLight pointLights[NR_POINT_LIGHTS];\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);  \n\nvoid main()\n{\n    vec3 normal = normalize(Normal);\n    vec3 viewDirection = normalize(-FragPosView);\n\n    // phase 1: Directional lighting\n    vec3 result = vec3(0.0, 0.0, 0.0);\n	if (directionalLight.exists) {\n		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);\n	}\n    // phase 2: Point lights\n	for (int i = 0; i < NR_POINT_LIGHTS; i++) {\n		if (pointLights[i].exists) {\n			result += CalculatePointLight(pointLights[i], normal, FragPosView, viewDirection);    \n		}\n	}\n    // phase 3: Spot light\n    // TODO: result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    \n    \n	// emission\n	result += material.emission;\n\n    FragColour = vec4(result, 1.0);\n}\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {\n	vec3 lightDirection = normalize(-light.direction);\n	// diffuse\n	float difference = max(dot(normal, lightDirection), 0.0);\n	// specular\n	vec3 reflectDirection = reflect(-lightDirection, normal);\n	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n	// combine\n	vec3 ambient  = light.ambient * material.diffuse; \n	vec3 diffuse  = light.diffuse * difference * material.diffuse;\n	vec3 specular = light.specular * spec * material.specular;\n\n	return (ambient + diffuse + specular);\n}\n\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)\n{\n    vec3 lightDirection = normalize(light.position - fragPos);\n    // diffuse\n    float difference = max(dot(normal, lightDirection), 0.0);\n    // specular\n    vec3 reflectDirection = reflect(-lightDirection, normal);\n    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n    // attenuation\n    float distance    = length(light.position - fragPos);\n    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n    // combine\n    vec3 ambient  = light.ambient  * material.diffuse;\n    vec3 diffuse  = light.diffuse  * difference * material.diffuse;\n    vec3 specular = light.specular * spec * material.specular;\n    ambient  *= attenuation;\n    diffuse  *= attenuation;\n    specular *= attenuation;\n\n    return (ambient + diffuse + specular);\n} \n", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aNormal;\n\nout vec3 FragPosView;\nout vec3 Normal;\n\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat3 normal;\n\nvoid main()\n{\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n\n	FragPosView = vec3(view * model * vec4(aPos, 1.0));\n	Normal = normal * aNormal;\n}", "#version 450 core\nout vec4 FragColour;\n\nin vec3 FragPosView;\nin vec3 Normal;\n\nstruct Material {\n	vec3 diffuse;   // diffuse and ambient is the same on the material\n	vec3 specular;  \n	vec3 emission; // the diffuse, specular, and emission can be turned into textures for more detail\n	float shininess;\n};\nuniform Material material;\n\nstruct DirectionalLight {\n	bool exists;\n    vec3 direction; // this is in viewcoordinates\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \nuniform DirectionalLight directionalLight;\n\nstruct PointLight {    \n	bool exists;\n    vec3 position; // this is in viewcoordinates\n    \n    float constant;\n    float linear;\n    float quadratic;  \n\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \n// Calculate the four closest point lights\n#define NR_POINT_LIGHTS 4\nuniform PointLight pointLights[NR_POINT_LIGHTS];\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);  \n\nvoid main()\n{\n    vec3 normal = normalize(Normal);\n    vec3 viewDirection = normalize(-FragPosView);\n\n    // phase 1: Directional lighting\n    vec3 result = vec3(0.0, 0.0, 0.0);\n	if (directionalLight.exists) {\n		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);\n	}\n    // phase 2: Point lights\n	for (int i = 0; i < NR_POINT_LIGHTS; i++) {\n		if (pointLights[i].exists) {\n			result += CalculatePointLight(pointLights[i], normal, FragPosView, viewDirection);    \n		}\n	}\n    // phase 3: Spot light\n    // TODO: result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    \n    \n	// emission\n	result += material.emission;\n\n    FragColour = vec4(result, 1.0);\n}\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {\n	vec3 lightDirection = normalize(-light.direction);\n	// diffuse\n	float difference = max(dot(normal, lightDirection), 0.0);\n	// specular\n	vec3 reflectDirection = reflect(-lightDirection, normal);\n	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n	// combine\n	vec3 ambient  = light.ambient * material.diffuse; \n	vec3 diffuse  = light.diffuse * difference * material.diffuse;\n	vec3 specular = light.specular * spec * material.specular;\n\n	return (ambient + diffuse + specular);\n}\n\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)\n{\n    vec3 lightDirection = normalize(light.position - fragPos);\n    // diffuse\n    float difference = max(dot(normal, lightDirection), 0.0);\n    // specular\n    vec3 reflectDirection = reflect(-lightDirection, normal);\n    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n    // attenuation\n    float distance    = length(light.position - fragPos);\n    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n    // combine\n    vec3 ambient  = light.ambient  * material.diffuse;\n    vec3 diffuse  = light.diffuse  * difference * material.diffuse;\n    vec3 specular = light.specular * spec * material.specular;\n    ambient  *= attenuation;\n    diffuse  *= attenuation;\n    specular *= attenuation;\n\n    return (ambient + diffuse + specular);\n} \n", "#version 450 core\nlayout (points) in;\nlayout (triangle_strip, max_vertices = 100) out;\n\nout vec3 FragPosView;\nout vec3 Normal;\n\nin VS_OUT {\n	vec3 chunkPosition;\n} gs_in[];\n\nuniform float cellLength;\n\n// no model matrix necessary\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat3 normal;\n\nvoid main() {\n	const float cellLengthFactor = 0.5;\n	vec3 chunkPosition = gs_in[0].chunkPosition;\n	vec3 cellPosition = vec3(gl_in[0].gl_Position);\n\n	// Front\n	Normal = normal * vec3(0.0, 0.0, 1.0);\n\n	vec3 vertexPosition = vec3(0.0, 0.0, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, 0.0, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, cellLength * cellLengthFactor, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, cellLength * cellLengthFactor, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	EndPrimitive();\n\n	// Right\n	Normal = normal * vec3(1.0, 0.0, 0.0);\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, 0.0, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, 0.0, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, cellLength * cellLengthFactor, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, cellLength * cellLengthFactor, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	EndPrimitive();\n\n	// Back\n	Normal = normal * vec3(0.0, 0.0, -1.0);\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, 0.0, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, 0.0, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, cellLength * cellLengthFactor, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, cellLength * cellLengthFactor, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	EndPrimitive();\n\n	// Left\n	Normal = normal * vec3(-1.0, 0.0, 0.0);\n\n	vertexPosition = vec3(0.0, 0.0, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, 0.0, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, cellLength * cellLengthFactor, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, cellLength * cellLengthFactor, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	EndPrimitive();\n\n	// Top\n	Normal = normal * vec3(0.0, 1.0, 0.0);\n\n	vertexPosition = vec3(0.0, cellLength * cellLengthFactor, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, cellLength * cellLengthFactor, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, cellLength * cellLengthFactor, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, cellLength * cellLengthFactor, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	EndPrimitive();\n\n	// Bottom\n	Normal = normal * vec3(0.0, 0.0, -1.0);\n\n	vertexPosition = vec3(0.0, 0.0, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, 0.0, -cellLength * cellLengthFactor);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(0.0, 0.0, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	vertexPosition = vec3(cellLength * cellLengthFactor, 0.0, 0.0);\n	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n	EmitVertex();\n\n	EndPrimitive();\n\n\n	//FragPosView = vec3(view * model * vec4(aPos, 1.0));\n	//Normal = normal * aNormal;\n}\n", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aChunkPosition;\n\nout VS_OUT{\n	vec3 chunkPosition;\n} vs_out;\n\nvoid main() {\n    gl_Position = vec4(aPos, 1.0);\n	vs_out.chunkPosition = aChunkPosition;\n}\n", "#version 450 core\nout vec4 FragColour;\n\nin vec2 texCoord;\n\n// textures\nuniform sampler2D texture0;\n\nvoid main()\n{\n	FragColour = texture(texture0, texCoord);\n}", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 texCoord;\n\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat4 normal;\n\nvoid main()\n{\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n	texCoord = aTexCoord;\n}" };
	this->fileNames = { "chunk.frag", "chunk.vert", "default.frag", "default.vert", "marchingCubes.frag", "marchingCubes.geom", "marchingCubes.vert", "textureDefault.frag", "textureDefault.vert" };
}

std::unordered_map<std::string, unsigned int> ShaderManager::shaderIndex;

void ShaderManager::setupShaderSource() {
#ifdef CFG_DEBUG
	// extract source code and save it to the sourceCode.txt file
	// which will be used in release
	extractSourceCode();

	
	std::string path = "src/shaders";

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_directory()) continue;

		std::string shaderCode;
		std::ifstream shaderFile;
		
		// ensure ifstream objects can throw exceptions:
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
		try {
			shaderFile.open(entry.path());
			std::stringstream shaderStream;

			shaderStream << shaderFile.rdbuf();

			shaderFile.close();

			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure e) {
			LogManager::LogError("Shader file not successfully read");
		}

		// get the name of the file
		std::string entryPath = entry.path().string();
		int index = (int)entryPath.size() - 1;
		for (; index >= 0; index--) {
			if (entryPath[index] == '\\' || entryPath[index] == '/') {
				index++;
				break;
			}
		}
		std::string name = entryPath.substr(index);

		compileShader(shaderCode, name);
	}
#else
	// Create the instance so that the shader strings are loaded into memory
	ShaderManager instance;
	for (int i = 0; i < instance.fileNames.size(); i++) {
		compileShader(instance.sourceCode[i], instance.fileNames[i]);
	}
#endif
}

unsigned int ShaderManager::getShaderID(const std::string& shaderName) {
	return shaderIndex[shaderName];
}

void ShaderManager::compileShader(const std::string& shaderCode, const std::string& name) {
	LogManager::Log("Compiling shader: " + name);

	// the last 4 letters
	std::string extension = name.substr(name.size() - 4);
	
	// glsl, frag, vert, geom, comp, tese, tesc
	if		(extension == "vert") shaderIndex[name] = glCreateShader(GL_VERTEX_SHADER);
	else if (extension == "frag") shaderIndex[name] = glCreateShader(GL_FRAGMENT_SHADER);
	else if (extension == "geom") shaderIndex[name] = glCreateShader(GL_GEOMETRY_SHADER);
	else if (extension == "comp") shaderIndex[name] = glCreateShader(GL_COMPUTE_SHADER);
	else if (extension == "tese") shaderIndex[name] = glCreateShader(GL_TESS_EVALUATION_SHADER);
	else if (extension == "tesc") shaderIndex[name] = glCreateShader(GL_TESS_CONTROL_SHADER);

	const char* shaderCodeC = shaderCode.c_str();
	glShaderSource(shaderIndex[name], 1, &shaderCodeC, NULL);
	glCompileShader(shaderIndex[name]);
	
	checkCompileErrors(shaderIndex[name], name);
}

void ShaderManager::checkCompileErrors(unsigned int shader, const std::string& name) {
	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		LogManager::LogError("Failed to compile file: " + name + "\n" + std::string(infoLog) + "\n -- --------------------------------------------------- -- ");
	}
}

void ShaderManager::cleanup() {
	for (const auto& index : shaderIndex) {
		LogManager::Log("Deleting shaders: " + index.first);
		glDeleteShader(index.second);
	}
}