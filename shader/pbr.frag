#version 430
in struct VSOut
{
	vec2 TexCoords;
} vsOut;

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gBaseColor;
uniform sampler2D gNormal;
uniform sampler2D gMetallicRoughness;

#define LIGHT_COUNT 4
struct PointLight
{
	vec3 position;
	vec3 intensity;
};
uniform PointLight lights[LIGHT_COUNT];

uniform vec3 cameraPos;

const float PI = 3.14159265359;

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
	vec3 F0 = mix(vec3(0.04f), baseColor, metallic);

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
	vec3 baseColor = texture(gBaseColor, vsOut.TexCoords).rgb;
	float metallic = texture(gMetallicRoughness, vsOut.TexCoords).r;
	float roughness = texture(gMetallicRoughness, vsOut.TexCoords).g;

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

void main()
{
	vec3 position = texture(gPosition, vsOut.TexCoords).rgb;
	vec3 normal = texture(gNormal, vsOut.TexCoords).rgb;
	
	vec3 viewDir = normalize(cameraPos - position);
	
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


	// hdr
	color /= (color + vec3(1.0));

	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));


	FragColor = vec4(color, 1.0);
}