#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <unordered_set>
#include "imgui/IconsFontAwesome6.h"

#include "Component/Register.h"
#include "Component/Entity.h"
#include "component/ComponentModel.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentNode.h"

class ModelNodeTreeEditor
{
private:
    Node* selectedNode = nullptr;  // メンバ変数で選択ノードを保持
    Entity currentEntity;
public:
    void HierarchyDraw(Entity e,Node* node)
    {
        bool isSelected = (node == selectedNode);
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (node->children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;
        if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;

        bool opened = ImGui::TreeNodeEx(node->name.c_str(), flags);

        if (ImGui::IsItemClicked()) {
            selectedNode = node;
            currentEntity = e;
        }

        if (opened)
        {
            for (Node* child : node->children)
            {
                HierarchyDraw(e, child); // 再帰呼び出し
            }
            ImGui::TreePop();
        }
    }

    void InspectorDraw(Register& reg) {

        if (IsRootNode(selectedNode))
        {
            if(reg.hasComponent<ComponentModel>(currentEntity)) {
                ImGui::PushID(currentEntity);
                ImGui::Text("Model: %s", reg.getEntityName(currentEntity).c_str());
                if (reg.hasComponent<ComponentNode>(currentEntity)) {
                    ImGui::InputFloat3("Position", &reg.getComponent<ComponentTransform>(currentEntity).position.x);
                    ImGui::InputFloat3("Rotation", &reg.getComponent<ComponentTransform>(currentEntity).rotation.x);
                    ImGui::InputFloat3("Scale", &reg.getComponent<ComponentTransform>(currentEntity).scale.x);
                }
                if (reg.hasComponent<ComponentAnimation>(currentEntity))
                {
                    ImGui::InputFloat("Animation Time", &reg.getComponent<ComponentAnimation>(currentEntity).animation_time);
                    ImGui::InputInt("Animation Index", &reg.getComponent<ComponentAnimation>(currentEntity).animation_index);
                }
                ImGui::PopID();
            }
        }
        else if (selectedNode)
        {
            ImGui::PushID(selectedNode);
            ImGui::Text("Node: %s", selectedNode->name.c_str());
            ImGui::InputFloat3("Position", &selectedNode->position.x);
            ImGui::InputFloat3("Rotation", &selectedNode->rotation.x);
            ImGui::InputFloat3("Scale", &selectedNode->scale.x);
            ImGui::PopID();
        }
        else
        {
            ImGui::Text("No node selected.");
        }
    }
private:
    bool IsRootNode(Node* node)
    {
        if (node == nullptr)
        {
            return false;
        }
        return (node->parent == nullptr);
    }
};