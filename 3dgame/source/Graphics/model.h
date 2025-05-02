#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <string>
#include "ModelResource.h"
#include "PipeLineState.h"

class Model
{
public:
	Model(ID3D11Device* device, const char* filename, float sampleRate = 60);
	~Model() {}

    struct Node
    {
        std::string			name;
        int                 myIndex = -1;
        int					parentIndex = -1;
        int                 jointIndex = -1;
        DirectX::XMFLOAT3	position = { 0, 0, 0 };
        DirectX::XMFLOAT4	rotation = { 0, 0, 0, 1 };
        DirectX::XMFLOAT3	scale = { 1, 1, 1 };

        DirectX::XMFLOAT4X4	localTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        DirectX::XMFLOAT4X4	globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        DirectX::XMFLOAT4X4	worldTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

        Node* parent = nullptr;
        std::vector<Node*>	children;
    };

    struct NodePose
    {
        DirectX::XMFLOAT3	position = { 0, 0, 0 };
        DirectX::XMFLOAT4	rotation = { 0, 0, 0, 1 };
        DirectX::XMFLOAT3	scale = { 1, 1, 1 };
    };

    // 定数バッファ(データのみGPUへので転送処理などはshaderに任せる)
    struct MaterialData
    {
        //DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };
        //DirectX::XMFLOAT3 emissiveColor = { 0,0,0 };
        //float metallic = 1.0f;
        //float roughness = 1.0f;
        //float normalScale = 0.0f;
        //float occlusionStrength = 0.0f;
        //float alphaCutOff = 0.5f;
        //int   alphaMode = gltf_model_resource::AlphaMode::Opaque;
        //bool  doubleSided = false;
        DirectX::XMFLOAT3 emissiveColor = { 0,0,0 };
        int   alphaMode = ModelResource::AlphaMode::Opaque;
        float alphaCutOff = 0.5f;
        bool  doubleSided = false;

        ModelResource::PbrMetallicRoughness pbrMetallicRoughness;

        ModelResource::NormalTextureInfo    normalTexture;
        ModelResource::OcclusionTextureInfo occlusionTexture;
        ModelResource::TextureInfo          emissiveTexture;
    };

    struct MeshData
    {
        DirectX::XMFLOAT4X4 world;
        int material{ -1 };
        int hasTangent{ 0 };
        int skin{ -1 };
        int padding;
    };

    struct Bone
    {
        int						nodeIndex;
        DirectX::XMFLOAT4X4		offsetTransform;
        Node* node = nullptr;
    };

    static constexpr size_t PRIMITIVE_MAX_JOINTS = 512;
    struct BoneData
    {
        DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
    };

    void CreateStructedBuffer(ID3D11Device* device);
    void UpdateStructedBuffer(ID3D11DeviceContext* dc, const ModelResource::Mesh* mesh = nullptr);

    void Update(float elapsedTime);

    void DrawGUI();

    // ノードデータ取得
    const std::vector<Node>& GetNodes() const { return _nodes; }

    // ルートノード取得
    Node* GetRootNode() { return _nodes.data(); }

    // ノードインデックス取得
    int GetNodeIndex(const char* name) const;

    // トランスフォーム更新処理
    void UpdateTransform();
    void UpdateTransform(const DirectX::XMFLOAT4X4& worldTransform);

    // ノードポーズ設定
    void SetNodePoses(const std::vector<NodePose>& nodePoses);

    // ノードポーズ取得
    void GetNodePoses(std::vector<NodePose>& nodePoses) const;

    // アニメーション計算
    void ComputeAnimation(int animationIndex, int nodeIndex, float time, NodePose& nodePose) const;
    void ComputeAnimation(int animationIndex, float time, std::vector<NodePose>& nodePoses) const;

    bool AddPipelineState(ID3D11Device* device, PipelineStateDesc desc);
public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    _materialResourceView;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                _materialBuffer;

    std::vector<ModelResource::Material::CBuffer>       _materialDates;
    std::vector<Node>                                   _nodes;
    std::vector<NodePose>                               _nodePoses;
    std::shared_ptr<ModelResource>                      _resource;

    DirectX::XMFLOAT4X4                                 _worldTransform;

    PipeLineState                                       state;

    std::unordered_map<int, std::vector<Bone>>      _bones;

    static constexpr const char* DefaultShaderName = "gltf_default_shader";
    static constexpr const char* DefaultInstancingShaderName = "gltf_instancing_default_shader";
    std::unordered_map<std::string, PipeLineState> pipeline_states;
};