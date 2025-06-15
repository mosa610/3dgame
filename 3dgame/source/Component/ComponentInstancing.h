#pragma once

#include <d3d11.h>
#include <wrl.h>

struct InstanceLocation
{
    int model_index = 0;
    size_t count = 0;
    size_t location = 0;
};

// インスタンシングコンポーネント（各モデルエンティティに付ける）
struct ComponentInstancing
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> instancing_world_matrix_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> instancing_previous_world_matrices_buffer;
    int model_index = 0; // どのモデルタイプかを識別
};

// シーンのインスタンシング管理コンポーネント（Sceneエンティティに付ける）
struct ComponentInstancingScene
{
    std::vector<InstanceLocation> instance_locations;
    Microsoft::WRL::ComPtr<ID3D11Buffer> world_matrices_buffer[2]; // ダブルバッファ
    int use_buffer_index = 0;
    bool is_dirty = true;
};

// 各インスタンスのデータ
struct InstanceData
{
    int model_index = 0;
    Entity entity = INVALID_ENTITY;
    DirectX::XMFLOAT4X4 world_matrix;
    DirectX::XMFLOAT4X4 previous_world_matrix;
};