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

struct gbuffer_scene_constants
{
    DirectX::XMFLOAT4 options;              //  xy : マウスの座標値, z : タイマー, w : フラグ
    DirectX::XMFLOAT4 z_buffer_parameteres; // 非線形深度から線形深度へ変換するためのパラメーター
    DirectX::XMFLOAT4 camera_position;
    DirectX::XMFLOAT4 camera_direction;
    DirectX::XMFLOAT4 camera_clip_distance;
    DirectX::XMFLOAT4 viewport_size;        //  xy : ビューポートサイズ, zw : 逆ビューポートサイズ
    DirectX::XMFLOAT4X4 view_transform;
    DirectX::XMFLOAT4X4 projection_transform;
    DirectX::XMFLOAT4X4 view_projection_transform;
    DirectX::XMFLOAT4X4 inverse_view_transform;
    DirectX::XMFLOAT4X4 inverse_projection_transform;
    DirectX::XMFLOAT4X4 inverse_view_projection_transform;

    DirectX::XMFLOAT4X4 previous_view_projection_transform;
};