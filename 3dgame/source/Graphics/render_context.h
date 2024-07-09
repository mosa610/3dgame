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