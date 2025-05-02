#include "gltf_model_forward.hlsli"

float4 SkinningPosition(float4 position, float4 boneWeights, uint4 boneIndices)
{
    float4 p = float4(0, 0, 0, 0);

	[unroll]
    for (int i = 0; i < 4; i++)
    {
        p += (boneWeights[i] * mul(position, joint_matrices[boneIndices[i]]));
    }
    return p;
}

float4 SkinningVector(float4 vec, float4 boneWeights, uint4 boneIndices)
{
    float4 v = float4(0, 0, 0, 0);

	[unroll]
    for (int i = 0; i < 4; i++)
    {
        v += boneWeights[i] * mul(joint_matrices[boneIndices[i]], vec);
    }
    return v;
}

VS_OUT main(VS_IN vin)
{
	float sigma = vin.tangent.w;
    float4 position = vin.position;
	
    position = SkinningPosition(vin.position, vin.weights, vin.joints);
    vin.normal = SkinningVector(vin.normal, vin.weights, vin.joints);
    vin.tangent = SkinningVector(vin.tangent, vin.weights, vin.joints);

	VS_OUT vout;

	vin.position.w = 1;
    vout.position = mul(position, /*mul(world, view_projection_transform)*/view_projection_transform);
	vout.w_position = mul(vin.position, world);

	vin.normal.w = 0;
	vout.w_normal = normalize(mul(vin.normal, world));

	vin.tangent.w = 0;
	vout.w_tangent = normalize(mul(vin.tangent, world));
	vout.w_tangent.w = sigma;

	vout.texcoord = vin.texcoord;

	// シャドウマップ用のパラメーター計算
	{
		// ライトから見たNDC座標を算出
		float4 wvpPos = mul(vin.position, mul(world, light_view_projection));
		// NDC座標からUV座標を算出する
		wvpPos /= wvpPos.w;
		wvpPos.y = -wvpPos.y;
		wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
		vout.shadow_texcoord = wvpPos.xyz;
    }
	
	return vout;
}
