#version 430
layout(location = 0) in vec3 aPositions;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 WorldPos;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	WorldPos = aPositions;
	gl_Position = proj * view * vec4(aPositions, 1.0);
}