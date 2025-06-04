#pragma once

#include "Register.h"
#include "System.h"
#include "ComponentModel.h"
#include "ComponentNode.h"
#include "ComponentAnimation.h"
#include "ComponentBone.h"
#include "ComponentMaterial.h"
#include "..//Graphics/RenderResourceContext.h"

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

    void setRenderContext(RenderResourceContext* context) {
        for (auto& sys : systems) sys->SetRenderContext(context);
    }

    void drawDebugGUI() {
        for (auto& sys : systems) sys->drawDebugGUI(reg);
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

            if(!reg.hasComponent<ComponentMaterial>(e)) {
                reg.addComponent<ComponentMaterial>(e, ComponentMaterial{});
            }

            if(!reg.hasComponent<ComponentBone>(e)) {
                reg.addComponent<ComponentBone>(e, ComponentBone{});
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

        // ComponentAnimation に対するコールバック
        reg.setOnComponentAdded<ComponentAnimation>([](Register& reg, Entity e, ComponentAnimation& anim) {
            if (!reg.hasComponent<ComponentBone>(e))
            {
                reg.addComponent<ComponentBone>(e, ComponentBone{});
            }
            });

        // ConmponentBone に対するコールバック
        reg.setOnComponentAdded<ComponentBone>([](Register& reg, Entity e, ComponentBone& bone) {
            if(!reg.hasComponent<ComponentModel>(e)) return;
                ComponentModel& model = reg.getComponent<ComponentModel>(e);
                if(!reg.hasComponent<ComponentNode>(e)) return;
                ComponentNode& node = reg.getComponent<ComponentNode>(e);
            const std::vector<ModelResource::Mesh>& resMeshes = model.resource->GetMeshes();

            for (auto&& mesh : resMeshes)
            {
                std::vector<Bone> bones;
                bones.resize(mesh.bones.size());
                for (size_t boneIndex = 0; boneIndex < bones.size(); ++boneIndex)
                {
                    auto&& src = mesh.bones.at(boneIndex);
                    auto&& dst = bones.at(boneIndex);

                    dst.node = &node.nodes[src.nodeIndex];
                    dst.nodeIndex = src.nodeIndex;
                    dst.offsetTransform = src.offsetTransform;
                    
                }
                bone.bones.emplace(mesh.index, bones);
            }
            });

        // ComponentMaterial に対するコールバック
        reg.setOnComponentAdded<ComponentMaterial>([](Register& reg, Entity e, ComponentMaterial& c_material) {
            if (!reg.hasComponent<ComponentModel>(e)) return;
            auto& model = reg.getComponent<ComponentModel>(e);

            for (std::vector<ModelResource::Material>::const_reference material : model.resource->GetMaterials())
            {
                c_material.material_dates.push_back(material.data);
            }

            D3D11_BUFFER_DESC bufferDesc{};
            bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ModelResource::Material::CBuffer) * c_material.material_dates.size());
            bufferDesc.StructureByteStride = sizeof(ModelResource::Material::CBuffer);
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            D3D11_SUBRESOURCE_DATA subResourceData{};
            subResourceData.pSysMem = c_material.material_dates.data();

            ID3D11Device* device = Graphics::Instance().Get_device();
            HRESULT hr = device->CreateBuffer(&bufferDesc, &subResourceData, c_material.material_buffer.GetAddressOf());
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
            shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(c_material.material_dates.size());

            hr = device->CreateShaderResourceView(c_material.material_buffer.Get(), &shaderResourceViewDesc, c_material.material_resource_view.GetAddressOf());
            //  _resource->materialResourceView -> シェーダーのメンバ->materialResourceView
            _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            });
    }

private:

    Register reg;
    std::vector<std::unique_ptr<ISystem>> systems;
};
