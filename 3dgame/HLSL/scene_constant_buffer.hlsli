#ifndef __SCENE_CONSTANT_BUFFER_H__
#define __SCENE_CONSTANT_BUFFER_H__

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    float4 options; //	xy : �}�E�X�̍��W�l, z : �^�C�}�[, w : �t���O
    float4 z_buffer_parameteres; // ����`�[�x������`�[�x�֕ϊ����邽�߂̃p�����[�^�[
    float4 camera_position;
    float4 camera_direction;
    float4 camera_clip_distance;
    float4 viewport_size; //  xy : �r���[�|�[�g�T�C�Y, zw : �t�r���[�|�[�g�T�C�Y
    row_major float4x4 view_transform;
    row_major float4x4 projection_transform;
    row_major float4x4 view_projection_transform;
    row_major float4x4 inverse_view_transform;
    row_major float4x4 inverse_projection_transform;
    row_major float4x4 inverse_view_projection_transform;

    row_major float4x4 previous_view_projection_transform;
};

#endif  //  __SCENE_CONSTANT_BUFFER_H__
