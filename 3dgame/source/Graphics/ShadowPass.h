#pragma once
#include "RenderGraph.h"
#include "..//Component/World.h"

class ShadowPass : public RenderPass {
public:
    ResourceHandle shadow_map;

    ShadowPass(World* world);
    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;

private:
    World* world;
};