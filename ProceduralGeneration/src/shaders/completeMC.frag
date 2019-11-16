#version 450 core
out vec4 FragColour;

in vec3 FragPosWorld;
in vec3 FragPosView;
in vec3 Normal;
in vec3 WorldNormal;

uniform float viewDistance;
uniform int LOD;
uniform float alpha;
uniform vec3 playerPos;

layout(binding = 0) uniform sampler2D Texture0;
layout(binding = 1) uniform sampler2D Texture1;
layout(binding = 2) uniform sampler2D Texture2;
layout(binding = 6) uniform sampler2D SandEskil;

layout(binding = 5) uniform samplerCube skybox;

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
vec4 GetColourTexture(vec3 normal);
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
	//result += material.emission;

	// FOG
	//vec3 fogColour = vec3(0.2, 0.25, 0.4);
	vec3 fogColour = texture(skybox, FragPosWorld - playerPos).xyz;
	float surfaceLevel = pow(min(dist / viewDistance, 1.0), 4.0);
	//surfaceLevel = 0.0;
	//surfaceLevel = clamp((surfaceLevel - 0.4) * 2.5, 0.0, 1.0);
//    FragColour = vec4(Lerp(result.x, fogColour.x, surfaceLevel), 
//					  Lerp(result.y, fogColour.y, surfaceLevel),
//					  Lerp(result.z, fogColour.z, surfaceLevel), 1.0);
	FragColour = vec4(Lerp(result.x, fogColour.x, surfaceLevel), 
					  Lerp(result.y, fogColour.y, surfaceLevel),
					  Lerp(result.z, fogColour.z, surfaceLevel), alpha);
	//FragColour = vec4(result, min(alpha, 1.0 - surfaceLevel));
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
	vec3 magenta = vec3(0.99, 0.0, 0.99);
	vec3 grey = vec3(0.4, 0.4, 0.4);
	vec3 blue = vec3(0.0, 0.467, 0.745);
	
	//vec3 newDiffuse = mix(mix(green, brown, clamp(FragPosWorld.y / 4.0 + 1.0, 0.0, 1.0)), brown, clamp(FragPosWorld.y / 5.0 + 1.0, 0.0, 1.0));
	//vec3 newDiffuse = mix(green, brown, clamp(FragPosWorld.y / 4.0 + 1.0, 0.0, 1.0));
//	vec3 newDiffuse = brown;
//	float normalDot = dot(WorldNormal, vec3(0.0, 1.0, 0.0));
//	if (normalDot > 0.9) newDiffuse = green;
//	else if (normalDot > 0.5) {
//		newDiffuse = mix(brown, green, (normalDot - 0.5) * 2.5);
//	}

	//newDiffuse *= (GetColourTexture(WorldNormal) / 4.0 + vec3(0.75, 0.75, 0.75));
	vec3 newDiffuse = GetColourTexture(WorldNormal).xyz;

//	if (FragPosWorld.y <= -2.0 + LOD * 0.05) {
//		newDiffuse = mix(blue, vec3(0.0, 0.0, 0.0), clamp((FragPosWorld.y + 2.0) * 20.0, 0.0, 1.0));
//	}

//	vec3 newDiffuse = brown;
//	if (roundEven(FragPosWorld.y) < -5.9) {
//		newDiffuse = grey;
//	}
//	else if (dot(WorldNormal, vec3(0.0, 1.0, 0.0)) > 0.75) {
//		newDiffuse = green;
//	}


	vec3 ambient  = light.ambient * newDiffuse * 1.1; // material.diffuse
	vec3 diffuse  = light.diffuse * difference * newDiffuse; // material.diffuse
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);
}

vec4 GetColourTexture(vec3 normal) {

	// Determine the blend weights for the 3 planar projections.
	// N_orig is the vertex-interpolated normal vector.
	vec3 blend_weights = abs(normal);   // Tighten up the blending zone:
	blend_weights = (blend_weights - 0.2) * 7;
	blend_weights = max(blend_weights, 0);      // Force weights to sum to 1.0 (very important!)
	blend_weights /= (blend_weights.x + blend_weights.y + blend_weights.z ).xxx; 
	// Now determine a color value and bump vector for each of the 3
	// projections, blend them, and store blended results in these two
	// vectors:
	vec4 blended_color; // .w hold spec value
	{
		// Compute the UV coords for each of the 3 planar projections.
		// tex_scale (default ~ 1.0) determines how big the textures appear.
		float tex_scale = 0.75;
		vec2 coord1 = FragPosWorld.yz * tex_scale;
		vec2 coord2 = FragPosWorld.zx * tex_scale;
		vec2 coord3 = FragPosWorld.xy * tex_scale;
		// This is where you would apply conditional displacement mapping.
		//if (blend_weights.x > 0) coord1 = . . .
		//if (blend_weights.y > 0) coord2 = . . .
		//if (blend_weights.z > 0) coord3 = . . .
		// Sample color maps for each projection, at those UV coords.
		vec4 col1 = texture(Texture0, coord1);
		vec4 col2;
		if (WorldNormal.y < 0.0) {
			col2 = texture(Texture0, coord2);
		}
		else {
			vec4 sand = texture(SandEskil, coord2);
			vec4 rock = texture(Texture0, coord2);

			rock = mix(sand, rock, pow(1.0 - clamp(min((WorldNormal.y - 0.75) * (4), 
														-FragPosWorld.y + 1.0), 0.0, 1.0), 2));

			vec4 grass = texture(Texture1, coord2);

			grass = mix(grass, rock, clamp(-FragPosWorld.y + 0.3, 0.0, 1.0));

			col2 = grass;
		}
		vec4 col3 = texture(Texture2, coord3);
		 // Finally, blend the results of the 3 planar projections.
		blended_color = col1.xyzw * blend_weights.xxxx +
						col2.xyzw * blend_weights.yyyy +
						col3.xyzw * blend_weights.zzzz;
	}

	return blended_color;
}

float Lerp(float aValue, float bValue, float surfaceLevel) {
	return aValue + surfaceLevel * (bValue - aValue);
}
