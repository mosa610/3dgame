#include "PostEffectPass.h"
#include "Graphics.h"

void PostEffectPass::setup(RenderGraphBuilder& builder)
{
    deferred_lighting = builder.createRenderTarget("DeferredLighting");

    builder.declareRead(deferred_lighting);

    posteffect = builder.createRenderTarget("PostEffect");

    builder.declareWrite(posteffect);

    bloomer = std::make_unique<bloom>(Graphics::Instance().Get_device(), Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
}

void PostEffectPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11RenderTargetView* rtv = resources.getRTV(posteffect);
    FLOAT ClearColor[]{ 0.f, 0.f, 0.f, 0.f };
    ctx->ClearRenderTargetView(rtv, ClearColor);
    ctx->OMSetRenderTargets(1, &rtv, nullptr);


}
