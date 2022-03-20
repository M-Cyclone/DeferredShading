#version 430 core
in vec2 TexCoords;

out vec2 FragColor;

const float PI = 3.14159265359;

// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float getRadicalInverseVdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 getHammersley(uint i, uint N)
{
	return vec2(float(i) / float(N), getRadicalInverseVdC(i));
}

vec3 getImportanceSampleGGX(uint i, uint totalCount, vec3 N, float roughness)
{
    vec2 randomX = getHammersley(i, totalCount);

	float a = roughness * roughness;
	
	float phi = 2.0 * PI * randomX.x;
	float cosTheta = sqrt((1.0 - randomX.y) / (1.0 + (a * a - 1.0) * randomX.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 halfway = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
	
	// from tangent-space H vector to world-space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * halfway.x + bitangent * halfway.y + N * halfway.z;
	return normalize(sampleVec);
}


float getSchlickBeckmannGGX(vec3 normal, vec3 dir, float k)
{
	float ndotd = max(dot(normal, dir), 0.0);
	return ndotd / (ndotd * (1.0 - k) + k);
}

float getGeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float k = (roughness * roughness) / 2.0;

    float ggx1 = getSchlickBeckmannGGX(normal, viewDir, k);
    float ggx2 = getSchlickBeckmannGGX(normal, lightDir, k);

    return ggx1 * ggx2;
}


vec2 getIntegrateBRDF(float NdotV, float roughness)
{
    // NdotV is dot(normal, viewDir), or cosTheta_i

    vec3 viewDir = vec3(sqrt(1.0 - NdotV*NdotV), 0.0, NdotV);

    float A = 0.0;
    float B = 0.0; 

    vec3 normal = vec3(0.0, 0.0, 1.0);
    
    const uint kSampleCount = 1024u;
    for(uint i = 0u; i < kSampleCount; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec3 halfway = getImportanceSampleGGX(i, kSampleCount, normal, roughness);
        vec3 lightDir = normalize(2.0 * dot(viewDir, halfway) * halfway - viewDir);

        float NdotL = max(lightDir.z, 0.0);
        float NdotH = max(halfway.z, 0.0);
        float VdotH = max(dot(viewDir, halfway), 0.0);

        if(NdotL > 0.0)
        {
            // calculate different part of the brdf
            float G = getGeometrySmith(normal, viewDir, lightDir, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(kSampleCount);
    B /= float(kSampleCount);
    return vec2(A, B);
}

void main() 
{
    // the brdf integration result is only related to cosTheta_i and roughness
    // when using this texture, brdf_integration = F0 * A(cosTheta, roughness) + B(cosTheta, roughness)
    vec2 integratedBRDF = getIntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}