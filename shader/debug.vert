#version 430
layout(location = 0) in vec2 aPositions;

out vec2 TexCoords;

void main()
{
	TexCoords = (aPositions + 1.0) * 0.5;
	gl_Position = vec4(aPositions, 0.0, 1.0);
}