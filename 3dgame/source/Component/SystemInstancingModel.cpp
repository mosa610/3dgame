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

    //// begin
    //{
    //    begin(reg, device, dc);
    //}

    // draw
    {
        draw(reg, dc);
    }

    // end
    {
        end(reg, dc);
    }
}

void SystemInstancingModel::begin(Entity e,Register& reg, ID3D11Device* device, ID3D11DeviceContext* dc)
{
    // ConstantBuffer Set
    auto& model = reg.getComponent<ComponentModel>(e);
    auto& bone = reg.getComponent<ComponentBone>(e);

    dc->VSSetConstantBuffers(0, 1, model.mesh_constant_buffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, model.mesh_constant_buffer.GetAddressOf());
    dc->VSSetConstantBuffers(2, 1, bone.skeleton_constant_buffer.GetAddressOf());
}

void SystemInstancingModel::draw(Register& reg, ID3D11DeviceContext* dc)
{
    //Entity scene_entity = reg.getEntityByName("Scene");
    //if (scene_entity == INVALID_ENTITY) return;

    //if (!reg.hasComponent<ComponentInstancingScene>(scene_entity)) return;

    //// インスタンシングバッファを更新
    //UpdateInstancingBuffers(reg, dc, scene_entity);

    //auto& c_instancing_scene = reg.getComponent<ComponentInstancingScene>(scene_entity);

    //// モデルタイプ別に描画
    //for (const auto& instance_loc : c_instancing_scene.instance_locations) {
    //    if (instance_loc.count == 0) continue;

    //    // 該当するモデルタイプのエンティティを取得
    //    Entity model_entity = INVALID_ENTITY;
    //    for (Entity e : reg.view<ComponentModel, ComponentInstancing>()) {
    //        auto& instancing = reg.getComponent<ComponentInstancing>(e);
    //        if (instancing.model_index == instance_loc.model_index) {
    //            model_entity = e;
    //            begin(e,reg, Graphics::Instance().Get_device(), dc);
    //            break;
    //        }
    //    }

    //    if (model_entity == INVALID_ENTITY) continue;

    //    auto& model = reg.getComponent<ComponentModel>(model_entity);

    //    // マテリアル設定
    //    if (reg.hasComponent<ComponentMaterial>(model_entity)) {
    //        auto& c_material = reg.getComponent<ComponentMaterial>(model_entity);
    //        UpdateStructedBuffer(model_entity, reg, dc);
    //        dc->PSSetShaderResources(0, 1, c_material.material_resource_view.GetAddressOf());
    //    }

    //    // シェーダー設定（必要に応じて実装）
    //    // SetInstancingPipelineState(dc, "instancing_shader_name");

    //    // メッシュ描画
    //    for (const auto& mesh : model.resource->GetMeshes()) {
    //        const ModelResource::Material& material = model.resource->GetMaterials()[mesh.materialIndex];

    //        // テクスチャ設定
    //        ID3D11ShaderResourceView* nullShaderResourceView{};
    //        std::vector<ID3D11ShaderResourceView*> shaderResourceViews = {
    //            material.baseMap.Get(),
    //            material.metalnessRoughnessMap.Get(),
    //            material.normalMap.Get(),
    //            material.emissiveMap.Get(),
    //            material.occlusionMap.Get()
    //        };
    //        dc->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());

    //        // 頂点バッファ設定
    //        ID3D11Buffer* vertex_buffers[] = {
    //            mesh.vertexBuffer.Get(),
    //            c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),
    //            c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get()
    //        };

    //        UINT strides[] = {
    //            sizeof(ModelResource::Vertex),
    //            sizeof(DirectX::XMFLOAT4X4),
    //            sizeof(DirectX::XMFLOAT4X4)
    //        };

    //        UINT offsets[] = { 0, 0, 0 };

    //        dc->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
    //        dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    //        const ComponentNode& c_node = reg.getComponent<ComponentNode>(model_entity);
    //        const Node& node = c_node.nodes[mesh.nodeIndex];
    //        // ボーン処理（必要に応じて）
    //        if (reg.hasComponent<ComponentBone>(model_entity)) {
    //            auto& bone = reg.getComponent<ComponentBone>(model_entity);
    //            // ボーン処理のコード...
    //             // スケルトン用定数バッファ更新
    //            SkeletonCB skeletonCB{};
    //            auto& bones = bone.bones.at(mesh.index);
    //            if (bones.size() > 0)
    //            {
    //                for (size_t i = 0; i < bones.size(); ++i)
    //                {
    //                    // bone行列
    //                    auto& bone = bones.at(i);
    //                    DirectX::XMMATRIX WorldTransform = XMLoadFloat4x4(&bone.node->worldTransform);
    //                    DirectX::XMMATRIX OffsetTransfoarm = XMLoadFloat4x4(&bone.offsetTransform);
    //                    DirectX::XMMATRIX BoneTransform = OffsetTransfoarm * WorldTransform;
    //                    XMStoreFloat4x4(&skeletonCB.boneTransforms[i], BoneTransform);
    //                }
    //            }
    //            else
    //            {
    //                int index = mesh.node->myIndex;
    //                skeletonCB.boneTransforms[0] = c_node.nodes[index].worldTransform;
    //            }
    //            bone.skeleton_constant_buffer._data = skeletonCB;
    //            bone.skeleton_constant_buffer.Update();
    //        }

    //        // 定数バッファ更新
    //        auto& transform = reg.getComponent<ComponentTransform>(model_entity);
    //        DirectX::XMMATRIX   World = XMLoadFloat4x4(&node.globalTransform) * XMLoadFloat4x4(&transform.world_transform);
    //        DirectX::XMFLOAT4X4 world;
    //        DirectX::XMStoreFloat4x4(&world, World);
    //        model.mesh_constant_buffer._data.previous_world = model.mesh_constant_buffer._data.world;
    //        model.mesh_constant_buffer._data.world = world;
    //        model.mesh_constant_buffer._data.materialIndex = mesh.materialIndex;
    //        model.mesh_constant_buffer._data.skin = node.jointIndex;
    //        model.mesh_constant_buffer._data.adjustalpha = 1.0f;
    //        model.mesh_constant_buffer.Update();

    //        // インスタンシング描画
    //        dc->DrawIndexedInstanced(
    //            static_cast<UINT>(mesh.indices.size()),
    //            static_cast<UINT>(instance_loc.count),
    //            0,
    //            0,
    //            static_cast<UINT>(instance_loc.location)
    //        );
    //    }
    //}

    //// バッファインデックスを切り替え（ダブルバッファリング）
    //c_instancing_scene.use_buffer_index = !c_instancing_scene.use_buffer_index;

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
                begin(e, reg, Graphics::Instance().Get_device(), dc);
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
    
        // メッシュ描画
        for (const auto& mesh : model.resource->GetMeshes()) {
            const ModelResource::Material& material = model.resource->GetMaterials()[mesh.materialIndex];
    
            // テクスチャ設定
            std::vector<ID3D11ShaderResourceView*> shaderResourceViews = {
                material.baseMap.Get(),
                material.metalnessRoughnessMap.Get(),
                material.normalMap.Get(),
                material.emissiveMap.Get(),
                material.occlusionMap.Get()
            };
            dc->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());
    
            // 【修正】頂点バッファ設定 - 正しい順序でバッファを設定
            ID3D11Buffer* vertex_buffers[] = {
                mesh.vertexBuffer.Get(),                                                                    // スロット0: 頂点データ
                c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),      // スロット1: 現在のワールド行列
                c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get()     // スロット2: 前フレームのワールド行列
            };
    
            UINT strides[] = {
                sizeof(ModelResource::Vertex),     // 頂点データのサイズ
                sizeof(DirectX::XMFLOAT4X4),      // ワールド行列のサイズ
                sizeof(DirectX::XMFLOAT4X4)       // 前フレームワールド行列のサイズ
            };
    
            UINT offsets[] = {
                0,                                              // 頂点データのオフセット
                static_cast<UINT>(instance_loc.location * sizeof(DirectX::XMFLOAT4X4)),  // 【修正】インスタンス開始位置のオフセット
                static_cast<UINT>(instance_loc.location * sizeof(DirectX::XMFLOAT4X4))   // 【修正】前フレーム用のオフセット
            };

            Graphics::Instance().debugLog("    Setting vertex buffers: offsets=[%u, %u, %u], strides=[%u, %u, %u]\n",
                offsets[0], offsets[1], offsets[2], strides[0], strides[1], strides[2]);

    
            dc->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
            dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    
            // 【修正】ボーン処理 - インスタンシングでは代表的なボーン情報のみ使用
            if (reg.hasComponent<ComponentBone>(model_entity)) {
                auto& bone = reg.getComponent<ComponentBone>(model_entity);
                const ComponentNode& c_node = reg.getComponent<ComponentNode>(model_entity);
    
                SkeletonCB skeletonCB{};
                auto& bones = bone.bones.at(mesh.index);
                if (bones.size() > 0) {
                    for (size_t i = 0; i < bones.size(); ++i) {
                        auto& bone_data = bones.at(i);
                        DirectX::XMMATRIX WorldTransform = XMLoadFloat4x4(&bone_data.node->worldTransform);
                        DirectX::XMMATRIX OffsetTransform = XMLoadFloat4x4(&bone_data.offsetTransform);
                        DirectX::XMMATRIX BoneTransform = OffsetTransform * WorldTransform;
                        XMStoreFloat4x4(&skeletonCB.boneTransforms[i], BoneTransform);
                    }
                }
                else {
                    int index = mesh.node->myIndex;
                    skeletonCB.boneTransforms[0] = c_node.nodes[index].worldTransform;
                }
                bone.skeleton_constant_buffer._data = skeletonCB;
                bone.skeleton_constant_buffer.Update();
            }
    
            // 【修正】定数バッファ更新 - インスタンシング用に単位行列を設定
            const ComponentNode& c_node = reg.getComponent<ComponentNode>(model_entity);
            const Node& node = c_node.nodes[mesh.nodeIndex];
    
            // インスタンシング描画では、ワールド変換はインスタンスデータで行うため、
            // 定数バッファのworldには単位行列またはノード変換のみを設定
            DirectX::XMMATRIX World = XMLoadFloat4x4(&node.globalTransform);  // 【修正】エンティティ固有の変換は除外
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, World);
    
            model.mesh_constant_buffer._data.previous_world = model.mesh_constant_buffer._data.world;
            model.mesh_constant_buffer._data.world = world;
            model.mesh_constant_buffer._data.materialIndex = mesh.materialIndex;
            model.mesh_constant_buffer._data.skin = node.jointIndex;
            model.mesh_constant_buffer._data.adjustalpha = 1.0f;
            model.mesh_constant_buffer.Update();
    
            Graphics::Instance().debugLog("    DrawIndexedInstanced: indices=%zu, instances=%zu\n",
                mesh.indices.size(), instance_loc.count);

            // インスタンシング描画
            dc->DrawIndexedInstanced(
                static_cast<UINT>(mesh.indices.size()),      // インデックス数
                static_cast<UINT>(instance_loc.count),       // インスタンス数
                0,                                           // インデックスバッファの開始位置
                0,                                           // 頂点バッファの開始位置
                0                                            // 【修正】インスタンス開始位置（オフセットで管理）
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
    //auto& c_instancing_scene = reg.getComponent<ComponentInstancingScene>(scene_entity);

    //if (!c_instancing_scene.is_dirty) return;

    //// 全インスタンスデータを収集
    //std::vector<InstanceData> all_instance_data;
    //std::map<int, std::vector<Entity>> model_groups; // model_index -> entities

    //// モデルタイプ別にエンティティをグループ化
    //for (Entity e : reg.view<ComponentModel, ComponentInstancing, ComponentTransform>()) {
    //    auto& instancing = reg.getComponent<ComponentInstancing>(e);
    //    model_groups[instancing.model_index].push_back(e);
    //}

    //// ワールド行列配列を構築
    //std::vector<DirectX::XMFLOAT4X4> world_matrices;
    //std::vector<DirectX::XMFLOAT4X4> previous_world_matrices;
    //c_instancing_scene.instance_locations.clear();

    //for (auto& [model_index, entities] : model_groups) {
    //    size_t location = world_matrices.size();

    //    for (Entity e : entities) {
    //        auto& transform = reg.getComponent<ComponentTransform>(e);

    //        // 前フレームの行列を保存
    //        DirectX::XMFLOAT4X4 previous_world = transform.world_transform;

    //        // ノード変換も考慮（必要に応じて）
    //        if (reg.hasComponent<ComponentNode>(e)) {
    //            auto& c_node = reg.getComponent<ComponentNode>(e);
    //            // 適切なノード変換を適用
    //            // DirectX::XMMATRIX World = XMLoadFloat4x4(&node.globalTransform) * XMLoadFloat4x4(&transform.world_transform);
    //            // XMStoreFloat4x4(&transform.world_transform, World);
    //        }

    //        world_matrices.push_back(transform.world_transform);
    //        previous_world_matrices.push_back(previous_world);
    //    }

    //    // インスタンス位置情報を記録
    //    InstanceLocation loc;
    //    loc.model_index = model_index;
    //    loc.count = world_matrices.size() - location;
    //    loc.location = location;
    //    c_instancing_scene.instance_locations.push_back(loc);
    //}

    //// GPUバッファを更新
    //if (!world_matrices.empty()) {
    //    dc->UpdateSubresource(
    //        c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),
    //        0, 0, world_matrices.data(), 0, 0
    //    );

    //    // 前フレーム用バッファも更新（必要に応じて）
    //    dc->UpdateSubresource(
    //        c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get(),
    //        0, 0, previous_world_matrices.data(), 0, 0
    //    );
    //}

    //c_instancing_scene.is_dirty = false;

    //auto& c_instancing_scene = reg.getComponent<ComponentInstancingScene>(scene_entity);

    //if (!c_instancing_scene.is_dirty) return;

    //// モデルタイプ別にエンティティをグループ化
    //std::map<int, std::vector<Entity>> model_groups;

    //for (Entity e : reg.view<ComponentModel, ComponentInstancing, ComponentTransform>()) {
    //    auto& instancing = reg.getComponent<ComponentInstancing>(e);
    //    model_groups[instancing.model_index].push_back(e);
    //}

    //// 全インスタンスのワールド行列を格納する配列
    //std::vector<DirectX::XMFLOAT4X4> current_world_matrices;
    //std::vector<DirectX::XMFLOAT4X4> previous_world_matrices;
    //c_instancing_scene.instance_locations.clear();

    //// モデルタイプごとにインスタンスデータを構築
    //for (auto& [model_index, entities] : model_groups) {
    //    if (entities.empty()) continue;

    //    size_t start_location = current_world_matrices.size();

    //    // 各エンティティのワールド行列を収集
    //    for (Entity e : entities) {
    //        auto& transform = reg.getComponent<ComponentTransform>(e);

    //        // 現在のワールド行列を追加
    //        current_world_matrices.push_back(transform.world_transform);

    //        // 【修正】前フレームの行列を管理（実際の実装では、前フレームの値を保存する仕組みが必要）
    //        // 今回は簡単のため、現在の値をそのまま使用
    //        previous_world_matrices.push_back(transform.world_transform);
    //    }

    //    // インスタンス位置情報を記録
    //    InstanceLocation loc;
    //    loc.model_index = model_index;
    //    loc.count = entities.size();
    //    loc.location = start_location;
    //    c_instancing_scene.instance_locations.push_back(loc);
    //}

    //// 【修正】GPUバッファを正しいパラメータで更新
    //if (!current_world_matrices.empty()) {
    //    // 現在フレームのワールド行列バッファを更新
    //    dc->UpdateSubresource(
    //        c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),
    //        0,                                                              // サブリソースインデックス
    //        nullptr,                                                        // 更新領域（全体）
    //        current_world_matrices.data(),                                  // データポインタ
    //        0,                                                              // 行ピッチ（3Dテクスチャ用、今回は使用しない）
    //        0                                                               // 深度ピッチ（3Dテクスチャ用、今回は使用しない）
    //    );

    //    // 前フレームのワールド行列バッファを更新
    //    dc->UpdateSubresource(
    //        c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get(),
    //        0,
    //        nullptr,
    //        previous_world_matrices.data(),
    //        0,
    //        0
    //    );
    //}

    //c_instancing_scene.is_dirty = false;

    auto& c_instancing_scene = reg.getComponent<ComponentInstancingScene>(scene_entity);
    
    if (!c_instancing_scene.is_dirty) return;
    
    // モデルタイプ別にエンティティをグループ化
    std::map<int, std::vector<Entity>> model_groups;
    
    for (Entity e : reg.view<ComponentModel, ComponentInstancing, ComponentTransform>()) {
        auto& instancing = reg.getComponent<ComponentInstancing>(e);
        model_groups[instancing.model_index].push_back(e);
    }
    
    // 全インスタンスのワールド行列を格納する配列
    std::vector<DirectX::XMFLOAT4X4> current_world_matrices;
    std::vector<DirectX::XMFLOAT4X4> previous_world_matrices;
    c_instancing_scene.instance_locations.clear();
    
    // モデルタイプごとにインスタンスデータを構築
    for (auto& [model_index, entities] : model_groups) {
        if (entities.empty()) continue;
    
        size_t start_location = current_world_matrices.size();
    
        // 各エンティティのワールド行列を収集
        for (Entity e : entities) {
            auto& transform = reg.getComponent<ComponentTransform>(e);
    
            // デバッグ: 行列の値を確認
            /*printf("Entity %d: World Matrix = [%.2f, %.2f, %.2f, %.2f]\n",
                e, transform.world_transform._11, transform.world_transform._12,
                transform.world_transform._13, transform.world_transform._14);*/

            Graphics::Instance().debugLog("Entity %d: World Matrix = [%.2f, %.2f, %.2f, %.2f]\n",
                e, transform.world_transform._11, transform.world_transform._12,
                transform.world_transform._13, transform.world_transform._14);
    
            current_world_matrices.push_back(transform.world_transform);
            previous_world_matrices.push_back(transform.world_transform);
        }
    
        // インスタンス位置情報を記録
        InstanceLocation loc;
        loc.model_index = model_index;
        loc.count = entities.size();
        loc.location = start_location;
        c_instancing_scene.instance_locations.push_back(loc);
    }
    
    // バッファサイズの確認とGPU更新
    if (!current_world_matrices.empty()) {
        size_t required_size = current_world_matrices.size() * sizeof(DirectX::XMFLOAT4X4);
    
        // バッファの最大サイズを確認（1000個 * sizeof(DirectX::XMFLOAT4X4)）
        const size_t MAX_BUFFER_SIZE = 1000 * sizeof(DirectX::XMFLOAT4X4);
    
        if (required_size > MAX_BUFFER_SIZE) {
            Graphics::Instance().debugLog("ERROR: Required size (%zu) exceeds buffer capacity (%zu)!\n",
                required_size, MAX_BUFFER_SIZE);

            Graphics::Instance().debugLog("Instance count: %zu, Max instances: 1000\n", current_world_matrices.size());
            return;
        }
    
        Graphics::Instance().debugLog("Updating %zu instances (%zu bytes) using UpdateSubresource\n",
            current_world_matrices.size(), required_size);

    
        // D3D11_USAGE_DEFAULT バッファには UpdateSubresource を使用
        dc->UpdateSubresource(
            c_instancing_scene.world_matrices_buffer[c_instancing_scene.use_buffer_index].Get(),
            0,                                          // サブリソースインデックス
            nullptr,                                    // 更新領域（全体）
            current_world_matrices.data(),              // データポインタ
            0,                                          // 行ピッチ（1Dバッファでは0）
            0                                           // 深度ピッチ（1Dバッファでは0）
        );
    
        // 前フレームのワールド行列バッファを更新
        dc->UpdateSubresource(
            c_instancing_scene.world_matrices_buffer[!c_instancing_scene.use_buffer_index].Get(),
            0,
            nullptr,
            previous_world_matrices.data(),
            0,
            0
        );
    
        Graphics::Instance().debugLog("Buffers updated successfully with UpdateSubresource\n");
    }
    else {
        Graphics::Instance().debugLog("No instances to update\n");
    }
    
    c_instancing_scene.is_dirty = false;
}
