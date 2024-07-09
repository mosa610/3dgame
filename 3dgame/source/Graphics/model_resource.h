//#pragma once
//
//#include <string>
//#include <vector>
//#include <wrl.h>
//#include <d3d11.h>
//#include <fbxsdk.h>
//#include <DirectXMath.h>
//
//class ModelResource
//{
//public:
//	ModelResource() {}
//	virtual ~ModelResource() {}
//
//	using NodeId = UINT64;
//
//	struct Node
//	{
//		NodeId				id;
//		std::string			name;
//		std::string			path;
//		int					parent_index;
//		DirectX::XMFLOAT3	scale;
//		DirectX::XMFLOAT4	rotate;
//		DirectX::XMFLOAT3	translate;
//		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Material
//	{
//		std::string			name;
//		std::string			textureFilename;
//		DirectX::XMFLOAT4	color = { 0.8f, 0.8f, 0.8f, 1.0f };
//
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Subset
//	{
//		UINT		startIndex = 0;
//		UINT		indexCount = 0;
//		int			materialIndex = 0;
//
//		Material* material = nullptr;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Vertex
//	{
//		DirectX::XMFLOAT3	position = { 0, 0, 0 };
//		DirectX::XMFLOAT3	normal = { 0, 0, 0 };
//		DirectX::XMFLOAT3	tangent = { 0, 0, 0 };
//		DirectX::XMFLOAT2	texcoord = { 0, 0 };
//		DirectX::XMFLOAT4	color = { 1, 1, 1, 1 };
//		DirectX::XMFLOAT4	boneWeight = { 1, 0, 0, 0 };
//		DirectX::XMUINT4	boneIndex = { 0, 0, 0, 0 };
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Mesh
//	{
//		uint64_t unique_id{ 0 };
//		std::string name;
//		std::vector<Vertex>						vertices;
//		std::vector<UINT>						indices;
//		std::vector<Subset>						subsets;
//
//		int										node_index;
//		std::vector<int>						nodeIndices;
//		std::vector<DirectX::XMFLOAT4X4>		offsetTransforms;
//
//		DirectX::XMFLOAT3						boundsMin;
//		DirectX::XMFLOAT3						boundsMax;
//
//		Microsoft::WRL::ComPtr<ID3D11Buffer>	vertexBuffer;
//		Microsoft::WRL::ComPtr<ID3D11Buffer>	indexBuffer;
//
//		DirectX::XMFLOAT4X4 default_global_transform
//		{
//			1,0,0,0,
//			0,1,0,0,
//			0,0,1,0,
//			0,0,0,1
//		};
//		DirectX::XMFLOAT3 bounding_box[2]
//		{
//			{+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX},
//			{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
//		};
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct NodeKeyData
//	{
//		DirectX::XMFLOAT3	scale;
//		DirectX::XMFLOAT4	rotate;
//		DirectX::XMFLOAT3	translate;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	struct Keyframe
//	{
//		float						seconds;
//		std::vector<NodeKeyData>	nodeKeys;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//	struct Animation
//	{
//		std::string					name;
//		float						secondsLength;
//		std::vector<Keyframe>		keyframes;
//
//		template<class Archive>
//		void serialize(Archive& archive, int version);
//	};
//
//	// 各種データ取得
//	const std::vector<Mesh>& GetMeshes() const { return meshes; }
//	const std::vector<Node>& GetNodes() const { return nodes; }
//	const std::vector<Animation>& GetAnimations() const { return animations; }
//	const std::vector<Material>& GetMaterials() const { return materials; }
//
//	// 読み込み
//	void Load(ID3D11Device* device, const char* filename);
//
//protected:
//	// モデルセットアップ
//	void BuildModel(ID3D11Device* device, const char* dirname);
//
//	// シリアライズ
//	void Serialize(const char* filename);
//
//	// デシリアライズ
//	void Deserialize(const char* filename, bool triangulate);
//
//	// ノードインデックスを取得する
//	int FindNodeIndex(NodeId nodeId) const;
//
//
//	void fetch_meshes(FbxScene* fbx_scene, std::vector<Mesh>& meshes);
//	void fetch_materials(FbxScene* fbx_scene,
//		std::unordered_map<uint64_t, Material>& materials);
//	//void fetch_skeleton(FbxMesh* fbx_mesh, Bone& bind_pose);
//	void fetch_animations(FbxScene* fbx_scene, std::vector<Animation>& animation_clips,
//		float sampling_rate /*If this value is 0, the animation data will be sampled at the default frame rate.*/);
//	void update_animation(Keyframe& keyframe);
//	bool append_animations(const char* animation_filename, float sampling_rate);
//	void blend_animations(const Keyframe* keyframes[2], float factor,
//		Keyframe& keyframe);
//	void create_com_objects(ID3D11Device* device, const char* fbx_filename);
//
//
//protected:
//	std::vector<Node>		nodes;
//	std::vector<Material>	materials;
//	std::vector<Mesh>		meshes;
//	std::vector<Animation>	animations;
//};
