#include "ModelRenderer.h"
#include <cstring>
#include <iostream>
#include "GraphicsState.h"

ModelRenderer::ModelRenderer(ID3D11Device* device, ID3D11DeviceContext* dc)
{
    std::string csoFileName;

    //// 頂点シェーダー
    //{
    //    // TODO:セマンティックの統合
    //    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
    //    {
    //        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //        { "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //        { "JOINTS",   0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //        { "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    };
    //    create_vs_from_cso(
    //        device,
    //        ".//Data//Shader//gltf_model_vs.cso",
    //        _vertexShader.ReleaseAndGetAddressOf(),
    //        _inputLayout.ReleaseAndGetAddressOf(),
    //        inputElementDesc,
    //        _countof(inputElementDesc)
    //    );
    //}

    //// ピクセルシェーダー
    //{
    //    create_ps_from_cso(
    //        device,
    //        ".//Data//Shader//gltf_model_ps.cso",
    //        _pixelShader.ReleaseAndGetAddressOf()
    //    );
    //}

    // 定数バッファ
    {
        HRESULT hr = _meshCB.Initialize(device, dc);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = _skeletonCB.Initialize(device, dc);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    {
        _materialCB.Initialize(device, dc, 1);
    }
}

void ModelRenderer::Begin(ID3D11DeviceContext* dc, const RenderContext& rc, Model* model, const char* pipline_state_name)
{
    auto pipeline_state = model->pipeline_states.find(std::string(pipline_state_name));
    if (pipeline_state == model->pipeline_states.end())
    {
        // シェーダーが見つからない場合はデフォルトを使用
        pipeline_state = model->pipeline_states.find(std::string(model->DefaultShaderName));
        if (pipeline_state == model->pipeline_states.end()) {
            // デフォルトのキーも見つからなかった場合の処理
            throw std::runtime_error("Both the requested and default shaders are not found in pipeline_states.");
        }
    }
    {
        // シェーダーセット
        dc->VSSetShader(pipeline_state->second.vertex_shader.Get(), nullptr, 0);
        dc->PSSetShader(pipeline_state->second.pixel_shader.Get(), nullptr, 0);
        dc->IASetInputLayout(pipeline_state->second.input_layout.Get());
        dc->IASetPrimitiveTopology(pipeline_state->second.primitive_toporogy);
    }


    {
        // 定数バッファセット
        dc->VSSetConstantBuffers(0, 1, _meshCB.GetAddressOf());
        dc->PSSetConstantBuffers(0, 1, _meshCB.GetAddressOf());
        dc->VSSetConstantBuffers(2, 1, _skeletonCB.GetAddressOf());
    }

    // 構造化バッファセット
    {
       
    }


    {
        // パイプラインステートセット
        GraphicsState& gs = GraphicsState::GetInstance();
        gs.SetBlendState(dc, BLEND_STATE::NONE);
        gs.SetDepthStencilState(dc, DEPTH_STATE::ZT_ON_ZW_ON);
        gs.SetRasterizerState(dc, RASTER_STATE::CULL_NONE);
        //gs.SetSamplerState(dc, SAMPLER_STATE::LINEAR_BORDER_WHITE);
        gs.SetSamplerState(dc, SAMPLER_STATE::POINT);
        gs.SetSamplerState(dc, SAMPLER_STATE::LINEAR);
        gs.SetSamplerState(dc, SAMPLER_STATE::ANISOTROPIC);
    }
}

void ModelRenderer::Draw(ID3D11DeviceContext* dc, Model* model, float alpha)
{

    dc->PSSetShaderResources(0, 1, model->_materialResourceView.GetAddressOf());

    for (const ModelResource::Mesh& mesh : model->_resource->GetMeshes())
    {
        const ModelResource::Material& material = model->_resource->GetMaterials()[mesh.materialIndex];
        const Model::Node& node = model->GetNodes()[mesh.nodeIndex];
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
        model->UpdateStructedBuffer(dc, &mesh);

        // 頂点バッファ設定
        UINT stride = sizeof(ModelResource::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // スケルトン用定数バッファ更新
        SkeletonCB skeletonCB{};
        auto& bones = model->_bones.at(&mesh);
        if (bones.size() > 0)
        {
            for (size_t i = 0; i < bones.size(); ++i)
            {
                auto& bone = bones.at(i);
                //auto& node = model->_nodes[bone.nodeIndex];
                DirectX::XMMATRIX WorldTransform = XMLoadFloat4x4(&bone.node->worldTransform);
                DirectX::XMMATRIX OffsetTransfoarm = XMLoadFloat4x4(&bone.offsetTransform);
                DirectX::XMMATRIX BoneTransform = OffsetTransfoarm * WorldTransform;
                XMStoreFloat4x4(&skeletonCB.boneTransforms[i], BoneTransform);
            }
        }
        else
        {
            int index = mesh.node->myIndex;
            skeletonCB.boneTransforms[0] = model->_nodes[index].worldTransform;
        }
        _skeletonCB._data = skeletonCB;
        _skeletonCB.Update();

        int skin = (mesh.bones.size() == 0) ? -1 : mesh.nodeIndex;

        DirectX::XMMATRIX   World = XMLoadFloat4x4(&node.globalTransform) * XMLoadFloat4x4(&model->_worldTransform);
        DirectX::XMFLOAT4X4 world;
        DirectX::XMStoreFloat4x4(&world, World);
        _meshCB._data.previous_world = _meshCB._data.world;
        _meshCB._data.world = world;
        _meshCB._data.materialIndex = mesh.materialIndex;
        _meshCB._data.skin = node.jointIndex;
        _meshCB._data.adjustalpha = alpha;
        _meshCB.Update();

        _materialCB._data[0].alphaCutOff = material.data.alphaCutOff;
        _materialCB._data[0].alphaMode = material.data.alphaMode;
        _materialCB._data[0].doubleSided = material.data.doubleSided;
        _materialCB._data[0].emissiveColor = material.data.emissiveColor;
        _materialCB._data[0].pbrMetallicRoughness = material.data.pbrMetallicRoughness;
        _materialCB._data[0].emissiveTexture = material.data.emissiveTexture;
        _materialCB._data[0].normalTexture = material.data.normalTexture;
        _materialCB._data[0].occlusionTexture = material.data.occlusionTexture;
        _materialCB.Update();

        // 描画
        dc->DrawIndexed(static_cast<UINT>(mesh.indices.size()), 0, 0);
    }
}

void ModelRenderer::End(ID3D11DeviceContext* dc)
{
    // シェーダー、インプットレイアウトバインド解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
    dc->IASetInputLayout(nullptr);
}
