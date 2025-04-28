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

/// �R���X�g���N�^
ModelResource::ModelResource()
{
}

// �A�j���[�V�����ǉ��ǂݍ���
void ModelResource::AppendAnimations(const char* filename)
{
    std::filesystem::path filepath(filename);
    std::filesystem::path dirpath(filepath.parent_path());

    if (filepath.extension() == ".gltf" ||
        filepath.extension() == ".glb")
    {
        // �ėp���f���t�@�C���̓ǂݍ���
        GLTFImporter importer(filename);

        // �A�j���[�V�����f�[�^�ǂݎ��
        importer.LoadAnimations(_animations, _nodes);
    }
    else
    {
        _ASSERT_EXPR_A(false, "found not model file");
    }
}

// �V���A���C�Y
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

// �f�V���A���C�Y
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

    // �Ǝ��`���̃��f���t�@�C���̑��݊m�F
    filepath.replace_extension(".cereal");
    if (std::filesystem::exists(filepath))
    {
        // �Ǝ��`���̃��f���t�@�C���̓ǂݍ���
        Deserialize(filepath.string().c_str());
    }
    else if (extension == ".gltf" || extension == ".glb")
    {
        // �ėp���f���t�@�C���̓ǂݍ���
        GLTFImporter importer(filename);

        // �}�e���A���f�[�^�ǂݎ��
        importer.LoadMaterials(_materials, device);

        // �m�[�h�f�[�^�ǂݎ��
        importer.LoadNodes(_nodes);

        // ���b�V���f�[�^�ǂݎ��
        importer.LoadMeshes(_meshes, _nodes);

        // �A�j���[�V�����f�[�^�ǂݎ��
        importer.LoadAnimations(_animations, _nodes, sampleRate);

        // �Ǝ��`���̃��f���t�@�C����ۑ�
        Serialize(filepath.string().c_str());
    }
    else
    {
        _ASSERT_EXPR_A(false, "found not model file");
    }

    // �}�e���A���\�z
    for (Material& material : _materials)
    {
        HRESULT hr;
        if (material.baseMap == nullptr)
        {
            if (material.baseTextureFileName.empty())
            {
                // �_�~�[�e�N�X�`���쐬
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.baseMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // �x�[�X�e�N�X�`���ǂݍ���
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
                // �@���_�~�[�e�N�X�`���쐬
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.normalMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // �@���e�N�X�`���ǂݍ���
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
                // �G�~�b�V�u�_�~�[�e�N�X�`������
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.emissiveMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // �G�~�b�V�u�e�N�X�`���ǂݍ���
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
                // �I�N���[�W�����_�~�[�e�N�X�`������
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.occlusionMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // �I�N���[�W�����e�N�X�`���ǂݍ���
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
                // PBR�_�~�[�e�N�X�`������
                hr = GpuResourceUtils::CreateDummyTexture(device, 0xFFFF7F7F,
                    material.metalnessRoughnessMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
            else
            {
                // PBR�e�N�X�`���ǂݍ���
                std::filesystem::path texturePath(dirpath / material.metalnessRoughnessTextureFileName);
                hr = GpuResourceUtils::LoadTexture(device, texturePath.string().c_str(),
                    material.metalnessRoughnessMap.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }
    }

    // �m�[�h�\�z
    for (size_t nodeIndex = 0; nodeIndex < _nodes.size(); ++nodeIndex)
    {
        Node& node = _nodes[nodeIndex];

        // �e�q�֌W���\�z
        node.parent = node.parentIndex >= 0 ? &_nodes[node.parentIndex] : nullptr;
        if (node.parent != nullptr)
        {
            node.parent->children.emplace_back(&node);
        }
    }

    // ���b�V���\�z
    for (Mesh& mesh : _meshes)
    {
        // �Q�ƃ}�e���A���ݒ�
        mesh.material = &_materials[mesh.materialIndex];

        // �Q�ƃm�[�h�ݒ�
        mesh.node = &_nodes[mesh.nodeIndex];

        // ���_�o�b�t�@
        {
            D3D11_BUFFER_DESC bufferDesc = {};
            D3D11_SUBRESOURCE_DATA subresourceData = {};

            bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;   // GPU�̂ݓǂݎ���
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

        // �C���f�b�N�X�o�b�t�@
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

        // �{�[���\�z
        for (Bone& bone : mesh.bones)
        {
            // �Q�ƃm�[�h�ݒ�
            bone.node = &_nodes.at(bone.nodeIndex);
        }
    }

}

// �A�j���[�V�����C���f�b�N�X�擾
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

