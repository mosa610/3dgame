#pragma once

#include "Register.h"
#include "System.h"
#include "ComponentModel.h"
#include "ComponentNode.h"

class World
{
public:
    World() { registerComponentCallback(); }
    ~World() {}
public:
    void initialize() {
        for (auto& sys : systems) sys->Initialize(reg);
    }

    void update(float dt) {
        for (auto& sys : systems) sys->update(reg, dt);
    }

    void render() {
        for (auto& sys : systems) sys->render(reg);
    }

    template<typename T, typename... Args>
    void addSystem(Args&&... args) {
        systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    Register& getRegister() { return reg; }

private:
    void registerComponentCallback() {
        // ConmponentModel に対するコールバック
        reg.setOnComponentAdded<ComponentModel>([](Register& reg, Entity e, ComponentModel& model) {
            ID3D11Device* device = Graphics::Instance().Get_device();
            if (!model.resource) {
                model.resource = std::make_shared<ModelResource>();
                model.resource->Load(device, model.file_name, 0);
            }

            if (!reg.hasComponent<ComponentNode>(e)) {
                reg.addComponent<ComponentNode>(e ,ComponentNode {});
            }
            });

        // ConmponentNode に対するコールバック
        reg.setOnComponentAdded<ComponentNode>([](Register& reg, Entity e, ComponentNode& node) {
            auto& model = reg.getComponent<ComponentModel>(e);

            std::vector<ModelResource::Node> nodes = model.resource->GetNodes();

            node.nodes.resize(nodes.size());
            node.node_poses.resize(nodes.size());

            for (size_t node_index = 0; node_index < node.nodes.size(); node_index++)
            {
                auto&& src = nodes[node_index];
                auto&& dst = node.nodes[node_index];
                auto&& anim = node.node_poses[node_index];

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

                anim.scale = dst.scale;
                anim.rotation = dst.rotation;
                anim.position = dst.position;

                if (dst.parent != nullptr)
                {
                    dst.parent->children.emplace_back(&dst);
                }
            }
            });


    }

private:

    Register reg;
    std::vector<std::unique_ptr<ISystem>> systems;
};
