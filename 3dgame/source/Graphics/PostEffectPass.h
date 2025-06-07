#pragma once
#include "RenderGraph.h"

class PostEffectPass : public RenderPass
{
public:


    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;
};