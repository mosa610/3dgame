#pragma once

#include "System.h"
#include "..//Graphics/Graphics.h"
#include "..//Graphics/PipeLineState.h"
#include "Entity.h"
#include "ComponentBone.h"
#include "ComponentModel.h"

class SystemModel : public ISystem {
public:
    void Initialize(Register& reg) override {
        ID3D11Device* device = Graphics::Instance().Get_device();
        ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
        for (Entity e : reg.view<ComponentModel, ComponentBone>()) {
            auto& bone = reg.getComponent<ComponentBone>(e);

            bone.skeleton_constant_buffer.Initialize(device, dc);
        }
    }
    void update(Register& reg, float dt) override {}
    void render(Register& reg) override {
        ID3D11Device* device = Graphics::Instance().Get_device();
        // begin
        {
            PipelineStateDesc desc;
            desc.vs_path = ".//Data//Shader//gltf_model_gbuffer_vs.cso";
            desc.ps_path = ".//Data//Shader//gltf_model_gbuffer_ps.cso";
            PipelineManager::instance().addPipelineState(device, desc);
        }
    }
};