#pragma once
#include "RenderGraph.h"

class World;

class GbufferRenderPass : public RenderPass
{
public:
    GbufferRenderPass(World* w);

    ResourceHandle scene;

    ResourceHandle albedo;
    ResourceHandle emissive;
    ResourceHandle normal;
    ResourceHandle parameter;
    ResourceHandle depth;

    ResourceHandle depthStencil;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;
    void debug(RenderGraphResources& resources) override;

private:
    void begin(ID3D11DeviceContext* ctx, RenderGraphResources& resources);

    void end(ID3D11DeviceContext* ctx, RenderGraphResources& resources);

private:
    World* world;
};