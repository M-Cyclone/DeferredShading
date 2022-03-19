#version 430
layout(location = 0) in vec3 aPositions;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out struct VSOut
{
	vec3 WorldPos;
	vec3 Normal;
	vec2 TexCoords;
} vsOut;

void main()
{
	vec4 worldPos = model * vec4(aPositions, 1.0);
	gl_Position = proj * view * worldPos;

	vsOut.WorldPos = worldPos.xyz;
	vsOut.Normal = transpose(inverse(mat3(model))) * aNormal;
	vsOut.TexCoords = aTexCoords;
}