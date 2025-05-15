#include "fullscreen_quad.hlsli"

cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b3)
{
    float extraction_threshold;
    float gaussian_sigma;
    float bloom_intensity;
    float exposure;
    float extraction_end;
    float3 pad;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    //float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    //float alpha = color.a;

    //float4 c,ce;
    //color.rgb = step(extraction_threshold, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
    ////c.rgb = step(extraction_threshold, dot(color.rgb, float3(0.299, 0.587, 0.114))) * color.rgb;
    ////ce.rgb = step(dot(color.rgb, float3(0.299, 0.587, 0.114)), extraction_end) * color.rgb;
    ////if(ce.rgb == 0) return float4(c.rgb, alpha);
    
    ////return float4(ce.rgb, alpha);
    //return float4(color.rgb, alpha);
    
    // ���̉摜���擾
    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    float alpha = color.a;
    
    // �P�x���v�Z�i�l�Ԃ̖ڂ̊��x�Ɋ�Â��d�ݕt���j
    float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));
    
    // �X���[�Y�ȍ��P�x���o�i臒l�ƏI���l�̊ԂŃX���[�Y�ɕω��j
    float contribution = smoothstep(extraction_threshold, extraction_end, luminance);
    
    // ���P�x�����̒��o
    float3 bright_color = color.rgb * contribution;
    
    return float4(bright_color, alpha);
}