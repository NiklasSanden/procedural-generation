#version 450 core
out vec4 FragColour;

in vec3 FragPosWorld;
in vec3 FragPosView;
in vec3 Normal;
in vec3 WorldNormal;

uniform float viewDistance;
uniform int LOD;

struct Material {
	vec3 diffuse;   // diffuse and ambient is the same on the material
	vec3 specular;  
	vec3 emission; // the diffuse, specular, and emission can be turned into textures for more detail
	float shininess;
};
uniform Material material;

struct DirectionalLight {
	bool exists;
    vec3 direction; // this is in viewcoordinates
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirectionalLight directionalLight;

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
float Lerp(float aValue, float bValue, float surfaceLevel);

void main()
{
	//FragColour = vec4(0.5, 0.0, 0.0, 1.0);
	//return;

	float dist = length(FragPosView);
	if (dist > viewDistance) discard;

    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(-FragPosView);

    // phase 1: Directional lighting
    vec3 result = vec3(0.0, 0.0, 0.0);
	if (directionalLight.exists) {
		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);
	}
    
	// emission
	result += material.emission;

	// FOG
	vec3 fogColour = vec3(0.2, 0.25, 0.4);
	float surfaceLevel = pow(min(dist / viewDistance, 1.0), 3.0);
    FragColour = vec4(Lerp(result.x, fogColour.x, surfaceLevel), 
					  Lerp(result.y, fogColour.y, surfaceLevel),
					  Lerp(result.z, fogColour.z, surfaceLevel), 1.0);
	//FragColour = vec4(result, 1.0);
}


vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
	vec3 lightDirection = normalize(-light.direction);
	// diffuse
	float difference = max(dot(normal, lightDirection), 0.0);
	// specular
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	// combine

	vec3 green = vec3(0.133, 0.545, 0.133);
	vec3 brown = vec3(0.545 , 0.27, 0.07);
	vec3 grey = vec3(0.4, 0.4, 0.4);
	
	vec3 newDiffuse = mix(mix(green, brown, FragPosWorld.y / 4.0 + 2.0), brown, FragPosWorld.y / 5.0 + 2.0);

//	vec3 newDiffuse = brown;
//	if (roundEven(FragPosWorld.y) < -5.9) {
//		newDiffuse = grey;
//	}
//	else if (dot(WorldNormal, vec3(0.0, 1.0, 0.0)) > 0.75) {
//		newDiffuse = green;
//	}


	vec3 ambient  = light.ambient * newDiffuse; // material.diffuse
	vec3 diffuse  = light.diffuse * difference * newDiffuse; // material.diffuse
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);
}

float Lerp(float aValue, float bValue, float surfaceLevel) {
	return aValue + surfaceLevel * (bValue - aValue);
}
