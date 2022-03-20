#version 430 core
in struct VSOut
{
	vec2 TexCoords;
} vsOut;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gBaseColor;
uniform sampler2D gNormal;
uniform sampler2D gMetallicRoughness;

uniform samplerCube PrefilterMap;
uniform sampler2D BrdfLut;

#define LIGHT_COUNT 4
struct PointLight
{
	vec3 position;
	vec3 intensity;
};
uniform PointLight lights[LIGHT_COUNT];

uniform vec3 cameraPos;

const float PI = 3.14159265359;

vec3 baseColor;
float metallic;
float roughness;
vec3 F0;

float getNormalDistribution(vec3 normal, vec3 halfDir, float roughness)
{
	// GGX model
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float ndoth = dot(normal, halfDir);

	float value = ndoth * ndoth * (alpha2 - 1.0) + 1.0;

	return alpha2 / (PI * value * value);
}

vec3 getFresnel(vec3 halfDir, vec3 viewDir, vec3 baseColor, float metallic)
{
	return F0 + (1.0 - F0) * pow(1 - dot(halfDir, viewDir), 5);
}

float getSchlickBeckmannGGX(vec3 normal, vec3 dir, float k)
{
	float ndotd = max(dot(normal, dir), 0.0);
	return ndotd / (ndotd * (1.0 - k) + k);
}

float getShadowMask(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
	// Smith model
	float k = (roughness + 1) * (roughness + 1) / 8.0;
	float ggx1 = getSchlickBeckmannGGX(normal, viewDir, k);
	float ggx2 = getSchlickBeckmannGGX(normal, lightDir, k);

	return ggx1 * ggx2;
}

vec3 getMicroFacetBRDF(vec3 normal, vec3 viewDir, vec3 lightDir)
{
	vec3 halfDir = normalize(viewDir + lightDir);

	float D = getNormalDistribution(normal, halfDir, roughness);
	vec3 F = getFresnel(halfDir, viewDir, baseColor, metallic);
	float G = getShadowMask(normal, viewDir, lightDir, roughness);

	return (D * F * G) / (4.0 * dot(normal, viewDir) * dot(normal, lightDir) + 0.0001);
}

vec3 getBRDF(vec3 normal, vec3 viewDir, vec3 lightDir)
{	
	return getMicroFacetBRDF(normal, viewDir, lightDir);
}


vec3 getFresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   


void main()
{
	vec3 position = texture(gPosition, vsOut.TexCoords).rgb;

	vec3 normal = texture(gNormal, vsOut.TexCoords).rgb;	
	vec3 viewDir = normalize(cameraPos - position);
	vec3 reflectDir = reflect(-viewDir, normal);

	baseColor = texture(gBaseColor, vsOut.TexCoords).rgb;
	metallic = texture(gMetallicRoughness, vsOut.TexCoords).r;
	roughness = texture(gMetallicRoughness, vsOut.TexCoords).g;

	F0 = mix(vec3(0.04f), baseColor, metallic);

	vec3 color = vec3(0.0);
	for(int i = 0; i < LIGHT_COUNT; ++i)
	{
		vec3 lightDir = normalize(lights[i].position - position);

		float dist = length(lights[i].position - position);
		vec3 li = lights[i].intensity / (dist * dist);

		float cosTheta = dot(normal, lightDir);

		vec3 brdf = getBRDF(normal, viewDir, lightDir);

		color += li * brdf * cosTheta;
	}


	const float kMaxReflectionLod = 4.0;
	vec3 F = getFresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
	vec3 prefilterLight = textureLod(PrefilterMap, reflectDir, roughness * kMaxReflectionLod).rgb;
	vec2 brdf = texture(BrdfLut, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
	vec3 specular = prefilterLight * (F * brdf.x + brdf.y);

	color += specular;


	// hdr
	color /= (color + vec3(1.0));

	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));


	FragColor = vec4(color, 1.0);
}