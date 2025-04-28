#include "sky_box.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    position.z = 1.0f; //  ��ԉ��̐[�x�ŕ`��
    vout.position = position;
    vout.color = color;

    //  NDC���W���烏�[���h���W�֕ϊ�
    vout.world_position = mul(position, inverse_view_projection_transform);
    vout.world_position /= vout.world_position.w;
    vout.world_position.w = 1;

    //  ���x�x�N�g���v�Z�p���W���Z�o
    vout.current_clip_position = vout.position;
    vout.previous_clip_position = mul(vout.world_position, previous_view_projection_transform);
    vout.previous_clip_position /= vout.previous_clip_position.w;
    return vout;
}
