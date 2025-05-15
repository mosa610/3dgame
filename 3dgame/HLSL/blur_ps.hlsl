#include "fullscreen_quad.hlsli"

cbuffer PARAMETRIC_CONSTANT_BUFFER : register(b3)
{
    float extraction_threshold;
    float gaussian_sigma;
    float bloom_intensity;
    float exposure;
    float extraction_end;
    float3 pad;
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
SamplerState sampler_states[5] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
//    uint mip_level = 0, width, height, number_of_levels;
//    texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels);

//    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
//    float alpha = color.a;

//    float3 blur_color = 0;
//    float gaussian_kernel_total = 0;

//    const int gaussian_half_kernel_size = 3;
//	//const float gaussian_sigma = 1.0;
//	[unroll]
//    for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
//    {
//		[unroll]
//        for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
//        {
//            float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) / (2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);
//            blur_color += texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord + float2(x * 1.0 / width, y * 1.0 / height)).rgb * gaussian_kernel;
//            gaussian_kernel_total += gaussian_kernel;
//        }
//    }
//    blur_color /= gaussian_kernel_total;

//	//const float bloom_intensity = 1.0;
//    color.rgb += blur_color * bloom_intensity;

//#if 1
//	// Tone mapping : HDR -> SDR
//	//const float exposure = 1.2;
//    color.rgb = 1 - exp(-color.rgb * exposure);
//#endif


//#if 1
//	// Gamma process
//    const float GAMMA = 2.2;
//    color.rgb = pow(color.rgb, 1.0 / GAMMA);
//#endif

//    return float4(color.rgb, alpha);
    
    uint mip_level = 0, width, height, number_of_levels;
    texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels);
    
    // ���̉摜
    float4 color = texture_maps[0].Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
    float alpha = color.a;
    
    // �u���[�����i���P�x�e�N�X�`���ɑ΂��āj
    float3 blur_color = 0;
    float gaussian_kernel_total = 0;
    const int gaussian_half_kernel_size = 3;
    
    [unroll]
    for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
    {
        [unroll]
        for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
        {
            float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) / (2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);
            
            // ���P�x�e�N�X�`������T���v�����O
            float3 sample_color = texture_maps[1].Sample(sampler_states[LINEAR_BORDER_BLACK],
                                pin.texcoord + float2(x * 1.0 / width, y * 1.0 / height)).rgb;
            
            blur_color += sample_color * gaussian_kernel;
            gaussian_kernel_total += gaussian_kernel;
        }
    }
    
    // ���K��
    if (gaussian_kernel_total > 0)
        blur_color /= gaussian_kernel_total;
    
    // �u���[�������̉摜�ɉ��Z�i���x�̒�������j
    color.rgb += blur_color * bloom_intensity;
    
    // �g�[���}�b�s���O: HDR -> SDR
    color.rgb = 1 - exp(-color.rgb * exposure);
    
    // �K���}�␳
    const float GAMMA = 2.2;
    color.rgb = pow(max(color.rgb, 0.0001), 1.0 / GAMMA);
    
    return float4(color.rgb, alpha);
}