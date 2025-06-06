#pragma once
#include "RenderGraph.h"

class SceneRenderPass : public RenderPass {
public:
    ResourceHandle scene;
    ResourceHandle depth_stencil;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;
    void debug(RenderGraphResources& resources) override;
};