#version 430 core
layout(location = 0) in vec3 aPositions;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 CubeMapTexCoords;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	CubeMapTexCoords = aPositions;
	vec4 pos = proj * mat4(mat3(view)) * vec4(aPositions, 1.0);
	gl_Position = pos.xyww;
}