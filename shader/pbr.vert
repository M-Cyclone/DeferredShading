#version 430
layout(location = 0) in vec3 aPositions;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out struct VSOut
{
	vec2 TexCoords;
} vsOut;

void main()
{
	vsOut.TexCoords = aTexCoords;
	gl_Position = vec4(aPositions, 1.0);
}