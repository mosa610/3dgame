#include "SystemTransform.h"
#include "ComponentNode.h"

void SystemTransform::update(Register& reg, float dt)
{
    for (Entity e : reg.view<ComponentTransform>()) {
        auto& t = reg.getComponent<ComponentTransform>(e);

        {
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(t.scale.x, t.scale.y, t.scale.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(t.rotation.x, t.rotation.y, t.rotation.z);
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(t.position.x, t.position.y, t.position.z);
            DirectX::XMMATRIX WorldTransform = S * R * T;

            DirectX::XMStoreFloat4x4(&t.world_transform, WorldTransform);
        }

        DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&t.world_transform);

        if (reg.hasComponent<ComponentNode>(e))
        {
            auto& c_node = reg.getComponent<ComponentNode>(e);
            for (auto& node : c_node.nodes)
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
    }
}
