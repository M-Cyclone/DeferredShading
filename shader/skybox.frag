#version 430 core
out vec4 FragColor;

in vec3 CubeMapTexCoords;

uniform samplerCube SkyBox;
uniform sampler2D DepthMap;

uniform float wndWidth;
uniform float wndHeight;

void main()
{
	vec2 TexCoords = vec2(gl_FragCoord.x / wndWidth, gl_FragCoord.y / wndHeight);
	if(texture(DepthMap, TexCoords).r < 1.0) discard;

	vec3 color = texture(SkyBox, CubeMapTexCoords).rgb;

	// hdr and gamma correction
	color /= (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

	FragColor = vec4(color, 1.0);
}