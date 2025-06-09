#include "BloomCombinePass.h"
#include "Graphics.h"
#include "shader.h"

void BloomCombinePass::setup(RenderGraphBuilder& builder)
{
    deferred = builder.createRenderTarget("DeferredLighting");

    builder.declareRead(deferred);

    bloom = builder.createRenderTarget("Bloom");

    builder.declareRead(bloom);

    bloom_combine = builder.createRenderTarget("BloomCombine");

    builder.declareWrite(bloom_combine);

    bit_block_transfer = std::make_unique<fullscreen_quad>(Graphics::Instance().Get_device());

    create_ps_from_cso(Graphics::Instance().Get_device(), ".//Data//Shader//bloom_final_pass_ps.cso", bloom_combine_ps.GetAddressOf());
}

void BloomCombinePass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11RenderTargetView* rtv = resources.getRTV(bloom_combine);
    FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
    ctx->ClearRenderTargetView(rtv, color);
    ctx->OMSetRenderTargets(1, &rtv, nullptr);

    ID3D11ShaderResourceView* srvs[2] = {
        resources.getSRV(deferred),
        resources.getSRV(bloom),
    };

    bit_block_transfer->blit(ctx, srvs, 0, 2, bloom_combine_ps.Get());
}
