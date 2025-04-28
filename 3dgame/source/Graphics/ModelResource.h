#pragma once

#include <string>
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <map>
//#include "SerializeTemplate.h"
#include "..//..//tinygltf-release/tiny_gltf.h"
#include "GpuResourceUtils.h"

class ModelResource
{
public:
	ModelResource();
	virtual ~ModelResource() {}

    static const std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDescs;

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

        template<class Archive>
        void serialize(Archive& archive);
    };

    enum AlphaMode
    {
        Opaque,
        Mask,
        Blend
    };
    struct TextureInfo
    {
        int index = -1;
        int texcoord = 0;

        template<class Archive>
        void serialize(Archive& archive);
    };
    struct NormalTextureInfo
    {
        int index = -1;
        int texcoord = 0;
        float scale = 1;

        template<class Archive>
        void serialize(Archive& archive);
    };
    struct OcclusionTextureInfo
    {
        int index = -1;
        int texcoord = 0;
        float strength = 1;

        template<class Archive>
        void serialize(Archive& archive);
    };
    struct PbrMetallicRoughness
    {
        DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };
        TextureInfo baseColorTexture;
        float metallicFactor = 1;
        float roughnessFactor = 1;
        TextureInfo metallicRoughnessTexture;

        template<class Archive>
        void serialize(Archive& archive);
    };
    struct Material
    {
        std::string			name;
        std::string			baseTextureFileName;
        std::string			normalTextureFileName;
        std::string			emissiveTextureFileName;
        std::string			occlusionTextureFileName;
        std::string			metalnessRoughnessTextureFileName;
        struct CBuffer
        {
            DirectX::XMFLOAT3 emissiveColor = { 0,0,0 };
            int   alphaMode = AlphaMode::Opaque;
            float alphaCutOff = 0.5f;
            bool  doubleSided = false;

            PbrMetallicRoughness pbrMetallicRoughness;

            NormalTextureInfo    normalTexture;
            OcclusionTextureInfo occlusionTexture;
            TextureInfo          emissiveTexture;

            template<class Archive>
            void serialize(Archive& archive);
        };
        CBuffer data;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	baseMap;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	normalMap;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	emissiveMap;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	occlusionMap;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	metalnessRoughnessMap;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct Vertex
    {
        DirectX::XMFLOAT3		position = { 0, 0, 0 };
        DirectX::XMFLOAT3		normal = { 0, 0, 0 };
        DirectX::XMFLOAT4		tangent = { 0, 0, 0, 1 };
        DirectX::XMFLOAT2		texcoord = { 0, 0 };
        DirectX::XMUINT4		boneIndex = { 0, 0, 0, 0 };
        DirectX::XMFLOAT4		boneWeight = { 1, 0, 0, 0 };

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct Bone
    {
        int						nodeIndex;
        DirectX::XMFLOAT4X4		offsetTransform;
        Node* node = nullptr;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct Mesh
    {
        std::vector<Vertex>		vertices;
        std::vector<uint32_t>	indices;
        std::vector<Bone>		bones;
        int			nodeIndex = 0;
        int			materialIndex = 0;
        Material* material = nullptr;
        Node* node = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct VectorKeyframe
    {
        float					seconds;
        DirectX::XMFLOAT3		value;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct QuaternionKeyframe
    {
        float					seconds;
        DirectX::XMFLOAT4		value;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct NodeAnim
    {
        std::vector<VectorKeyframe>		positionKeyframes;
        std::vector<QuaternionKeyframe>	rotationKeyframes;
        std::vector<VectorKeyframe>		scaleKeyframes;

        template<class Archive>
        void serialize(Archive& archive);
    };

    struct Animation
    {
        std::string					name;
        float						secondsLength;
        std::vector<NodeAnim>		nodeAnims;

        template<class Archive>
        void serialize(Archive& archive);
    };

    // �A�j���[�V�����ǉ��ǂݍ���
    void AppendAnimations(const char* filename);

    // �m�[�h�f�[�^�擾
    const std::vector<Node>& GetNodes() { return _nodes; }

    // �}�e���A���f�[�^�擾
    const std::vector<Material>& GetMaterials() const { return _materials; }

    // ���b�V���f�[�^�擾
    const std::vector<Mesh>& GetMeshes() const { return _meshes; }

    // �A�j���[�V�����f�[�^�擾
    const std::vector<Animation>& GetAnimations() const { return _animations; }

    // �A�j���[�V�����C���f�b�N�X�擾
    int GetAnimationIndex(const char* name) const;

public:
    void Load(ID3D11Device* device, const char* filename, float sampleRate = 60);

    // �V���A���C�Y
    void Serialize(const char* filename);

    // �f�V���A���C�Y
    void Deserialize(const char* filename);
private:

    std::vector<Node>		_nodes;
    std::vector<Material>	_materials;
    std::vector<Mesh>		_meshes;
    std::vector<Animation>	_animations;
};