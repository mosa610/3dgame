#include "Model.h"
#include "Graphics.h"

Model::Model(ID3D11Device* device, const char* filename, float sampleRate)
{
    _resource = std::make_shared<ModelResource>();
    _resource->Load(device, filename, sampleRate);

    // ノード
    const std::vector<ModelResource::Node>& resNodes = _resource->GetNodes();

    _nodes.resize(resNodes.size());
    _nodePoses.resize(resNodes.size());

    for (size_t nodeIndex = 0; nodeIndex < _nodes.size(); ++nodeIndex)
    {
        auto&& src = resNodes[nodeIndex];
        auto&& dst = _nodes[nodeIndex];
        auto&& anim = _nodePoses[nodeIndex];

        dst.name = src.name.c_str();
        dst.parent = src.parentIndex >= 0 ? &_nodes[src.parentIndex] : nullptr;
        dst.myIndex = src.myIndex;
        dst.parentIndex = src.parentIndex;
        dst.jointIndex = src.jointIndex;
        dst.scale = src.scale;
        dst.rotation = src.rotation;
        dst.position = src.position;

        dst.localTransform = src.localTransform;
        dst.globalTransform = src.globalTransform;
        dst.worldTransform = src.worldTransform;

        anim.scale = dst.scale;
        anim.rotation = dst.rotation;
        anim.position = dst.position;

        if (dst.parent != nullptr)
        {
            dst.parent->children.emplace_back(&dst);
        }
    }

    // ボーン
    const std::vector<ModelResource::Mesh>& resMeshes = _resource->GetMeshes();
    for(auto && mesh : resMeshes)
    {
        std::vector<Model::Bone> bones;
        bones.resize(mesh.bones.size());
        for (size_t boneIndex = 0; boneIndex < bones.size(); ++boneIndex)
        {
            auto&& src = mesh.bones.at(boneIndex);
            auto&& dst = bones.at(boneIndex);

            dst.node = &_nodes[src.nodeIndex];
            dst.nodeIndex = src.nodeIndex;
            dst.offsetTransform = src.offsetTransform;
            /*dst.node->name = src.node->name;
            dst.node->parent = src.node->parentIndex >= 0 ? bones.at(src.node->parentIndex).node : nullptr;
            dst.node->myIndex = src.node->myIndex;
            dst.node->parentIndex = src.node->parentIndex;
            dst.node->jointIndex = src.node->jointIndex;
            dst.node->scale = src.node->scale;
            dst.node->rotation = src.node->rotation;
            dst.node->position = src.node->position;

            dst.node->localTransform = src.node->localTransform;
            dst.node->globalTransform = src.node->globalTransform;
            dst.node->worldTransform = src.node->worldTransform;*/

            /*if (dst.node->parent != nullptr)
            {
                dst.node->parent->children.emplace_back(&dst);
            }*/
        }
        _bones.emplace(mesh.index, bones);
    }

    // 行列初期化
    DirectX::XMFLOAT4X4 worldTransform;
    DirectX::XMStoreFloat4x4(&_worldTransform, DirectX::XMMatrixIdentity());
    UpdateTransform(_worldTransform);

    // defaultShader
    {
        PipelineStateDesc desc;
        desc.name = DefaultShaderName;
        desc.vs_path = ".//Data//Shader//gltf_model_gbuffer_vs.cso";
        desc.ps_path = ".//Data//Shader//gltf_model_gbuffer_ps.cso";
        AddPipelineState(device, desc);
    }


    // 構造化バッファ生成
    //for (std::vector<gltf_model_resource::Material>::const_reference material : _resource->GetMaterials())
    //{
    //    _materialDates.push_back(material.data);
    //}
    //HRESULT hr = GpuResourceUtils::CreateStructuredBuffer(
    //    device,
    //    _materialDates,
    //    _materialResourceView.GetAddressOf(),
    //    _materialBuffer);
    //_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    CreateStructedBuffer(device);

}

void Model::CreateStructedBuffer(ID3D11Device* device)
{
    // シェーダー側に輸送？

    // GPU用のシェーダーリソースを用意
    for (std::vector<ModelResource::Material>::const_reference material : _resource->GetMaterials())
    {
        _materialDates.push_back(material.data);
    }

    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ModelResource::Material::CBuffer) * _materialDates.size());
    bufferDesc.StructureByteStride = sizeof(ModelResource::Material::CBuffer);
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pSysMem = _materialDates.data();

    HRESULT hr = device->CreateBuffer(&bufferDesc, &subResourceData, _materialBuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
    shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(_materialDates.size());

    hr = device->CreateShaderResourceView(_materialBuffer.Get(), &shaderResourceViewDesc, _materialResourceView.GetAddressOf());
    //  _resource->materialResourceView -> シェーダーのメンバ->materialResourceView
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Model::UpdateStructedBuffer(ID3D11DeviceContext* dc,const ModelResource::Mesh* mesh)
{
    if (mesh != nullptr)
    {
        _materialDates[0].pbrMetallicRoughness.baseColor = mesh->material->data.pbrMetallicRoughness.baseColor;
        _materialDates[0].pbrMetallicRoughness.metallicFactor = mesh->material->data.pbrMetallicRoughness.metallicFactor;
        _materialDates[0].pbrMetallicRoughness.roughnessFactor = mesh->material->data.pbrMetallicRoughness.roughnessFactor;
        _materialDates[0].emissiveColor = mesh->material->data.emissiveColor;
        _materialDates[0].alphaMode = mesh->material->data.alphaMode;
        _materialDates[0].alphaCutOff = mesh->material->data.alphaCutOff;
        _materialDates[0].doubleSided = mesh->material->data.doubleSided;
        _materialDates[0].emissiveTexture = mesh->material->data.emissiveTexture;
        _materialDates[0].normalTexture = mesh->material->data.normalTexture;
        _materialDates[0].occlusionTexture = mesh->material->data.occlusionTexture;



        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        HRESULT hr = dc->Map(_materialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        // 新しいデータを書き込み
        memcpy(mappedResource.pData, _materialDates.data(), sizeof(MaterialData) * _materialDates.size());

        // マップ解除
        dc->Unmap(_materialBuffer.Get(), 0);
    }
}

void Model::Update(float elapsedTime)
{
    //_materialDates[0].pbrMetallicRoughness.baseColor = { 1,0,1,1 };
    _materialDates[0].emissiveColor = { 0,0,0 };
}

void Model::DrawGUI()
{
    if (ImGui::TreeNode("Model"))
    {
        for (Node& node : _nodes)
        {
            ImGui::SliderFloat3("modelPos", &node.position.x, -10.0f, 10.0f, "%.1f");
            ImGui::SliderFloat3("modelRot", &node.rotation.x, -DirectX::XM_2PI, DirectX::XM_2PI, "%.1f");
            ImGui::SliderFloat3("modelScale", &node.scale.x, 0.0f, 10.0f, "%.1f");
        }
        ImGui::TreePop();
    }
}

// ノードインデックス取得
int Model::GetNodeIndex(const char* name) const
{
    for (size_t nodeIndex = 0; nodeIndex < _nodes.size(); ++nodeIndex)
    {
        if (_nodes[nodeIndex].name == name)
        {
            return static_cast<int>(nodeIndex);
        }
    }
    return -1;
}

void Model::UpdateTransform()
{
    UpdateTransform(_worldTransform);
}

// トランスフォーム更新処理
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform)
{
    DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

    for (Node& node : _nodes)
    {
        // ローカル行列算出
        DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation));
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.position.x, node.position.y, node.position.z);
        DirectX::XMMATRIX LocalTransform = S * R * T;

        // グローバル行列算出
        DirectX::XMMATRIX ParentGlobalTransform;
        if (node.parent != nullptr)
        {
            ParentGlobalTransform = DirectX::XMLoadFloat4x4(&node.parent->globalTransform);
        }
        else
        {
            ParentGlobalTransform = DirectX::XMMatrixIdentity();
        }
        DirectX::XMMATRIX GlobalTransform = LocalTransform * ParentGlobalTransform;

        // ワールド行列算出
        DirectX::XMMATRIX WorldTransform = GlobalTransform * ParentWorldTransform;

        // 計算結果を格納
        DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
        DirectX::XMStoreFloat4x4(&node.globalTransform, GlobalTransform);
        DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
    }
}

void Model::ComputeAnimation(int animationIndex, int nodeIndex, float time, NodePose& nodePose) const
{
    const ModelResource::Animation& animation = _resource->GetAnimations()[animationIndex];
    const ModelResource::NodeAnim& nodeAnim = animation.nodeAnims[nodeIndex];

    // 位置
    for (size_t index = 0; index < nodeAnim.positionKeyframes.size() - 1; ++index)
    {
        // 現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::VectorKeyframe& currentKeyframe = nodeAnim.positionKeyframes[index];
        const ModelResource::VectorKeyframe& nextKeyframe = nodeAnim.positionKeyframes[index + 1];
        if (time >= currentKeyframe.seconds && time <= nextKeyframe.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を算出する
            float rate = (time - currentKeyframe.seconds) / (nextKeyframe.seconds - currentKeyframe.seconds);

            // 前のキーフレームと次のキーフレームの姿勢を補完
            DirectX::XMVECTOR CurrentPosition = DirectX::XMLoadFloat3(&currentKeyframe.value);
            DirectX::XMVECTOR NextPosition = DirectX::XMLoadFloat3(&nextKeyframe.value);
            DirectX::XMVECTOR Position = DirectX::XMVectorLerp(CurrentPosition, NextPosition, rate);
            // 計算結果をノードに格納
            DirectX::XMStoreFloat3(&nodePose.position, Position);
        }
    }
    // 回転
    for (size_t index = 0; index < nodeAnim.rotationKeyframes.size() - 1; ++index)
    {
        // 現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::QuaternionKeyframe& currentKeyframe = nodeAnim.rotationKeyframes[index];
        const ModelResource::QuaternionKeyframe& nextKeyframe = nodeAnim.rotationKeyframes[index + 1];
        if (time >= currentKeyframe.seconds && time <= nextKeyframe.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を算出する
            float rate = (time - currentKeyframe.seconds) / (nextKeyframe.seconds - currentKeyframe.seconds);

            // 前のキーフレームと次のキーフレームの姿勢を補完
            DirectX::XMVECTOR CurrentQuaternion = DirectX::XMLoadFloat4(&currentKeyframe.value);
            DirectX::XMVECTOR NextQuaternion = DirectX::XMLoadFloat4(&nextKeyframe.value);
            DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionSlerp(CurrentQuaternion, NextQuaternion, rate);
            // 計算結果をノードに格納
            DirectX::XMStoreFloat4(&nodePose.rotation, Quaternion);
        }
    }
    // スケール
    for (size_t index = 0; index < nodeAnim.scaleKeyframes.size() - 1; ++index)
    {
        // 現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::VectorKeyframe& currentKeyframe = nodeAnim.scaleKeyframes[index];
        const ModelResource::VectorKeyframe& nextKeyframe = nodeAnim.scaleKeyframes[index + 1];
        if (time >= currentKeyframe.seconds && time <= nextKeyframe.seconds)
        {
            // 再生時間とキーフレームの時間から補完率を算出する
            float rate = (time - currentKeyframe.seconds) / (nextKeyframe.seconds - currentKeyframe.seconds);

            // 前のキーフレームと次のキーフレームの姿勢を補完
            DirectX::XMVECTOR CurrentScale = DirectX::XMLoadFloat3(&currentKeyframe.value);
            DirectX::XMVECTOR NextScale = DirectX::XMLoadFloat3(&nextKeyframe.value);
            DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(CurrentScale, NextScale, rate);
            // 計算結果をノードに格納
            DirectX::XMStoreFloat3(&nodePose.scale, Scale);
        }
    }
}

// アニメーション計算
void Model::ComputeAnimation(int animationIndex, float time, std::vector<NodePose>& nodePoses) const
{
    if (nodePoses.size() != _nodes.size())
    {
        nodePoses.resize(_nodes.size());
    }
    for (size_t nodeIndex = 0; nodeIndex < nodePoses.size(); ++nodeIndex)
    {
        ComputeAnimation(animationIndex, static_cast<int>(nodeIndex), time, nodePoses[nodeIndex]);
    }
}

bool Model::AddPipelineState(ID3D11Device* device, PipelineStateDesc desc)
{
    auto it = pipeline_states.find(desc.name);
    if (it != pipeline_states.end())
        return false;

    //const std::map<std::string, buffer_view>& vertex_buffer_views{ meshes.at(0).primitives.at(0).vertex_buffer_views };
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS",   0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    PipeLineState state;
    if (!desc.vs_path.empty())
        create_vs_from_cso(device, desc.vs_path.data(), state.vertex_shader.ReleaseAndGetAddressOf(), state.input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
    if (!desc.hs_path.empty())
        create_hs_from_cso(device, desc.hs_path.data(), state.hull_shader.ReleaseAndGetAddressOf());
    if (!desc.ds_path.empty())
        create_ds_from_cso(device, desc.ds_path.data(), state.domain_shader.ReleaseAndGetAddressOf());
    if (!desc.gs_path.empty())
        create_gs_from_cso(device, desc.gs_path.data(), state.geometry_shader.ReleaseAndGetAddressOf());
    if (!desc.ps_path.empty())
        create_ps_from_cso(device, desc.ps_path.data(), state.pixel_shader.ReleaseAndGetAddressOf());
    state.primitive_toporogy = desc.primitive_toporogy;

    pipeline_states.insert_or_assign(desc.name, state);
    return true;
}

// ノードポーズ設定
void Model::SetNodePoses(const std::vector<NodePose>& nodePoses)
{
    for (size_t nodeIndex = 0; nodeIndex < _nodes.size(); ++nodeIndex)
    {
        const NodePose& pose = nodePoses[nodeIndex];
        Node& node = _nodes[nodeIndex];

        node.position = pose.position;
        node.rotation = pose.rotation;
        node.scale = pose.scale;
    }
}

// ノードポーズ取得
void Model::GetNodePoses(std::vector<NodePose>& nodePoses) const
{
    if (nodePoses.size() != _nodes.size())
    {
        nodePoses.resize(_nodes.size());
    }
    for (size_t nodeIndex = 0; nodeIndex < _nodes.size(); ++nodeIndex)
    {
        const Node& node = _nodes[nodeIndex];
        NodePose& pose = nodePoses[nodeIndex];

        pose.position = node.position;
        pose.rotation = node.rotation;
        pose.scale = node.scale;
    }
}