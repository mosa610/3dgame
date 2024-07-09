//#pragma once
//
//
//#include <memory>
//#include <vector>
//#include <DirectXMath.h>
//#include "model_resource.h"
//
//// ���f��
//class Model
//{
//public:
//	Model(const char* filename);
//	~Model() {}
//
//	struct Node
//	{
//		const char*			name;
//		Node*				parent;
//		DirectX::XMFLOAT3	scale = { 1,1,1 };
//		DirectX::XMFLOAT4	rotate = { 0,0,0,0 };
//		DirectX::XMFLOAT3	translate = { 0,0,0 };
//		DirectX::XMFLOAT4X4	localTransform;
//		DirectX::XMFLOAT4X4	worldTransform;
//
//		std::vector<Node*>	children;
//	};
//
//	// �s��v�Z
//	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);
//
//	// �m�[�h���X�g�擾
//	const std::vector<Node>& GetNodes() const { return nodes; }
//	std::vector<Node>& GetNodes() { return nodes; }
//
//	// ���\�[�X�擾
//	const ModelResource* GetResource() const { return resource.get(); }
//
//	void render() const;
//
//private:
//	std::shared_ptr<ModelResource>	resource;
//	std::vector<Node>				nodes;
//};