#pragma once
#include "RenderGraph.h"
#include "fullscreen_quad.h"

class BloomCombinePass : public RenderPass
{
public:
    ResourceHandle deferred;
    ResourceHandle bloom;

    ResourceHandle bloom_combine;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;

private:
    std::unique_ptr<fullscreen_quad> bit_block_transfer;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> bloom_combine_ps;
};