#ifndef __LIGHTS_HLSLI__
#define __LIGHTS_HLSLI__
// ���s����
struct directional_lights
{
    float4 direction;
    float4 color;
};

// �_����
struct point_lights
{
	float4	position;
	float4	color;
	float	range;
	float3	dummy;
};

// �X�|�b�g���C�g
struct spot_lights
{
	float4	position;
	float4	direction;
	float4	color;
	float	range;
	float	innerCorn;
	float	outerCorn;
	float	dummy;
};

// �������C�g
struct hemisphere_lights
{
    float4 sky_color;
    float4 ground_color;
    float weight;
    float3 dummy;
};

#endif	//	__LIGHTS_HLSLI__
