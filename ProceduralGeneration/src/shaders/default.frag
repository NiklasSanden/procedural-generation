#version 450 core
out vec4 FragColour;

in vec3 FragPosView;
in vec3 LightPosView;
in vec3 Normal;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 position; // in world coordinates
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light; 

void main()
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;
	
	// diffuse
	vec3 lightDirection = normalize(LightPosView - FragPosView);
	vec3 normal = normalize(Normal);
	float difference = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * (difference * material.diffuse);

	// specular
	vec3 viewDirection = normalize(-FragPosView); // viewPos - fragPosView, but viewPos is (0, 0, 0) in viewspace
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColour = vec4(result, 1.0);
}