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
	this->sourceCode = { "#version 450 core\nout vec4 FragColour;\n\nin vec3 FragPosView;\nin vec3 Normal;\n\nstruct Material {\n	vec3 diffuse;   // diffuse and ambient is the same on the material\n	vec3 specular;  \n	vec3 emission;  // the diffuse, specular, and emission can be turned into textures for more detail\n	float shininess;\n};\nuniform Material material;\n\nstruct DirectionalLight {\n	bool exists;\n    vec3 direction; // this is in viewcoordinates\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \nuniform DirectionalLight directionalLight;\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);\n\nvoid main()\n{\n    vec3 normal = normalize(Normal);\n    vec3 viewDirection = normalize(-FragPosView);\n\n    // phase 1: Directional lighting\n    vec3 result = vec3(0.0, 0.0, 0.0);\n	if (directionalLight.exists) {\n		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);\n	}\n    \n	// emission\n	result += material.emission;\n\n    FragColour = vec4(result, 1.0);\n}\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {\n	vec3 lightDirection = normalize(-light.direction);\n	// diffuse\n	float difference = max(dot(normal, lightDirection), 0.0);\n	// specular\n	vec3 reflectDirection = reflect(-lightDirection, normal);\n	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n	// combine\n	vec3 ambient  = light.ambient * material.diffuse; \n	vec3 diffuse  = light.diffuse * difference * material.diffuse;\n	vec3 specular = light.specular * spec * material.specular;\n\n	return (ambient + diffuse + specular);\n}\n", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aNormal;\n\nout vec3 FragPosView;\nout vec3 Normal;\n\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat3 normal;\n\nvoid main()\n{\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n\n	FragPosView = vec3(view * model * vec4(aPos, 1.0));\n	Normal = normal * aNormal;\n}\n", "#version 450 core\nout vec4 FragColour;\n\nin vec3 FragPosView;\nin vec3 Normal;\n\nstruct Material {\n	vec3 diffuse;   // diffuse and ambient is the same on the material\n	vec3 specular;  \n	vec3 emission; // the diffuse, specular, and emission can be turned into textures for more detail\n	float shininess;\n};\nuniform Material material;\n\nstruct DirectionalLight {\n	bool exists;\n    vec3 direction; // this is in viewcoordinates\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \nuniform DirectionalLight directionalLight;\n\nstruct PointLight {    \n	bool exists;\n    vec3 position; // this is in viewcoordinates\n    \n    float constant;\n    float linear;\n    float quadratic;  \n\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \n// Calculate the four closest point lights\n#define NR_POINT_LIGHTS 4\nuniform PointLight pointLights[NR_POINT_LIGHTS];\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);  \n\nvoid main()\n{\n    vec3 normal = normalize(Normal);\n    vec3 viewDirection = normalize(-FragPosView);\n\n    // phase 1: Directional lighting\n    vec3 result = vec3(0.0, 0.0, 0.0);\n	if (directionalLight.exists) {\n		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);\n	}\n    // phase 2: Point lights\n	for (int i = 0; i < NR_POINT_LIGHTS; i++) {\n		if (pointLights[i].exists) {\n			result += CalculatePointLight(pointLights[i], normal, FragPosView, viewDirection);    \n		}\n	}\n    // phase 3: Spot light\n    // TODO: result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    \n    \n	// emission\n	result += material.emission;\n\n    FragColour = vec4(result, 1.0);\n}\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {\n	vec3 lightDirection = normalize(-light.direction);\n	// diffuse\n	float difference = max(dot(normal, lightDirection), 0.0);\n	// specular\n	vec3 reflectDirection = reflect(-lightDirection, normal);\n	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n	// combine\n	vec3 ambient  = light.ambient * material.diffuse; \n	vec3 diffuse  = light.diffuse * difference * material.diffuse;\n	vec3 specular = light.specular * spec * material.specular;\n\n	return (ambient + diffuse + specular);\n}\n\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)\n{\n    vec3 lightDirection = normalize(light.position - fragPos);\n    // diffuse\n    float difference = max(dot(normal, lightDirection), 0.0);\n    // specular\n    vec3 reflectDirection = reflect(-lightDirection, normal);\n    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n    // attenuation\n    float distance    = length(light.position - fragPos);\n    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n    // combine\n    vec3 ambient  = light.ambient  * material.diffuse;\n    vec3 diffuse  = light.diffuse  * difference * material.diffuse;\n    vec3 specular = light.specular * spec * material.specular;\n    ambient  *= attenuation;\n    diffuse  *= attenuation;\n    specular *= attenuation;\n\n    return (ambient + diffuse + specular);\n} \n", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aNormal;\n\nout vec3 FragPosView;\nout vec3 Normal;\n\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat3 normal;\n\nvoid main()\n{\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n\n	FragPosView = vec3(view * model * vec4(aPos, 1.0));\n	Normal = normal * aNormal;\n}", "#version 450 core\nout vec4 FragColour;\n\nin vec3 FragPosView;\nin vec3 Normal;\nin float NoiseValue;\n\nstruct Material {\n	vec3 diffuse;   // diffuse and ambient is the same on the material\n	vec3 specular;  \n	vec3 emission; // the diffuse, specular, and emission can be turned into textures for more detail\n	float shininess;\n};\nuniform Material material;\n\nstruct DirectionalLight {\n	bool exists;\n    vec3 direction; // this is in viewcoordinates\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \nuniform DirectionalLight directionalLight;\n\nstruct PointLight {    \n	bool exists;\n    vec3 position; // this is in viewcoordinates\n    \n    float constant;\n    float linear;\n    float quadratic;  \n\n    vec3 ambient;\n    vec3 diffuse;\n    vec3 specular;\n};  \n// Calculate the four closest point lights\n#define NR_POINT_LIGHTS 4\nuniform PointLight pointLights[NR_POINT_LIGHTS];\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);  \n\nvoid main()\n{\n	if (NoiseValue < 0.0 || NoiseValue > 1.0){\n		FragColour = vec4(1.0, 0.0, 0.0, 1.0);\n		return;\n	}\n	FragColour = vec4(NoiseValue, NoiseValue, NoiseValue, 1.0);\n	return;\n\n    vec3 normal = normalize(Normal);\n    vec3 viewDirection = normalize(-FragPosView);\n\n    // phase 1: Directional lighting\n    vec3 result = vec3(0.0, 0.0, 0.0);\n	if (directionalLight.exists) {\n		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);\n	}\n    // phase 2: Point lights\n	for (int i = 0; i < NR_POINT_LIGHTS; i++) {\n		if (pointLights[i].exists) {\n			result += CalculatePointLight(pointLights[i], normal, FragPosView, viewDirection);    \n		}\n	}\n    // phase 3: Spot light\n    // TODO: result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    \n    \n	// emission\n	result += material.emission;\n\n    FragColour = vec4(result, 1.0);\n}\n\nvec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {\n	vec3 lightDirection = normalize(-light.direction);\n	// diffuse\n	float difference = max(dot(normal, lightDirection), 0.0);\n	// specular\n	vec3 reflectDirection = reflect(-lightDirection, normal);\n	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n	// combine\n	vec3 ambient  = light.ambient * material.diffuse; \n	vec3 diffuse  = light.diffuse * difference * material.diffuse;\n	vec3 specular = light.specular * spec * material.specular;\n\n	return (ambient + diffuse + specular);\n}\n\nvec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)\n{\n    vec3 lightDirection = normalize(light.position - fragPos);\n    // diffuse\n    float difference = max(dot(normal, lightDirection), 0.0);\n    // specular\n    vec3 reflectDirection = reflect(-lightDirection, normal);\n    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);\n    // attenuation\n    float distance    = length(light.position - fragPos);\n    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n    // combine\n    vec3 ambient  = light.ambient  * material.diffuse;\n    vec3 diffuse  = light.diffuse  * difference * material.diffuse;\n    vec3 specular = light.specular * spec * material.specular;\n    ambient  *= attenuation;\n    diffuse  *= attenuation;\n    specular *= attenuation;\n\n    return (ambient + diffuse + specular);\n} \n", "#version 450 core\nlayout (points) in;\nlayout (triangle_strip, max_vertices = 100) out;\n\nout vec3 FragPosView;\nout vec3 Normal;\nout float NoiseValue;\n\nin VS_OUT{\n	vec3 chunkPosition;\n	vec3 vertexPositions[8];\n	float noiseValues[8];\n} gs_in[];\n\nuniform float cellLength;\n\n// no model matrix necessary\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat3 normal;\n\nvoid main() {\n	vec3 chunkPosition = gs_in[0].chunkPosition;\n	vec3 cellPosition = vec3(gl_in[0].gl_Position);\n\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + gs_in[0].vertexPositions[0], 1.0);\n	NoiseValue = gs_in[0].noiseValues[0];\n	EmitVertex();\n\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + gs_in[0].vertexPositions[1], 1.0);\n	NoiseValue = gs_in[0].noiseValues[1];\n	EmitVertex();\n\n	gl_Position = projection * view * vec4(chunkPosition + cellPosition + gs_in[0].vertexPositions[2], 1.0);\n	NoiseValue = gs_in[0].noiseValues[2];\n	EmitVertex();\n	\n	EndPrimitive();\n\n\n\n\n	// ---------------------------\n	// OLD\n	// ---------------------------\n\n	// Bottom\n//	Normal = normal * vec3(0.0, 0.0, -1.0);\n//\n//	vertexPosition = vec3(0.0, 0.0, -cellLength * cellLengthFactor);\n//	FragPosView = vec3(view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0));\n//	gl_Position = projection * view * vec4(chunkPosition + cellPosition + vertexPosition, 1.0);\n//	EmitVertex();\n\n//	EndPrimitive();\n\n\n	//FragPosView = vec3(view * model * vec4(aPos, 1.0));\n	//Normal = normal * aNormal;\n}\n\n//	vec3 vertexPositions[] = {\n//		vec3(0.0, 0.0, 0.0),\n//		vec3(cellLength, 0.0, 0.0),\n//		vec3(0.0, cellLength, 0.0),\n//		vec3(cellLength, cellLength, 0.0),\n//		vec3(0.0, 0.0, -cellLength),\n//		vec3(cellLength, 0.0, -cellLength),\n//		vec3(0.0, cellLength, -cellLength),\n//		vec3(cellLength, cellLength, -cellLength)\n//	};\n", "#version 450 core\nlayout (location = 0) in vec3 aCellPos;\nlayout (location = 1) in vec3 aChunkPosition;\n\nout VS_OUT{\n	vec3 chunkPosition;\n	vec3 vertexPositions[8];\n	float noiseValues[8];\n} vs_out;\n\nuniform float cellLength;\n\nconst int permuations[] = { 151,160,137,91,90,15,										131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,\n							151,160,137,91,90,15,										131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180\n};\n\nfloat octavePerlin(float x, float y, float z, int octaves, float persistence);\nfloat perlin(float x, float y, float z);\nfloat grad(int hash, float x, float y, float z);\nfloat fade(float t);\nfloat lerp(float a, float b, float x);\n\nvoid main() {\n    gl_Position = vec4(aCellPos, 1.0);\n	vs_out.chunkPosition = aChunkPosition;\n	\n	vs_out.vertexPositions[0] = vec3(0.0, 0.0, 0.0);\n	vs_out.vertexPositions[1] = vec3(cellLength, 0.0, 0.0);\n	vs_out.vertexPositions[2] = vec3(0.0, cellLength, 0.0);\n	vs_out.vertexPositions[3] = vec3(cellLength, cellLength, 0.0);\n	vs_out.vertexPositions[4] = vec3(0.0, 0.0, -cellLength);\n	vs_out.vertexPositions[5] = vec3(cellLength, 0.0, -cellLength);\n	vs_out.vertexPositions[6] = vec3(0.0, cellLength, -cellLength);\n	vs_out.vertexPositions[7] = vec3(cellLength, cellLength, -cellLength);\n\n	for (int i = 0; i < 8; i++) {\n		vs_out.noiseValues[i] = octavePerlin(vs_out.vertexPositions[i].x + aChunkPosition.x + aCellPos.x, \n											 vs_out.vertexPositions[i].y + aChunkPosition.y + aCellPos.y, \n											 vs_out.vertexPositions[i].z + aChunkPosition.z + aCellPos.z, \n											 1,\n											 1.0);\n	}\n}\n\nfloat octavePerlin(float x, float y, float z, int octaves, float persistence) {\n	x = abs(x);\n	y = abs(y);\n	z = abs(z);\n\n	float total = 0.0;\n	float frequency = 1.0;\n	float amplitude = 1.0;\n	float maxValue = 0.0;			// Used for normalizing result to 0.0 - 1.0\n	for (int i = 0; i < octaves; i++) {\n		total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;\n			\n		maxValue += amplitude;\n			\n		amplitude *= persistence;\n		frequency *= 2.0;\n	}\n		\n	return total / maxValue;\n}\n\nfloat perlin(float x, float y, float z) {\n	int xi = int(x) & 255;								// Calculate the unit cube that the point asked will be located in\n	int yi = int(y) & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that\n	int zi = int(z) & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.\n	float xf = x - int(x);								// We also fade the location to smooth the result.\n	float yf = y - int(y);\n	float zf = z - int(z);\n	float u = fade(xf);\n	float v = fade(yf);\n	float w = fade(zf);\n															\n	int aaa, aba, aab, abb, baa, bba, bab, bbb;\n	aaa = permuations[permuations[permuations[xi    ] + yi    ] + zi    ];\n	aba = permuations[permuations[permuations[xi    ] + yi + 1] + zi    ];\n	aab = permuations[permuations[permuations[xi    ] + yi    ] + zi + 1];\n	abb = permuations[permuations[permuations[xi    ] + yi + 1] + zi + 1];\n	baa = permuations[permuations[permuations[xi + 1] + yi    ] + zi    ];\n	bba = permuations[permuations[permuations[xi + 1] + yi + 1] + zi    ];\n	bab = permuations[permuations[permuations[xi + 1] + yi    ] + zi + 1];\n	bbb = permuations[permuations[permuations[xi + 1] + yi + 1] + zi + 1];\n	\n	float x1, x2, y1, y2;\n	x1 = lerp(	grad (aaa, xf      , yf  , zf),				// The gradient function calculates the dot product between a pseudorandom\n				grad (baa, xf - 1.0, yf  , zf),				// gradient vector and the vector from the input coordinate to the 8\n				u);										// surrounding points in its unit cube.\n	x2 = lerp(	grad (aba, xf      , yf - 1.0, zf),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)\n				grad (bba, xf - 1.0, yf - 1.0, zf),				// values we made earlier.\n			        u);\n	y1 = lerp(x1, x2, v);\n\n	x1 = lerp(	grad (aab, xf      , yf  , zf - 1.0),\n				grad (bab, xf - 1.0, yf  , zf - 1.0),\n				u);\n	x2 = lerp(	grad (abb, xf      , yf - 1.0, zf - 1.0),\n		        grad (bbb, xf - 1.0, yf - 1.0, zf - 1.0),\n		        u);\n	y2 = lerp (x1, x2, v);\n		\n	return (lerp (y1, y2, w) + 1.0) / 2.0;						// For convenience we bound it to 0 - 1 (theoretical min/max before is -1 - 1)\n}\n\nfloat grad(int hash, float x, float y, float z) {\n	int h = hash & 15;									// Take the hashed value and take the first 4 bits of it (15 == 0b1111)\n	float u = h < 8 /* 0b1000 */ ? x : y;				// If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.\n		\n	float v;											// In Ken Perlin's original implementation this was another conditional operator (?:).  I\n														// expanded it for readability.\n		\n	if(h < 4 /* 0b0100 */)								// If the first and second significant bits are 0 set v = y\n		v = y;\n	else if(h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/)// If the first and second significant bits are 1 set v = x\n		v = x;\n	else 												// If the first and second significant bits are not equal (0/1, 1/0) set v = z\n		v = z;\n		\n	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.\n}\n	\nfloat fade(float t) {\n														// Fade function as defined by Ken Perlin.  This eases coordinate values\n														// so that they will ease towards integral values.  This ends up smoothing\n														// the final output.\n	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);			// 6t^5 - 15t^4 + 10t^3\n}\n	\nfloat lerp(float a, float b, float x) {\n	return a + x * (b - a);\n}\n", "#version 450 core\nout vec4 FragColour;\n\nin vec2 texCoord;\n\n// textures\nuniform sampler2D texture0;\n\nvoid main()\n{\n	FragColour = texture(texture0, texCoord);\n}", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoord;\n\nout vec2 texCoord;\n\nuniform mat4 model;\nuniform mat4 view;\nuniform mat4 projection;\nuniform mat4 normal;\n\nvoid main()\n{\n    gl_Position = projection * view * model * vec4(aPos, 1.0);\n	texCoord = aTexCoord;\n}" };
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