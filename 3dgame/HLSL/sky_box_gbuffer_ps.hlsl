#include "sky_box.hlsli"

TextureCube specular_pmrem : register(t0);
SamplerState samplerstate : register(s0);

PSGBufferOut main(VS_OUT pin)
{
    //	�����x�N�g��
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);

	//	mipmap�g���ƕ����̂�mipmap�Ȃ��ɂȂ�悤�ɂ��Ă���
    float3 base_color = specular_pmrem.SampleLevel(samplerstate, E, 0) * pin.color;

    //  GBufferData�ɏo�͏����܂Ƃ߂�
    GBufferData data = (GBufferData)0;
    data.shading_model = shading_model_unlit;
    data.emissive_color = base_color.rgb;
    data.w_position = pin.world_position.xyz;
    data.velocity = calculate_uvspace_velocity(pin.current_clip_position, pin.previous_clip_position);
    return EncodeGBuffer(data, view_projection_transform, z_buffer_parameteres);
}
