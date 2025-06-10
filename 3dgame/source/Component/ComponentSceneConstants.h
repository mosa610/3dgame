#pragma once
#include <DirectXMath.h>
#include <memory>
#include <d3d11.h>
#include <wrl.h>

struct SceneConstants
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

struct ComponentSceneConstants
{
    std::unique_ptr<SceneConstants> scene_constants;
    std::unique_ptr<SceneConstants> shadow_constants;

    Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> shadow_constant_buffer;
};