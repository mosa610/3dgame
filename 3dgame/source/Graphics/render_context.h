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
    DirectX::XMFLOAT4X4 view_projection;	//ビュー・プロジェクション変換行列
    DirectX::XMFLOAT4 light_direction;	//ライトの向き
    DirectX::XMFLOAT4 camera_position;
};