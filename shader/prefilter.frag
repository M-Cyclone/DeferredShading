#version 430 core
in vec3 WorldPos;

out vec4 FragColor;

uniform samplerCube HdrCubeMap;
uniform float resolution;
uniform float roughness;

const float PI = 3.14159265359;


float getRadicalInverseVdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 getHammersley(uint idx, uint totalCount)
{
    // res.x is not random and it's used for phi
    // res.y is random and it's used to generate random theta
    return vec2(float(idx) / float(totalCount), getRadicalInverseVdC(idx));
}


vec3 getImportanceSampleGGX(uint idx, uint totalCount, vec3 normal, float roughness)
{
    float alpha = roughness * roughness;

    vec2 randomX = getHammersley(idx, totalCount);
    float phi = randomX.x * 2.0 * PI;
    float cosTheta = sqrt((1.0 - randomX.y) / (1.0 + (alpha * alpha - 1.0) * randomX.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 halfDirTangentSpace = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = normalize(cross(normal, tangent));

    vec3 halfDirWorldSpace = tangent * halfDirTangentSpace.x + bitangent * halfDirTangentSpace.y + normal * halfDirTangentSpace.z;
    return normalize(halfDirWorldSpace);
}


float getNormalDistribution(vec3 normal, vec3 halfDir, float roughness)
{
	// GGX model
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float ndoth = max(dot(normal, halfDir), 0.0);

	float value = ndoth * ndoth * (alpha2 - 1.0) + 1.0;

	return alpha2 / (PI * value * value);
}


void main()
{
    // Solid angle associated to a texel of the cubemap
    float saTexel = 4.0 * PI / (6.0 * resolution * resolution);

    vec3 normal = normalize(WorldPos);
    vec3 viewDir = normal;

    const uint kSampleCount = 1024u;

    vec3 color = vec3(0.0);
    float totalWeight = 0.0;

    for(uint i = 0; i < kSampleCount; ++i)
    {
        vec3 halfDir = getImportanceSampleGGX(i, kSampleCount, normal, roughness);
        vec3 lightDir = normalize(2.0 * dot(viewDir, halfDir) * halfDir - viewDir);

        float ldotn = dot(lightDir, normal);
        if(ldotn > 0.0)
        {
            // sample on the mipmap of HdrCubeMap to avoid the light points
            // microfacet model, the posibility density of the normal direction
            float ndoth = max(dot(normal, halfDir), 0.0);
            float hdotv = max(dot(viewDir, halfDir), 0.0);

            // the posibility density of the light direction for impotance sample
            float pdf = getNormalDistribution(normal, halfDir, roughness) * ndoth / (4.0 * hdotv) + 0.0001;

            // Solid angle associated to a sample
            float saSample = 1.0 / (float(kSampleCount) * pdf + 0.0001);

            float miplevel = (roughness == 0.0) ? 0.0 : (0.5 * log2(saSample / saTexel));

            color += textureLod(HdrCubeMap, lightDir, miplevel).rgb * ldotn;
            totalWeight += ldotn;
        }
    }

    color /= totalWeight;
    FragColor = vec4(color, 1.0);
}