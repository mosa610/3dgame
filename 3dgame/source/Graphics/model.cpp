//#include "Graphics.h"
//#include "model.h"
//
//Model::Model(const char* filename)
//{
//	// リソース読み込み
//	resource = std::make_shared<ModelResource>(Graphics::Instance().Get_device(),filename,0,true);
//	//resource->Load(Graphics::Instance().Get_device(), filename);
//
//	// ノード
//	const std::vector<ModelResource::scene::node>& resNodes = resource->GetNodes();
//
//	nodes.resize(resNodes.size());
//	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
//	{
//		auto&& src = resNodes.at(nodeIndex);
//		auto&& dst = nodes.at(nodeIndex);
//
//		dst.name = src.name.c_str();
//		dst.parent = src.parent_index >= 0 ? &nodes.at(src.parent_index) : nullptr;
//		dst.scale = src.scale;
//		dst.rotate = src.rotate;
//		dst.translate = src.translate;
//
//		if (dst.parent != nullptr)
//		{
//			dst.parent->children.emplace_back(&dst);
//		}
//	}
//
//	// 行列計算
//	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
//	UpdateTransform(transform);
//}
//
//void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
//{
//	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
//
//	for (Node& node : nodes)
//	{
//		// ローカル行列算出
//		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
//		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
//		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
//		DirectX::XMMATRIX LocalTransform = S * R * T;
//
//		// ワールド行列算出
//		DirectX::XMMATRIX ParentTransform;
//		if (node.parent != nullptr)
//		{
//			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
//		}
//		else
//		{
//			ParentTransform = Transform;
//		}
//		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;
//
//		// 計算結果を格納
//		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
//		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
//
//		resource->skin->render(Graphics::Instance().Get_device_context(),
//			node.worldTransform, { 1,1,1,1 }, 0);
//	}
//}
//
//void Model::render() const
//{
//	for (auto& node : nodes)
//	{
//		resource->skin->render(Graphics::Instance().Get_device_context(),
//			node.worldTransform, { 1,1,1,1 }, 0);
//	}
//}
