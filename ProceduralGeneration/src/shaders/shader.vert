#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

out vec3 ourColour;

uniform float timeValue;

vec3 finalColour;

void main()
{
    gl_Position = vec4(aPos, 1.0);

	if (aColour.x > 0.9){
		if (timeValue < 1.0f/3.0f){
			finalColour = vec3(aColour.x - timeValue * 3.0, timeValue * 3.0, 0.0); 
		}
		else if (timeValue < 2.0f/3.0f){
			finalColour = vec3(0.0, aColour.x - (timeValue - 0.33) * 3.0, (timeValue - 0.33) * 3.0);
		}
		else{
			finalColour = vec3((timeValue - 0.66) * 3.0, 0.0, aColour.x - (timeValue - 0.66) * 3.0);
		}
	}
	else if (aColour.y > 0.9){
		if (timeValue < 1.0f/3.0f){
			finalColour = vec3(0.0, aColour.y - timeValue * 3.0, timeValue * 3.0);
		}
		else if (timeValue < 2.0f/3.0f){
			finalColour = vec3((timeValue - 0.33) * 3.0, 0.0, aColour.y - (timeValue - 0.33) * 3.0);
		}
		else{
			finalColour = vec3(aColour.y - (timeValue - 0.66) * 3.0, (timeValue - 0.66) * 3.0, 0.0); 
		}
	}
	else if (aColour.z > 0.9){
		if (timeValue < 1.0f/3.0f){
			finalColour = vec3(timeValue * 3.0, 0.0, aColour.z - timeValue * 3.0);
		}
		else if (timeValue < 2.0f/3.0f){
			finalColour = vec3(aColour.z - (timeValue - 0.33) * 3.0, (timeValue - 0.33) * 3.0, 0.0); 
		}
		else{
			finalColour = vec3(0.0, aColour.z - (timeValue - 0.66) * 3.0, (timeValue - 0.66) * 3.0);
		}
	}

    ourColour = finalColour;
}