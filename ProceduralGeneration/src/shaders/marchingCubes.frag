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

struct PointLight {    
	bool exists;
    vec3 position; // this is in viewcoordinates
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
// Calculate the four closest point lights
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection);  

void main()
{
	FragColour = vec4(0.5, 0.0, 0.0, 1.0);
	return;

    vec3 normal = normalize(Normal);
    vec3 viewDirection = normalize(-FragPosView);

    // phase 1: Directional lighting
    vec3 result = vec3(0.0, 0.0, 0.0);
	if (directionalLight.exists) {
		result += CalculateDirectionalLight(directionalLight, normal, viewDirection);
	}
    // phase 2: Point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++) {
		if (pointLights[i].exists) {
			result += CalculatePointLight(pointLights[i], normal, FragPosView, viewDirection);    
		}
	}
    // phase 3: Spot light
    // TODO: result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
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

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDirection)
{
    vec3 lightDirection = normalize(light.position - fragPos);
    // diffuse
    float difference = max(dot(normal, lightDirection), 0.0);
    // specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine
    vec3 ambient  = light.ambient  * material.diffuse;
    vec3 diffuse  = light.diffuse  * difference * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
} 
