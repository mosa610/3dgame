#include "SystemAnimation.h"
#include "ComponentModel.h"
#include "ComponentAnimation.h"
#include "ComponentNode.h"
#include "..//Graphics/ModelResource.h"

void SystemAnimation::update(Register& reg, float elapsed_time)
{
    for (Entity e : reg.view<ComponentModel, ComponentAnimation>())
    {
        auto& model = reg.getComponent<ComponentModel>(e);
        auto& c_animation = reg.getComponent<ComponentAnimation>(e);
        ComputeAnimation(e, reg, model, c_animation);

        // アニメーション更新
        const ModelResource::Animation& animation = model.resource->GetAnimations().at(c_animation.animation_index);
        c_animation.animation_time += elapsed_time;
        if (c_animation.animation_time > animation.secondsLength)
        {
            c_animation.animation_time -= animation.secondsLength;
        }
        if (reg.hasComponent<ComponentNode>(e))
        {
            auto& c_node = reg.getComponent<ComponentNode>(e);
            for (size_t nodeIndex = 0; nodeIndex < c_node.nodes.size(); ++nodeIndex)
            {
                const NodePose& pose = c_node.node_poses[nodeIndex];
                Node& node = c_node.nodes[nodeIndex];

                node.position = pose.position;
                node.rotation = pose.rotation;
                node.scale = pose.scale;
            }
        }
    }
}

void SystemAnimation::ComputeAnimation(Entity e, Register& reg, ComponentModel& model, ComponentAnimation& c_animation)
{
    float time = c_animation.animation_time;

    if (reg.hasComponent<ComponentNode>(e))
    {
        auto& c_node = reg.getComponent<ComponentNode>(e);
        if (c_node.node_poses.size() != c_node.nodes.size())
        {
            c_node.node_poses.resize(c_node.nodes.size());
        }
        for (size_t nodeIndex = 0; nodeIndex < c_node.node_poses.size(); ++nodeIndex)
        {

            const ModelResource::Animation& animation = model.resource->GetAnimations()[c_animation.animation_index];
            const ModelResource::NodeAnim& nodeAnim = animation.nodeAnims[nodeIndex];

            // 位置
            for (size_t index = 0; index < nodeAnim.positionKeyframes.size() - 1; ++index)
            {
                // 現在の時間がどのキーフレームの間にいるか判定する
                const ModelResource::VectorKeyframe& currentKeyframe = nodeAnim.positionKeyframes[index];
                const ModelResource::VectorKeyframe& nextKeyframe = nodeAnim.positionKeyframes[index + 1];
                if (time >= currentKeyframe.seconds && time <= nextKeyframe.seconds)
                {
                    // 再生時間とキーフレームの時間から補完率を算出する
                    float rate = (time - currentKeyframe.seconds) / (nextKeyframe.seconds - currentKeyframe.seconds);

                    // 前のキーフレームと次のキーフレームの姿勢を補完
                    DirectX::XMVECTOR CurrentPosition = DirectX::XMLoadFloat3(&currentKeyframe.value);
                    DirectX::XMVECTOR NextPosition = DirectX::XMLoadFloat3(&nextKeyframe.value);
                    DirectX::XMVECTOR Position = DirectX::XMVectorLerp(CurrentPosition, NextPosition, rate);
                    // 計算結果をノードに格納
                    DirectX::XMStoreFloat3(&c_node.node_poses[nodeIndex].position, Position);
                }
            }
            // 回転
            for (size_t index = 0; index < nodeAnim.rotationKeyframes.size() - 1; ++index)
            {
                // 現在の時間がどのキーフレームの間にいるか判定する
                const ModelResource::QuaternionKeyframe& currentKeyframe = nodeAnim.rotationKeyframes[index];
                const ModelResource::QuaternionKeyframe& nextKeyframe = nodeAnim.rotationKeyframes[index + 1];
                if (time >= currentKeyframe.seconds && time <= nextKeyframe.seconds)
                {
                    // 再生時間とキーフレームの時間から補完率を算出する
                    float rate = (time - currentKeyframe.seconds) / (nextKeyframe.seconds - currentKeyframe.seconds);

                    // 前のキーフレームと次のキーフレームの姿勢を補完
                    DirectX::XMVECTOR CurrentQuaternion = DirectX::XMLoadFloat4(&currentKeyframe.value);
                    DirectX::XMVECTOR NextQuaternion = DirectX::XMLoadFloat4(&nextKeyframe.value);
                    DirectX::XMVECTOR Quaternion = DirectX::XMQuaternionSlerp(CurrentQuaternion, NextQuaternion, rate);
                    // 計算結果をノードに格納
                    DirectX::XMStoreFloat4(&c_node.node_poses[nodeIndex].rotation, Quaternion);
                }
            }
            // スケール
            for (size_t index = 0; index < nodeAnim.scaleKeyframes.size() - 1; ++index)
            {
                // 現在の時間がどのキーフレームの間にいるか判定する
                const ModelResource::VectorKeyframe& currentKeyframe = nodeAnim.scaleKeyframes[index];
                const ModelResource::VectorKeyframe& nextKeyframe = nodeAnim.scaleKeyframes[index + 1];
                if (time >= currentKeyframe.seconds && time <= nextKeyframe.seconds)
                {
                    // 再生時間とキーフレームの時間から補完率を算出する
                    float rate = (time - currentKeyframe.seconds) / (nextKeyframe.seconds - currentKeyframe.seconds);

                    // 前のキーフレームと次のキーフレームの姿勢を補完
                    DirectX::XMVECTOR CurrentScale = DirectX::XMLoadFloat3(&currentKeyframe.value);
                    DirectX::XMVECTOR NextScale = DirectX::XMLoadFloat3(&nextKeyframe.value);
                    DirectX::XMVECTOR Scale = DirectX::XMVectorLerp(CurrentScale, NextScale, rate);
                    // 計算結果をノードに格納
                    DirectX::XMStoreFloat3(&c_node.node_poses[nodeIndex].scale, Scale);
                }
            }
        }
    }
}
