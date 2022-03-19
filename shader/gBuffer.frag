#version 430
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gBaseColor;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gMetallicRoughness;

in struct VSOut
{
	vec3 WorldPos;
	vec3 Normal;
	vec2 TexCoords;
} vsOut;

uniform sampler2D BaseColor;
uniform sampler2D NormalMap;
uniform sampler2D Metallic;
uniform sampler2D Roughness;

vec3 getNormal()
{
	vec3 tangentNormal = texture(NormalMap, vsOut.TexCoords).xyz * 2.0 - 1.0;

	vec3 deltaPos1 = dFdx(vsOut.WorldPos);
	vec3 deltaPos2 = dFdy(vsOut.WorldPos);
	vec2 deltaUV1 = dFdx(vsOut.TexCoords);
	vec2 deltaUV2 = dFdy(vsOut.TexCoords);

	vec3 N = normalize(vsOut.Normal);
    vec3 T = normalize(deltaPos1 * deltaUV2.t - deltaPos2 * deltaUV1.t);
    vec3 B = -normalize(cross(N, T));

	return normalize(mat3(T, B, N) * tangentNormal);
}

void main()
{
	gPosition = vsOut.WorldPos;
	gBaseColor.rgb = texture(BaseColor, vsOut.TexCoords).rgb;
	gNormal = getNormal();
	gMetallicRoughness = vec3(texture(Metallic, vsOut.TexCoords).r, texture(Roughness, vsOut.TexCoords).r, 0.0);
}