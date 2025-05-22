#pragma once

#include "System.h"
#include "Register.h"
#include "ComponentModel.h"
#include "ComponentNode.h"
#include "..//Graphics/Graphics.h"

class ModelResource;

class SystemLoadModel : public ISystem
{
    void Initialize(Register& reg) override {
        ID3D11Device* device = Graphics::Instance().Get_device();
        for (Entity e : reg.view<ComponentModel>()) {
            auto& model = reg.getComponent<ComponentModel>(e);
            if(!model.resource) model.resource = std::make_shared<ModelResource>();
            model.resource->Load(device, model.file_name, 0);
            if (reg.hasComponent<ComponentNode>(e))
            {
                auto& node = reg.getComponent<ComponentNode>(e);
                std::vector<ModelResource::Node> nodes = model.resource->GetNodes();

                node.nodes.resize(nodes.size());
                for (size_t node_index = 0; node_index < node.nodes.size(); node_index++)
                {
                    auto&& src = nodes[node_index];
                    auto&& dst = node.nodes[node_index];
                    
                    dst.name = src.name.c_str();
                    dst.parent = src.parentIndex >= 0 ? &node.nodes[src.parentIndex] : nullptr;
                    dst.myIndex = src.myIndex;
                    dst.parentIndex = src.parentIndex;
                    dst.jointIndex = src.jointIndex;
                    dst.scale = src.scale;
                    dst.rotation = src.rotation;
                    dst.position = src.position;

                    dst.localTransform = src.localTransform;
                    dst.globalTransform = src.globalTransform;
                    dst.worldTransform = src.worldTransform;
                }
                {
                    
                }

            }

            
        }
    }
    void update(Register& reg, float dt) override {}
    void render(Register& reg) override {}
};