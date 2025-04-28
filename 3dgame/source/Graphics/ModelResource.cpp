#include <filesystem>
#include <fstream>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include "ModelResource.h"
#include "..//misc.h"
#include "GLTFImporter.h"
#include "GpuResourceUtils.h"
//#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "../../tinygltf-release/tiny_gltf.h"

const std::vector<D3D11_INPUT_ELEMENT_DESC> ModelResource::InputElementDescs =
{
    { "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

template<class Archive>
void ModelResource::Node::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(name),
        CEREAL_NVP(parentIndex),
        CEREAL_NVP(position),
        CEREAL_NVP(rotation),
        CEREAL_NVP(scale)
    );
}

template<class Archive>
void ModelResource::TextureInfo::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(index),
        CEREAL_NVP(texcoord)
    );
}

template<class Archive>
void ModelResource::NormalTextureInfo::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(index),
        CEREAL_NVP(texcoord),
        CEREAL_NVP(scale)
    );
}

template<class Archive>
void ModelResource::OcclusionTextureInfo::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(index),
        CEREAL_NVP(texcoord),
        CEREAL_NVP(strength)
    );
}

template<class Archive>
void ModelResource::PbrMetallicRoughness::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(baseColor),
        CEREAL_NVP(baseColorTexture),
        CEREAL_NVP(metallicFactor),
        CEREAL_NVP(roughnessFactor),
        CEREAL_NVP(metallicRoughnessTexture)
    );
}

template<class Archive>
void ModelResource::Material::CBuffer::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(emissiveColor),
        CEREAL_NVP(alphaMode),
        CEREAL_NVP(alphaCutOff),
        CEREAL_NVP(doubleSided),
        CEREAL_NVP(pbrMetallicRoughness),
        CEREAL_NVP(normalTexture),
        CEREAL_NVP(occlusionTexture),
        CEREAL_NVP(emissiveTexture)
    );
}

template<class Archive>
void ModelResource::Material::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(name),
        CEREAL_NVP(baseTextureFileName),
        CEREAL_NVP(normalTextureFileName),
        CEREAL_NVP(emissiveTextureFileName),
        CEREAL_NVP(occlusionTextureFileName),
        CEREAL_NVP(metalnessRoughnessTextureFileName),
        CEREAL_NVP(data)
    );
}

template<class Archive>
void ModelResource::Vertex::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(position),
        CEREAL_NVP(boneWeight),
        CEREAL_NVP(boneIndex),
        CEREAL_NVP(texcoord),
        CEREAL_NVP(normal),
        CEREAL_NVP(tangent)
    );
}

template<class Archive>
void ModelResource::Bone::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(nodeIndex),
        CEREAL_NVP(offsetTransform)
    );
}

template<class Archive>
void ModelResource::Mesh::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(vertices),
        CEREAL_NVP(indices),
        CEREAL_NVP(bones),
        CEREAL_NVP(nodeIndex),
        CEREAL_NVP(materialIndex)
    );
}

template<class Archive>
void ModelResource::VectorKeyframe::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(seconds),
        CEREAL_NVP(value)
    );
}

template<class Archive>
void ModelResource::QuaternionKeyframe::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(seconds),
        CEREAL_NVP(value)
    );
}

template<class Archive>
void ModelResource::NodeAnim::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(positionKeyframes),
        CEREAL_NVP(rotationKeyframes),
        CEREAL_NVP(scaleKeyframes)
    );
}

template<class Archive>
void ModelResource::Animation::serialize(Archive& archive)
{
    archive(
        CEREAL_NVP(name),
        CEREAL_NVP(secondsLength),
        CEREAL_NVP(nodeAnims)
    );
}

/// コンストラクタ
ModelResource::ModelResource()
{
}

// アニメーション追加読み込み
void ModelResource::AppendAnimations(const char* filename)
{
    std::filesystem::path filepath(filename);
    std::filesystem::path dirpath(filepath.parent_path());

    if (filepath.extension() == ".gltf" ||
        filepath.extension() == ".glb")
    {
        // 汎用モデルファイルの読み込み
        GLTFImporter importer(filename);

        // アニメーションデータ読み取り
        importer.LoadAnimations(_animations, _nodes);
    }
    else
    {
        _ASSERT_EXPR_A(false, "found not model file");
    }
}

// シリアライズ
void ModelResource::Serialize(const char* filename)
{
    /*if (std::ofstream ofs(filename, std::ios::binary); ofs.is_open())
    {
        cereal::BinaryOutputArchive archive(ofs);

        try
        {
            archive(
                CEREAL_NVP(nodes),
                CEREAL_NVP(materials),
                CEREAL_NVP(meshes),
                CEREAL_NVP(animations)
            );
        }
        catch (...)
        {
            _ASSERT_EXPR_A(false, "Model serialize failed.");
        }
    }*/
}

// デシリアライズ
void ModelResource::Deserialize(const char* filename)
{
    /*if (std::ifstream ifs(filename, std::ios::binary); ifs.is_open())
    {
        cereal::BinaryInputArchive archive(ifs);

        try
        {
            archive(
                CEREAL_NVP(nodes),
                CEREAL_NVP(materials),
                CEREAL_NVP(meshes),
                CEREAL_NVP(animations)
            );
        }
        catch (...)
        {
            _ASSERT_EXPR_A(false, "Model deserialize failed.");
        }
    }
    else
    {
        _ASSERT_EXPR_A(false, "Model File not found.");
    }*/
}

void ModelResource::Load(ID3D11Device* device, const char* filename, float sampleRate)
{
    std::filesystem::path filepath(filename);
    std::filesystem::path dirpath(filepath.parent_path());

    std::filesystem::path extension = filepath.extension();

    // 独自形式のモデルファイルの存在確認
    filepath.replace_extension(".cereal");
    if (std::filesystem::exists(filepath))
    {
        // 独自形式のモデルファイルの読み込み
        Deserialize(filepath.string().c_str());
    }
    else if (extension == ".gltf" || extension == ".glb")
    {
        // 汎用モデルファイルの読み込み
        GLTFImporter importer(filename);

        // マテリアルデータ読み取り
        importer.LoadMaterials(_materials, device);

        // ノードデータ読み取り
        importer.LoadNodes(_nodes);

        // メッシュデータ読み取り
        importer.LoadMeshes(_meshes, _nodes);

        // アニメーションデータ読み取り
        importer.LoadAnimations(_animations, _nodes, sampleRate);

        // 独自形式のモデルファイルを保存
        Serialize(filepath.string().c_str());
    }
    else
    {
        _ASSERT_EXPR_A(false, "found not model file");
    }

    // マテリアル構築
    for (Material& material : _materials)
    {
        HRESULT hr;
        if (material.baseMap == nullptr)
        {
            if (material.baseTextureFileName.empty())
            {
                // ダミーテクスチャ作成
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.baseMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // ベーステクスチャ読み込み
                std::filesystem::path diffuseTexturePath(dirpath / material.baseTextureFileName);
                hr = GpuResourceUtils::LoadTexture(device, diffuseTexturePath.string().c_str(),
                    material.baseMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }

        if (material.normalMap == nullptr)
        {
            if (material.normalTextureFileName.empty())
            {
                // 法線ダミーテクスチャ作成
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.normalMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // 法線テクスチャ読み込み
                std::filesystem::path texturePath(dirpath / material.normalTextureFileName);
                hr = GpuResourceUtils::LoadTexture(device, texturePath.string().c_str(),
                    material.normalMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }

        if (material.emissiveMap == nullptr)
        {
            if (material.emissiveTextureFileName.empty())
            {
                // エミッシブダミーテクスチャ生成
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.emissiveMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // エミッシブテクスチャ読み込み
                std::filesystem::path texturePath(dirpath / material.emissiveTextureFileName);
                hr = GpuResourceUtils::LoadTexture(device, texturePath.string().c_str(),
                    material.emissiveMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }

        if (material.occlusionMap == nullptr)
        {
            if (material.occlusionTextureFileName.empty())
            {
                // オクルージョンダミーテクスチャ生成
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.occlusionMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // オクルージョンテクスチャ読み込み
                std::filesystem::path texturePath(dirpath / material.occlusionTextureFileName);
                hr = GpuResourceUtils::LoadTexture(device, texturePath.string().c_str(),
                    material.occlusionMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }

        if (material.metalnessRoughnessMap == nullptr)
        {
            if (material.metalnessRoughnessTextureFileName.empty())
            {
                // PBRダミーテクスチャ生成
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.metalnessRoughnessMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // PBRテクスチャ読み込み
                std::filesystem::path texturePath(dirpath / material.metalnessRoughnessTextureFileName);
                hr = GpuResourceUtils::LoadTexture(device, texturePath.string().c_str(),
                    material.metalnessRoughnessMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }
    }

    // ノード構築
    for (size_t nodeIndex = 0; nodeIndex < _nodes.size(); ++nodeIndex)
    {
        Node& node = _nodes[nodeIndex];

        // 親子関係を構築
        node.parent = node.parentIndex >= 0 ? &_nodes[node.parentIndex] : nullptr;
        if (node.parent != nullptr)
        {
            node.parent->children.emplace_back(&node);
        }
    }

    // メッシュ構築
    for (Mesh& mesh : _meshes)
    {
        // 参照マテリアル設定
        mesh.material = &_materials[mesh.materialIndex];

        // 参照ノード設定
        mesh.node = &_nodes[mesh.nodeIndex];

        // 頂点バッファ
        {
            D3D11_BUFFER_DESC bufferDesc = {};
            D3D11_SUBRESOURCE_DATA subresourceData = {};

            bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;   // GPUのみ読み取り可
            bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bufferDesc.CPUAccessFlags = 0;
            bufferDesc.MiscFlags = 0;
            bufferDesc.StructureByteStride = 0;
            subresourceData.pSysMem = mesh.vertices.data();
            subresourceData.SysMemPitch = 0;
            subresourceData.SysMemSlicePitch = 0;

            HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }

        // インデックスバッファ
        {
            D3D11_BUFFER_DESC bufferDesc = {};
            D3D11_SUBRESOURCE_DATA subresourceData = {};

            bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bufferDesc.CPUAccessFlags = 0;
            bufferDesc.MiscFlags = 0;
            bufferDesc.StructureByteStride = 0;
            subresourceData.pSysMem = mesh.indices.data();
            subresourceData.SysMemPitch = 0;
            subresourceData.SysMemSlicePitch = 0;
            HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        }

        // ボーン構築
        for (Bone& bone : mesh.bones)
        {
            // 参照ノード設定
            bone.node = &_nodes.at(bone.nodeIndex);
        }
    }

}

// アニメーションインデックス取得
int ModelResource::GetAnimationIndex(const char* name) const
{
    for (size_t animationIndex = 0; animationIndex < _animations.size(); ++animationIndex)
    {
        if (_animations[animationIndex].name == name)
        {
            return static_cast<int>(animationIndex);
        }
    }
    return -1;
}

