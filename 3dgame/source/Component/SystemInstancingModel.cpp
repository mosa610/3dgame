#include "SystemInstancingModel.h"
#include "..//Graphics/Graphics.h"
#include "..//Graphics/PipelineManager.h"
#include "Register.h"
#include "ComponentBone.h"
#include "ComponentModel.h"
#include "ComponentMaterial.h"
#include "ComponentNode.h"
#include "ComponentTransform.h"
#include "ComponentInstancing.h"
#include "ComponentScene.h"

void SystemInstancingModel::Initialize(Register& reg)
{
    ID3D11Device* device = Graphics::Instance().Get_device();
    ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
    for (Entity e : reg.view<ComponentModel>()) {
        auto& model = reg.getComponent<ComponentModel>(e);

        model.mesh_constant_buffer.Initialize(device, dc);
    }

    for (Entity e : reg.view<ComponentBone>()) {
        auto& bone = reg.getComponent<ComponentBone>(e);

        bone.skeleton_constant_buffer.Initialize(device, dc);
    }
}

void SystemInstancingModel::update(Register& reg, float elapsed_time)
{
}

void SystemInstancingModel::render(Register& reg)
{
    ID3D11Device* device = Graphics::Instance().Get_device();
    ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
    // begin
    {
        begin(reg, device, dc);
    }

    // draw
    {
        draw(reg, dc);
    }

    // end
    {
        end(reg, dc);
    }
}

void SystemInstancingModel::begin(Register& reg, ID3D11Device* device, ID3D11DeviceContext* dc)
{
    // ConstantBuffer Set
    for (Entity e : reg.view<ComponentModel, ComponentBone>()) {
        auto& model = reg.getComponent<ComponentModel>(e);
        auto& bone = reg.getComponent<ComponentBone>(e);

        dc->VSSetConstantBuffers(0, 1, model.mesh_constant_buffer.GetAddressOf());
        dc->PSSetConstantBuffers(0, 1, model.mesh_constant_buffer.GetAddressOf());
        dc->VSSetConstantBuffers(2, 1, bone.skeleton_constant_buffer.GetAddressOf());
    }
}

void SystemInstancingModel::draw(Register& reg, ID3D11DeviceContext* dc)
{
    Entity scene_entity = reg.getEntityByName("Scene");
    if (scene_entity == INVALID_ENTITY) return;

    if (!reg.hasComponent<ComponentInstancingScene>(scene_entity)) return;

    // インスタンシングバッファを更新
    UpdateInstancingBuffers(reg, dc, scene_entity);

    auto& c_instancing_scene = reg.getComponent<ComponentInstancingScene>(scene_entity);

    // モデルタイプ別に描画
    for (const auto& instance_loc : c_instancing_scene.instance_locations) {
        if (instance_loc.count == 0) continue;

        // 該当するモデルタイプのエンティティを取得
        Entity model_entity = INVALID_ENTITY;
        for (Entity e : reg.view<ComponentModel, ComponentInstancing>()) {
            auto& instancing = reg.getComponent<ComponentInstancing>(e);
            if (instancing.model_index == instance_loc.model_index) {
                model_entity = e;
                break;
            }
        }

        if (model_entity == INVALID_ENTITY) continue;

        auto& model = reg.getComponent<ComponentModel>(model_entity);

        // マテリアル設定
        if (reg.hasComponent<ComponentMaterial>(model_entity)) {
            auto& c_material = reg.getComponent<ComponentMaterial>(model_entity);
            UpdateStructedBuffer(model_entity, reg, dc);
            dc->PSSetShaderResources(0, 1, c_material.material_resource_view.GetAddressOf());
        }

        // シェーダー設定（必要に応じて実装）
        // SetInstancingPipelineState(dc, "instancing_shader_name");

        // メッシュ描画
        for (const auto& mesh : model.resource->GetMeshes()) {
            const ModelResource::Material& material = model.resource->GetMaterials()[mesh.materialIndex];

            // テクスチャ設定
            ID3D11ShaderResourceView* nullShaderResourceView{};
            std::vector<ID3D11ShaderResourceView*> shaderResourceViews = {
                material.baseMap.Get(),
                material.metalnessRoughnessMap.Get(),
                material.normalMap.Get(),
                material.emissiveMap.Get(),
                material.occlusionMap.Get()
            };
            dc->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());

            // 頂点バッファ設定
            ID3D11Buffer* vertex_buffers[] = {
                mesh.vertexBuffer.Get(),
                c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),
                c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get()
            };

            UINT strides[] = {
                sizeof(ModelResource::Vertex),
                sizeof(DirectX::XMFLOAT4X4),
                sizeof(DirectX::XMFLOAT4X4)
            };

            UINT offsets[] = { 0, 0, 0 };

            dc->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
            dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            // ボーン処理（必要に応じて）
            if (reg.hasComponent<ComponentBone>(model_entity)) {
                auto& bone = reg.getComponent<ComponentBone>(model_entity);
                // ボーン処理のコード...
            }

            // 定数バッファ更新
            model.mesh_constant_buffer._data.materialIndex = mesh.materialIndex;
            model.mesh_constant_buffer._data.adjustalpha = 1.0f;
            model.mesh_constant_buffer.Update();

            // インスタンシング描画
            dc->DrawIndexedInstanced(
                static_cast<UINT>(mesh.indices.size()),
                static_cast<UINT>(instance_loc.count),
                0,
                0,
                static_cast<UINT>(instance_loc.location)
            );
        }
    }

    // バッファインデックスを切り替え（ダブルバッファリング）
    c_instancing_scene.use_buffer_index = !c_instancing_scene.use_buffer_index;
}

void SystemInstancingModel::end(Register& reg, ID3D11DeviceContext* dc)
{
    // シェーダー、インプットレイアウトバインド解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
    dc->IASetInputLayout(nullptr);
}

void SystemInstancingModel::UpdateStructedBuffer(Entity e, Register& reg, ID3D11DeviceContext* dc)
{
    auto& material = reg.getComponent<ComponentMaterial>(e);

    D3D11_MAPPED_SUBRESOURCE mappedResource{};
    HRESULT hr = dc->Map(material.material_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // 新しいデータを書き込み
    memcpy(mappedResource.pData, material.material_dates.data(), sizeof(MaterialData) * material.material_dates.size());

    // マップ解除
    dc->Unmap(material.material_buffer.Get(), 0);
}

void SystemInstancingModel::UpdateInstancingBuffers(Register& reg, ID3D11DeviceContext* dc, Entity scene_entity)
{
    auto& c_instancing_scene = reg.getComponent<ComponentInstancingScene>(scene_entity);

    if (!c_instancing_scene.is_dirty) return;

    // 全インスタンスデータを収集
    std::vector<InstanceData> all_instance_data;
    std::map<int, std::vector<Entity>> model_groups; // model_index -> entities

    // モデルタイプ別にエンティティをグループ化
    for (Entity e : reg.view<ComponentModel, ComponentInstancing, ComponentTransform>()) {
        auto& instancing = reg.getComponent<ComponentInstancing>(e);
        model_groups[instancing.model_index].push_back(e);
    }

    // ワールド行列配列を構築
    std::vector<DirectX::XMFLOAT4X4> world_matrices;
    std::vector<DirectX::XMFLOAT4X4> previous_world_matrices;
    c_instancing_scene.instance_locations.clear();

    for (auto& [model_index, entities] : model_groups) {
        size_t location = world_matrices.size();

        for (Entity e : entities) {
            auto& transform = reg.getComponent<ComponentTransform>(e);

            // 前フレームの行列を保存
            DirectX::XMFLOAT4X4 previous_world = transform.world_transform;

            // ノード変換も考慮（必要に応じて）
            if (reg.hasComponent<ComponentNode>(e)) {
                auto& c_node = reg.getComponent<ComponentNode>(e);
                // 適切なノード変換を適用
                // DirectX::XMMATRIX World = XMLoadFloat4x4(&node.globalTransform) * XMLoadFloat4x4(&transform.world_transform);
                // XMStoreFloat4x4(&transform.world_transform, World);
            }

            world_matrices.push_back(transform.world_transform);
            previous_world_matrices.push_back(previous_world);
        }

        // インスタンス位置情報を記録
        InstanceLocation loc;
        loc.model_index = model_index;
        loc.count = world_matrices.size() - location;
        loc.location = location;
        c_instancing_scene.instance_locations.push_back(loc);
    }

    // GPUバッファを更新
    if (!world_matrices.empty()) {
        dc->UpdateSubresource(
            c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),
            0, 0, world_matrices.data(), 0, 0
        );

        // 前フレーム用バッファも更新（必要に応じて）
        dc->UpdateSubresource(
            c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get(),
            0, 0, previous_world_matrices.data(), 0, 0
        );
    }

    c_instancing_scene.is_dirty = false;
}
