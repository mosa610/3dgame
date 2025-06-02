#include "SystemModel.h"
#include "..//Graphics/PipeLineState.h"
#include "Register.h"
#include "ComponentBone.h"
#include "ComponentModel.h"
#include "ComponentMaterial.h"
#include "ComponentNode.h"
#include "ComponentTransform.h"

void SystemModel::Initialize(Register& reg)
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

void SystemModel::begin(Register& reg, ID3D11Device* device, ID3D11DeviceContext* dc)
{
    PipelineStateDesc desc;
    desc.id = 0;
    desc.vs_path = ".//Data//Shader//gltf_model_gbuffer_vs.cso";
    desc.ps_path = ".//Data//Shader//gltf_model_gbuffer_ps.cso";
    PipelineManager::instance().addPipelineState(device, desc);

    // Pipeline Set
    PipelineManager::instance().setPipelineState(0, dc);

    // ConstantBuffer Set
    for (Entity e : reg.view<ComponentModel, ComponentBone>()) {
        auto& model = reg.getComponent<ComponentModel>(e);
        auto& bone = reg.getComponent<ComponentBone>(e);

        dc->VSSetConstantBuffers(0, 1, model.mesh_constant_buffer.GetAddressOf());
        dc->PSSetConstantBuffers(0, 1, model.mesh_constant_buffer.GetAddressOf());
        dc->VSSetConstantBuffers(2, 1, bone.skeleton_constant_buffer.GetAddressOf());
    }
}

void SystemModel::draw(Register& reg, ID3D11DeviceContext* dc)
{
    for(Entity e : reg.view<ComponentModel>()) {
        auto& model = reg.getComponent<ComponentModel>(e);

        // Material
        if (reg.hasComponent<ComponentMaterial>(e)) {
            auto& c_material = reg.getComponent<ComponentMaterial>(e);

            // material_resource Set
            dc->PSSetShaderResources(0,1, c_material.material_resource_view.GetAddressOf());
        }

        // Mesh
        for (const auto& mesh : model.resource->GetMeshes())
        {
            const ModelResource::Material& material = model.resource->GetMaterials()[mesh.materialIndex];
            const ComponentNode& c_node = reg.getComponent<ComponentNode>(e);
            const Node& node = c_node.nodes[mesh.nodeIndex];
            // テクスチャ読み込み
            ID3D11ShaderResourceView* nullShaderResourceView{};
            std::vector<ID3D11ShaderResourceView*> shaderResourceViews = {
                material.baseMap.Get(),
                material.metalnessRoughnessMap.Get(),
                material.normalMap.Get(),
                material.emissiveMap.Get(),
                material.occlusionMap.Get()
            };
            dc->PSSetShaderResources(1, static_cast<UINT>(shaderResourceViews.size()), shaderResourceViews.data());
            UpdateStructedBuffer(e, reg, dc, &mesh);

            // 頂点バッファ設定
            UINT stride = sizeof(ModelResource::Vertex);
            UINT offset = 0;
            dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
            dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            // bone
            if (reg.hasComponent<ComponentBone>(e)) {
                auto& bone = reg.getComponent<ComponentBone>(e);

                // スケルトン用定数バッファ更新
                SkeletonCB skeletonCB{};
                auto& bones = bone.bones.at(mesh.index);
                if (bones.size() > 0)
                {
                    for (size_t i = 0; i < bones.size(); ++i)
                    {
                        // bone行列
                        auto& bone = bones.at(i);
                        DirectX::XMMATRIX WorldTransform = XMLoadFloat4x4(&bone.node->worldTransform);
                        DirectX::XMMATRIX OffsetTransfoarm = XMLoadFloat4x4(&bone.offsetTransform);
                        DirectX::XMMATRIX BoneTransform = OffsetTransfoarm * WorldTransform;
                        XMStoreFloat4x4(&skeletonCB.boneTransforms[i], BoneTransform);
                    }
                }
                else
                {
                    int index = mesh.node->myIndex;
                    skeletonCB.boneTransforms[0] =c_node.nodes[index].worldTransform;
                }
                bone.skeleton_constant_buffer._data = skeletonCB;
                bone.skeleton_constant_buffer.Update();

            }

            auto& transform = reg.getComponent<ComponentTransform>(e);
            DirectX::XMMATRIX   World = XMLoadFloat4x4(&node.globalTransform) * XMLoadFloat4x4(&transform.world_transform);
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, World);
            model.mesh_constant_buffer._data.previous_world = model.mesh_constant_buffer._data.world;
            model.mesh_constant_buffer._data.world = world;
            model.mesh_constant_buffer._data.materialIndex = mesh.materialIndex;
            model.mesh_constant_buffer._data.skin = node.jointIndex;
            model.mesh_constant_buffer._data.adjustalpha = 1.0f;
            model.mesh_constant_buffer.Update();

            // 描画
            dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
        }
    }
}

void SystemModel::end(Register& reg, ID3D11DeviceContext* dc)
{
    // シェーダー、インプットレイアウトバインド解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
    dc->IASetInputLayout(nullptr);
}

void SystemModel::UpdateStructedBuffer(Entity e, Register& reg, ID3D11DeviceContext* dc, const ModelResource::Mesh* mesh)
{
    auto& material = reg.getComponent<ComponentMaterial>(e);

    if (mesh != nullptr)
    {
        material.material_dates[0].pbrMetallicRoughness.baseColor = mesh->material->data.pbrMetallicRoughness.baseColor;
        material.material_dates[0].pbrMetallicRoughness.metallicFactor = mesh->material->data.pbrMetallicRoughness.metallicFactor;
        material.material_dates[0].pbrMetallicRoughness.roughnessFactor = mesh->material->data.pbrMetallicRoughness.roughnessFactor;
        material.material_dates[0].emissiveColor = mesh->material->data.emissiveColor;
        material.material_dates[0].alphaMode = mesh->material->data.alphaMode;
        material.material_dates[0].alphaCutOff = mesh->material->data.alphaCutOff;
        material.material_dates[0].doubleSided = mesh->material->data.doubleSided;
        material.material_dates[0].emissiveTexture = mesh->material->data.emissiveTexture;
        material.material_dates[0].normalTexture = mesh->material->data.normalTexture;
        material.material_dates[0].occlusionTexture = mesh->material->data.occlusionTexture;


        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        HRESULT hr = dc->Map(material.material_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 新しいデータを書き込み
        memcpy(mappedResource.pData, material.material_dates.data(), sizeof(MaterialData) * material.material_dates.size());

        // マップ解除
        dc->Unmap(material.material_buffer.Get(), 0);
    }
}
