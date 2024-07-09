//#include <stdlib.h>
//#include <fstream>
//#include <functional>
//#include <cereal/cereal.hpp>
//#include <cereal/archives/binary.hpp>
//#include <cereal/types/string.hpp>
//#include <cereal/types/vector.hpp>
//#include <WICTextureLoader.h>
//#include <filesystem>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//
//#include "../misc.h"
//#include "../Logger.h"
//#include "model_resource.h"
//#include "texture.h"
//#include "shader.h"
//
//// CEREALバージョン定義
//CEREAL_CLASS_VERSION(ModelResource::Node, 1)
//CEREAL_CLASS_VERSION(ModelResource::Material, 1)
//CEREAL_CLASS_VERSION(ModelResource::Subset, 1)
//CEREAL_CLASS_VERSION(ModelResource::Vertex, 1)
//CEREAL_CLASS_VERSION(ModelResource::Mesh, 1)
//CEREAL_CLASS_VERSION(ModelResource::NodeKeyData, 1)
//CEREAL_CLASS_VERSION(ModelResource::Keyframe, 1)
//CEREAL_CLASS_VERSION(ModelResource::Animation, 1)
//CEREAL_CLASS_VERSION(ModelResource, 1)
//
//
//inline DirectX::XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix);
//inline DirectX::XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3);
//inline DirectX::XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4);
//
//// シリアライズ
//namespace DirectX
//{
//	template<class Archive>
//	void serialize(Archive& archive, XMUINT4& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z),
//			cereal::make_nvp("w", v.w)
//		);
//	}
//
//	template<class Archive>
//	void serialize(Archive& archive, XMFLOAT2& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y)
//		);
//	}
//
//	template<class Archive>
//	void serialize(Archive& archive, XMFLOAT3& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z)
//		);
//	}
//
//	template<class Archive>
//	void serialize(Archive& archive, XMFLOAT4& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z),
//			cereal::make_nvp("w", v.w)
//		);
//	}
//
//	template<class Archive>
//	void serialize(Archive& archive, XMFLOAT4X4& m)
//	{
//		archive(
//			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
//			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
//			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
//			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
//		);
//	}
//}
//
//template<class Archive>
//void ModelResource::Node::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(id),
//		CEREAL_NVP(name),
//		CEREAL_NVP(path),
//		CEREAL_NVP(parent_index),
//		CEREAL_NVP(scale),
//		CEREAL_NVP(rotate),
//		CEREAL_NVP(translate),
//		CEREAL_NVP(attribute)
//	);
//}
//
//template<class Archive>
//void ModelResource::Material::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(name),
//		CEREAL_NVP(textureFilename),
//		CEREAL_NVP(color)
//	);
//}
//
//template<class Archive>
//void ModelResource::Subset::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(startIndex),
//		CEREAL_NVP(indexCount),
//		CEREAL_NVP(materialIndex)
//	);
//}
//
//template<class Archive>
//void ModelResource::Vertex::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(position),
//		CEREAL_NVP(normal),
//		CEREAL_NVP(tangent),
//		CEREAL_NVP(texcoord),
//		CEREAL_NVP(color),
//		CEREAL_NVP(boneWeight),
//		CEREAL_NVP(boneIndex)
//	);
//}
//
//template<class Archive>
//void ModelResource::Mesh::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(unique_id),
//		CEREAL_NVP(name),
//		CEREAL_NVP(vertices),
//		CEREAL_NVP(indices),
//		CEREAL_NVP(subsets),
//		CEREAL_NVP(nodeIndex),
//		CEREAL_NVP(nodeIndices),
//		CEREAL_NVP(offsetTransforms),
//		CEREAL_NVP(boundsMin),
//		CEREAL_NVP(boundsMax),
//		CEREAL_NVP(bounding_box),
//		CEREAL_NVP(default_global_transform)
//
//	);
//}
//
//template<class Archive>
//void ModelResource::NodeKeyData::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(scale),
//		CEREAL_NVP(rotate),
//		CEREAL_NVP(translate)
//	);
//}
//
//template<class Archive>
//void ModelResource::Keyframe::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(seconds),
//		CEREAL_NVP(nodeKeys)
//	);
//}
//
//template<class Archive>
//void ModelResource::Animation::serialize(Archive& archive, int version)
//{
//	archive(
//		CEREAL_NVP(name),
//		CEREAL_NVP(secondsLength),
//		CEREAL_NVP(keyframes)
//	);
//}
//
//// 読み込み
//void ModelResource::Load(ID3D11Device* device, const char* filename)
//{
//	// ディレクトリパス取得
//	char drive[32], dir[256], dirname[256];
//	::_splitpath_s(filename, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
//	::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);
//
//	// デシリアライズ
//	Deserialize(filename,true);
//
//	// モデル構築
//	BuildModel(device, dirname);
//}
//
//// モデル構築
//void ModelResource::BuildModel(ID3D11Device* device, const char* dirname)
//{
//	for (Material& material : materials)
//	{
//		// 相対パスの解決
//		char filename[256];
//		::_makepath_s(filename, 256, nullptr, dirname, material.textureFilename.c_str(), nullptr);
//
//		// マルチバイト文字からワイド文字へ変換
//		wchar_t wfilename[256];
//		::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, 256);
//
//		// テクスチャ読み込み
//		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
//		HRESULT hr = DirectX::CreateWICTextureFromFile(device, wfilename, resource.GetAddressOf(), material.shaderResourceView.GetAddressOf());
//		if (FAILED(hr))
//		{
//			// WICでサポートされていないフォーマットの場合（TGAなど）は
//			// STBで画像読み込みをしてテクスチャを生成する
//			int width, height, bpp;
//			unsigned char* pixels = stbi_load(filename, &width, &height, &bpp, STBI_rgb_alpha);
//			if (pixels != nullptr)
//			{
//				D3D11_TEXTURE2D_DESC desc = { 0 };
//				desc.Width = width;
//				desc.Height = height;
//				desc.MipLevels = 1;
//				desc.ArraySize = 1;
//				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//				desc.SampleDesc.Count = 1;
//				desc.SampleDesc.Quality = 0;
//				desc.Usage = D3D11_USAGE_DEFAULT;
//				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//				desc.CPUAccessFlags = 0;
//				desc.MiscFlags = 0;
//				D3D11_SUBRESOURCE_DATA data;
//				::memset(&data, 0, sizeof(data));
//				data.pSysMem = pixels;
//				data.SysMemPitch = width * 4;
//
//				Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
//				HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//				hr = device->CreateShaderResourceView(texture.Get(), nullptr, material.shaderResourceView.GetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//				// 後始末
//				stbi_image_free(pixels);
//			}
//			else
//			{
//				// 読み込み失敗したらダミーテクスチャを作る
//				LOG("load failed : %s\n", filename);
//
//				const int width = 8;
//				const int height = 8;
//				UINT pixels[width * height];
//				::memset(pixels, 0xFF, sizeof(pixels));
//
//				D3D11_TEXTURE2D_DESC desc = { 0 };
//				desc.Width = width;
//				desc.Height = height;
//				desc.MipLevels = 1;
//				desc.ArraySize = 1;
//				desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//				desc.SampleDesc.Count = 1;
//				desc.SampleDesc.Quality = 0;
//				desc.Usage = D3D11_USAGE_DEFAULT;
//				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//				desc.CPUAccessFlags = 0;
//				desc.MiscFlags = 0;
//				D3D11_SUBRESOURCE_DATA data;
//				::memset(&data, 0, sizeof(data));
//				data.pSysMem = pixels;
//				data.SysMemPitch = width;
//
//				Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
//				HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//				hr = device->CreateShaderResourceView(texture.Get(), nullptr, material.shaderResourceView.GetAddressOf());
//				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//			}
//		}
//	}
//
//	for (Mesh& mesh : meshes)
//	{
//		// サブセット
//		for (Subset& subset : mesh.subsets)
//		{
//			subset.material = &materials.at(subset.materialIndex);
//		}
//
//		// 頂点バッファ
//		{
//			D3D11_BUFFER_DESC bufferDesc = {};
//			D3D11_SUBRESOURCE_DATA subresourceData = {};
//
//			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
//			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
//			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//			bufferDesc.CPUAccessFlags = 0;
//			bufferDesc.MiscFlags = 0;
//			bufferDesc.StructureByteStride = 0;
//			subresourceData.pSysMem = mesh.vertices.data();
//			subresourceData.SysMemPitch = 0;
//			subresourceData.SysMemSlicePitch = 0;
//
//			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.GetAddressOf());
//			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//		}
//
//		// インデックスバッファ
//		{
//			D3D11_BUFFER_DESC bufferDesc = {};
//			D3D11_SUBRESOURCE_DATA subresourceData = {};
//
//			bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * mesh.indices.size());
//			//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
//			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//			bufferDesc.CPUAccessFlags = 0;
//			bufferDesc.MiscFlags = 0;
//			bufferDesc.StructureByteStride = 0;
//			subresourceData.pSysMem = mesh.indices.data();
//			subresourceData.SysMemPitch = 0; //Not use for index buffers.
//			subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
//			HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.GetAddressOf());
//			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//		}
//	}
//}
//
//// シリアライズ
//void ModelResource::Serialize(const char* filename)
//{
//	std::filesystem::path cereal_filename(filename);
//	std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
//	cereal::BinaryOutputArchive serialization(ofs);
//	serialization(nodes, meshes, materials, animations);
//
//
//	std::ofstream ostream(filename, std::ios::binary);
//	if (ostream.is_open())
//	{
//		cereal::BinaryOutputArchive archive(ostream);
//
//		try
//		{
//			archive(
//				CEREAL_NVP(nodes),
//				CEREAL_NVP(materials),
//				CEREAL_NVP(meshes),
//				CEREAL_NVP(animations)
//			);
//		}
//		catch (...)
//		{
//			LOG("model deserialize failed.\n%s\n", filename);
//			return;
//		}
//	}
//}
//
//// デシリアライズ
//void ModelResource::Deserialize(const char* filename, bool triangulate)
//{
//	std::filesystem::path cereal_filename(filename);
//	cereal_filename.replace_extension("cereal");
//	if (std::filesystem::exists(cereal_filename.c_str()))
//	{
//		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
//		cereal::BinaryInputArchive deserialization(ifs);
//		deserialization(nodes, meshes, materials, animations);
//	}
//	else
//	{
//		FbxManager* fbx_manager{ FbxManager::Create() };
//		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager,"") };
//
//		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager,"") };
//		bool import_status{ false };
//		import_status = fbx_importer->Initialize(filename);
//		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
//
//		import_status = fbx_importer->Import(fbx_scene);
//		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
//
//		FbxGeometryConverter fbx_converter(fbx_manager);
//		if (triangulate)
//		{
//			fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
//			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
//		}
//
//		std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
//			Node& node{nodes.emplace_back()};
//			node.attribute = fbx_node->GetNodeAttribute() ?
//				fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
//			node.name = fbx_node->GetName();
//			node.id = fbx_node->GetUniqueID();
//			node.parent_index = FindNodeIndex(fbx_node->GetParent() ?
//				fbx_node->GetParent()->GetUniqueID() : 0);
//			for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
//			{
//				traverse(fbx_node->GetChild(child_index));
//			}
//		} };
//		traverse(fbx_scene->GetRootNode());
//
//
//		fbx_manager->Destroy();
//	}
//}
//
//// ノードインデックスを取得する
//	int ModelResource::FindNodeIndex(NodeId nodeId) const
//{
//	int nodeCount = static_cast<int>(nodes.size());
//	for (int i = 0; i < nodeCount; ++i)
//	{
//		if (nodes[i].id == nodeId)
//		{
//			return i;
//		}
//	}
//	return -1;
//}
//
//	void ModelResource::fetch_meshes(FbxScene* fbx_scene, std::vector<Mesh>& meshes)
//	{
//		for (const Node& node : nodes)
//		{
//			if (node.attribute != FbxNodeAttribute::EType::eMesh)
//			{
//				continue;
//			}
//
//			FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
//			FbxMesh* fbx_mesh{ fbx_node->GetMesh() };
//
//			Mesh& mesh{ meshes.emplace_back() };
//			mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
//			mesh.name = fbx_mesh->GetNode()->GetName();
//			mesh.node_index = FindNodeIndex(mesh.unique_id);
//
//			mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());
//
//			std::vector<bone_influences_per_control_point> bone_influences;
//			fetch_bone_influences(fbx_mesh, bone_influences);
//
//			fetch_skeleton(fbx_mesh, mesh.bind_pose);
//
//			std::vector<mesh::subset>& subsets{ mesh.subsets };
//			const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
//			subsets.resize(material_count > 0 ? material_count : 1);
//			for (int material_index = 0; material_index < material_count; ++material_index)
//			{
//				const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
//				subsets.at(material_index).material_name = fbx_material->GetName();
//				subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
//			}
//			if (material_count > 0)
//			{
//				const int polygon_count{ fbx_mesh->GetPolygonCount() };
//				for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
//				{
//					const int material_index
//					{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
//					subsets.at(material_index).index_count += 3;
//				}
//				uint32_t offset{ 0 };
//				for (mesh::subset& subset : subsets)
//				{
//					subset.start_index_location = offset;
//					offset += subset.index_count;
//					// This will be used as counter in the following procedures, reset to zero
//					subset.index_count = 0;
//				}
//			}
//
//			const int polygon_count{ fbx_mesh->GetPolygonCount() };
//			mesh.vertices.resize(polygon_count * 3LL);
//			mesh.indices.resize(polygon_count * 3LL);
//
//			FbxStringList uv_names;
//			fbx_mesh->GetUVSetNames(uv_names);
//			const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
//			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
//			{
//				const int material_index{ material_count > 0 ?
//					fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
//				mesh::subset& subset{ subsets.at(material_index) };
//				const uint32_t offset{ subset.start_index_location + subset.index_count };
//				for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
//				{
//					const int vertex_index{ polygon_index * 3 + position_in_polygon };
//
//					vertex vertex;
//					const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index,position_in_polygon) };
//					vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
//					vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
//					vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);
//
//					const bone_influences_per_control_point& influences_per_control_point
//					{ bone_influences.at(polygon_vertex) };
//					for (size_t influence_index = 0; influence_index < influences_per_control_point.size();
//						++influence_index)
//					{
//						if (influence_index < MAX_BONE_INFLUENCES)
//						{
//							vertex.bone_weights[influence_index] =
//								influences_per_control_point.at(influence_index).bone_weight;
//							vertex.bone_indices[influence_index] =
//								influences_per_control_point.at(influence_index).bone_index;
//						}
//					}
//
//
//					if (fbx_mesh->GetElementNormalCount() > 0)
//					{
//						FbxVector4 normal;
//						fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
//						vertex.normal.x = static_cast<float>(normal[0]);
//						vertex.normal.y = static_cast<float>(normal[1]);
//						vertex.normal.z = static_cast<float>(normal[2]);
//					}
//					if (fbx_mesh->GetElementUVCount() > 0)
//					{
//						FbxVector2 uv;
//						bool unmapped_uv;
//						fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon,
//							uv_names[0], uv, unmapped_uv);
//						vertex.texcoord.x = static_cast<float>(uv[0]);
//						vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
//					}
//					if (fbx_mesh->GenerateTangentsData(0, false))
//					{
//						const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
//						vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
//						vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
//						vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
//						vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
//					}
//
//					mesh.vertices.at(vertex_index) = std::move(vertex);
//					//mesh.indices.at(vertex_index) = vertex_index;
//					mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
//					subset.index_count++;
//				}
//				for (const vertex& v : mesh.vertices)
//				{
//					mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
//					mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
//					mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
//					mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
//					mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
//					mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
//				}
//			}
//		}
//	}
//
//	void ModelResource::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, Material>& materials)
//	{
//	}
//
//	void ModelResource::fetch_animations(FbxScene* fbx_scene, std::vector<Animation>& animation_clips, float sampling_rate)
//	{
//	}
//
//	void ModelResource::update_animation(Keyframe& keyframe)
//	{
//	}
//
//	bool ModelResource::append_animations(const char* animation_filename, float sampling_rate)
//	{
//		return false;
//	}
//
//	void ModelResource::blend_animations(const Keyframe* keyframes[2], float factor, Keyframe& keyframe)
//	{
//	}
//
//	void ModelResource::create_com_objects(ID3D11Device* device, const char* fbx_filename)
//	{
//	}
//
//
//	inline DirectX::XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
//	{
//		DirectX::XMFLOAT4X4 xmfloat4x4;
//		for (int row = 0; row < 4; ++row)
//		{
//			for (int column = 0; column < 4; ++column)
//			{
//				xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
//			}
//		}
//		return xmfloat4x4;
//	}
//
//	inline DirectX::XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
//	{
//		DirectX::XMFLOAT3 xmfloat3;
//		xmfloat3.x = static_cast<float>(fbxdouble3[0]);
//		xmfloat3.y = static_cast<float>(fbxdouble3[1]);
//		xmfloat3.z = static_cast<float>(fbxdouble3[2]);
//		return xmfloat3;
//	}
//
//	inline DirectX::XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
//	{
//		DirectX::XMFLOAT4 xmfloat4;
//		xmfloat4.x = static_cast<float>(fbxdouble4[0]);
//		xmfloat4.y = static_cast<float>(fbxdouble4[1]);
//		xmfloat4.z = static_cast<float>(fbxdouble4[2]);
//		xmfloat4.w = static_cast<float>(fbxdouble4[3]);
//		return xmfloat4;
//	}
