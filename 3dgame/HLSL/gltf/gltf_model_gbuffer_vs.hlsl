#include "gltf_model.hlsli"

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
    if (skin > -1)
    {
        //row_major float4x4 skin_matrix =
        //    vin.weights.x * joint_matrices[vin.joints.x] +
        //    vin.weights.y * joint_matrices[vin.joints.y] +
        //    vin.weights.z * joint_matrices[vin.joints.z] +
        //    vin.weights.w * joint_matrices[vin.joints.w];
        //vin.position = mul(float4(vin.position.xyz, 1), skin_matrix);
        //vin.normal = normalize(mul(float4(vin.normal.xyz, 0), skin_matrix));
        //vin.tangent = normalize(mul(float4(vin.tangent.xyz, 0), skin_matrix));
        
        position = SkinningPosition(vin.position, vin.weights, vin.joints);
        //vin.position = mul(SkinningPosition(vin.position, vin.weights, vin.joints), world);
        //vin.normal = SkinningVector(vin.normal, vin.weights, vin.joints);
        //vin.tangent = SkinningVector(vin.tangent, vin.weights, vin.joints);
    }

    VS_OUT vout = (VS_OUT)0;

    //vin.position.w = 1;
    vout.position = mul(position, mul(world, view_projection_transform));
    vout.w_position = position;//mul(vin.position, world);

    vout.current_clip_position = vout.position;
    vout.previous_clip_position = mul(vin.position, mul(previous_world, previous_view_projection_transform));

    vin.normal.w = 0;
    vout.w_normal = normalize(mul(vin.normal, world));

    vin.tangent.w = 0;
    vout.w_tangent = normalize(mul(vin.tangent, world));
    vout.w_tangent.w = sigma;

    vout.texcoord = vin.texcoord;
	
    return vout;
}
