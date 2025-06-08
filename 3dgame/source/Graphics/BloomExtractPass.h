#pragma once
#include "RenderGraph.h"
#include "fullscreen_quad.h"
#include "..//Component/World.h"

class BloomExtractPass : public RenderPass
{
public:
    BloomExtractPass(World* world) : world(world) {}

    ResourceHandle deferred_input;
    ResourceHandle bloom_extract;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> glow_extraction_ps;
    std::unique_ptr<fullscreen_quad> bit_block_transfer;

    struct bloom_constants
    {
        float bloom_extraction_threshold;
        float bloom_intensity;
        float something[2];
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    World* world;
};