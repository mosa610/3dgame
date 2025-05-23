#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <unordered_set>
#include "imgui/IconsFontAwesome6.h"

#include "Component/ComponentNode.h"

class ModelNodeTreeEditor
{
public:
    static void Draw(Node* node)
    {
        ImGuiTreeNodeFlags flags = node->children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None;

        bool opened = ImGui::TreeNodeEx(node->name.c_str(), flags);
        if (opened)
        {
            ImGui::PushID(node);
            ImGui::InputFloat3("Position", &node->position.x);
            ImGui::InputFloat3("Rotation", &node->rotation.x);
            ImGui::InputFloat3("Scale", &node->scale.x);
            ImGui::PopID();

            for (Node* child : node->children)
            {
                Draw(child); // 再帰呼び出し
            }
            ImGui::TreePop();
        }
    }
};