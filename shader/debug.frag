#version 430 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D Framebuffer;

void main()
{
	FragColor = texture(Framebuffer, TexCoords);
//	FragColor = vec4(1.0);
}