#pragma once
#include "RenderGraph.h"
#include <memory>
#include "bloom.h"

class PostEffectPass : public RenderPass
{
public:
    ResourceHandle deferred_lighting;
    ResourceHandle posteffect;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;

private:
    std::unique_ptr<bloom> bloomer;
};