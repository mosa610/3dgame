#pragma once

#include <DirectXMath.h>

struct RenderContext
{
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;
};

struct scene_constants
{
    DirectX::XMFLOAT4X4 view_projection;	//�r���[�E�v���W�F�N�V�����ϊ��s��
    DirectX::XMFLOAT4 light_direction;	//���C�g�̌���
    DirectX::XMFLOAT4 camera_position;
};

struct gbuffer_scene_constants
{
    DirectX::XMFLOAT4 options;              //  xy : �}�E�X�̍��W�l, z : �^�C�}�[, w : �t���O
    DirectX::XMFLOAT4 z_buffer_parameteres; // ����`�[�x������`�[�x�֕ϊ����邽�߂̃p�����[�^�[
    DirectX::XMFLOAT4 camera_position;
    DirectX::XMFLOAT4 camera_direction;
    DirectX::XMFLOAT4 camera_clip_distance;
    DirectX::XMFLOAT4 viewport_size;        //  xy : �r���[�|�[�g�T�C�Y, zw : �t�r���[�|�[�g�T�C�Y
    DirectX::XMFLOAT4X4 view_transform;
    DirectX::XMFLOAT4X4 projection_transform;
    DirectX::XMFLOAT4X4 view_projection_transform;
    DirectX::XMFLOAT4X4 inverse_view_transform;
    DirectX::XMFLOAT4X4 inverse_projection_transform;
    DirectX::XMFLOAT4X4 inverse_view_projection_transform;

    DirectX::XMFLOAT4X4 previous_view_projection_transform;
};