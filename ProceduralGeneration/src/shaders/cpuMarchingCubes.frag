#version 450 core
out vec4 FragColour;

in vec3 FragPosView;
in vec3 Normal;

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

void main()
{
	//FragColour = vec4(0.5, 0.0, 0.0, 1.0);
	//return;

    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(-FragPosView);

    // phase 1: Directional lighting
    vec3 result = vec3(0.0, 0.0, 0.0);
	if (directionalLight.exists) {
		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);
	}
    
	// emission
	result += material.emission;

    FragColour = vec4(result, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection) {
	vec3 lightDirection = normalize(-light.direction);
	// diffuse
	float difference = max(dot(normal, lightDirection), 0.0);
	// specular
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	// combine
	vec3 ambient  = light.ambient * material.diffuse; 
	vec3 diffuse  = light.diffuse * difference * material.diffuse;
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);
}
