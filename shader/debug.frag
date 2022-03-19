#version 430
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D Framebuffer;

void main()
{
	FragColor = texture(Framebuffer, TexCoords);
}