#pragma once

#include <map>
#include <filesystem>
#define TINYGLTF_NO_EXTERNAL_IMAGE 
#define TINYGLTF_NO_STB_IMAGE 
#define TINYGLTF_NO_STB_IMAGE_WRITE 
//#include "..//..//tinygltf-release/tiny_gltf.h"
#include "ModelResource.h"

class GLTFImporter
{
private:
	using MeshList = std::vector<ModelResource::Mesh>;
	using MaterialList = std::vector<ModelResource::Material>;
	using NodeList = std::vector<ModelResource::Node>;
	using AnimationList = std::vector<ModelResource::Animation>;

public:
	GLTFImporter(const char* filename);

	// �m�[�h�f�[�^��ǂݍ���
	void LoadNodes(NodeList& nodes);

	// ���b�V���f�[�^��ǂݍ���
	void LoadMeshes(MeshList& meshes, const NodeList& nodes);

	// �}�e���A���f�[�^��ǂݍ���
	void LoadMaterials(MaterialList& materials, ID3D11Device* device = nullptr);

	// �A�j���[�V�����f�[�^��ǂݍ���
	void LoadAnimations(AnimationList& animations, const NodeList& nodes, float sampleRate = 60);

private:
	// gltfVector3 �� XMFLOAT3
	static DirectX::XMFLOAT3 gltfVector3ToXMFLOAT3(const std::vector<double>& gltfValue);

	// gltfQuaternion �� XMFLOAT4
	static DirectX::XMFLOAT4 gltfQuaternionToXMFLOAT4(const std::vector<double>& gltfValue);

	// gltfMatrix �� XMFLOAT4X4
	static DirectX::XMFLOAT4X4 gltfMatrixToXMFLOAT4X4(const std::vector<double>& gltfValue);

	// ���W�n�ϊ�
	static void ConvertPositionAxisSystem(DirectX::XMFLOAT3& v);
	static void ConvertPositionAxisSystem(DirectX::XMFLOAT4& v);
	static void ConvertRotationAxisSystem(DirectX::XMFLOAT4& q);
	static void ConvertMatrixAxisSystem(DirectX::XMFLOAT4X4& m);
	static void ConvertNodeAxisSystem(ModelResource::Node& node);
	static void ConvertMeshAxisSystem(ModelResource::Mesh& mesh);
	static void ConvertAnimationAxisSystem(ModelResource::Animation& animation);

	// �^���W�F���g�v�Z
	static void ComputeTangents(std::vector<ModelResource::Vertex>& vertices, const std::vector<uint32_t>& indices);

private:
	std::filesystem::path			filepath;
	tinygltf::Model					gltfModel;
};
