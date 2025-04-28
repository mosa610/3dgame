#include "..//sprite.hlsli"
#include "..//shading_functions.hlsli"

Texture2D color_map : register(t0);
SamplerState linear_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(linear_sampler_state, pin.texcoord);
    //  @remainder  �V�F�[�f�B���O���f���ɉ����ăK���}�␳�؂�̖Y��Ă��I
    color.rgb = pow(color.rgb, 1.0f / GammaFactor);
    return color;
}

