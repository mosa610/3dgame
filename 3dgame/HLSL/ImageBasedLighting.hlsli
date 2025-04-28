#ifndef __IMAGE_BASED_LIGHTING__
#define __IMAGE_BASED_LIGHTING__

// SamplerState
#define POINT 1
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);

Texture2D skyBox : register(t32);
TextureCube diffuseIem : register(t33);
TextureCube specularPmrem : register(t34);
Texture2D lutGgx : register(t35);

float4 SampleLutGgx(float2 brdfSamplePoint)
{
    return lutGgx.Sample(samplerStates[LINEAR], brdfSamplePoint);
}

float4 SampleSkybox(float3 v, float roughness)
{
    const float PI = 3.14159265358979;
    uint width, height, numberOfLevels;
    skyBox.GetDimensions(0, width, height, numberOfLevels);

    float lod = roughness * float(numberOfLevels - 1);

    v = normalize(v);
    // Blinn/Newell Latitude Mapping
    float2 samplePoint;
    samplePoint.x = (atan2(v.z, v.x) + PI) / (PI * 2.0f);
    samplePoint.y = 1.0f - ((asin(v.y) + PI * 0.5f) / PI);
    return skyBox.SampleLevel(samplerStates[LINEAR], samplePoint, lod);
}

float4 SampleDiffuseIem(float3 v)
{
    return diffuseIem.Sample(samplerStates[LINEAR], v);
}

float4 SampleSpecularPmrem(float3 v, float roughness)
{
    uint width, height, numberOfLevel;
    specularPmrem.GetDimensions(0, width, height, numberOfLevel);

    float lod = roughness * float(numberOfLevel - 1);

    return specularPmrem.SampleLevel(samplerStates[LINEAR], v, lod);
}

#endif
