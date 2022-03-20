#version 430 core
in vec3 WorldPos;

out vec4 FragColor;

uniform samplerCube HdrCubeMap;

const float PI = 3.14159265359;

void main()
{		
    vec3 normal = normalize(WorldPos);

    vec3 irradiance = vec3(0.0);

    vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), normal));
    vec3 up = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    int sampleCount = 0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            vec3 dirTangentSpace = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            vec3 dirWorldSpace = dirTangentSpace.x * right + dirTangentSpace.y * up + dirTangentSpace.z * normal;
            
            irradiance += texture(HdrCubeMap, dirWorldSpace).rgb * sin(2 * theta) * 0.5;

            ++sampleCount;
        }
    }

    irradiance *= PI * (1.0 / float(sampleCount));

    FragColor = vec4(irradiance, 1.0);
}