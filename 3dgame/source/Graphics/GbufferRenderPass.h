#pragma once
#include "RenderGraph.h"

class GbufferRenderPass :public RenderPass
{
public:
    ResourceHandle albedo;
    ResourceHandle emissive;
    ResourceHandle normal;
    ResourceHandle parameter;
    ResourceHandle depth;

    void setup(RenderGraphBuilder& builder) override {
        albedo = builder.createRenderTarget("gbuffer_albedo", ResourceType::RenderTarget);
        emissive = builder.createRenderTarget("gbuffer_emissive", ResourceType::RenderTarget);
        normal = builder.createRenderTarget("gbuffer_normal", ResourceType::RenderTarget);
        parameter = builder.createRenderTarget("gbuffer_parameter", ResourceType::RenderTarget);
        depth = builder.createRenderTarget("gbuffer_depth", ResourceType::RenderTarget);

        builder.declareWrite(albedo);
        builder.declareWrite(emissive);
        builder.declareWrite(normal);
        builder.declareWrite(parameter);
        builder.declareWrite(depth);
    }
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override {
        begin(ctx, resources);
    }

    void begin(ID3D11DeviceContext* ctx, RenderGraphResources& resources) {
        ID3D11RenderTargetView* rtvs[] = {
            resources.getRTV(albedo),
            resources.getRTV(emissive),
            resources.getRTV(normal),
            resources.getRTV(parameter),
            resources.getRTV(depth)
        };
        ctx->OMSetRenderTargets(5, rtvs, nullptr);

        FLOAT clear_color[]{ 0.f, 0.f, 0.f, .0f };
        for (int i = 0; i < 5; ++i)
        {
            if (i == 4) {
                clear_color[0] = 1.f;
            }
            else {
                clear_color[0] = 0.f;
            }
            ctx->ClearRenderTargetView(rtvs[i], clear_color);
        }
    }
};