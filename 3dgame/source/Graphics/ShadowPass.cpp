#include "ShadowPass.h"

CONST LONG SHADOWMAP_WIDTH{ 1024 };
CONST LONG SHADOWMAP_HEIGHT{ 1024 };
CONST float SHADOWMAP_DRAWRECT{ 50 };

ShadowPass::ShadowPass(World* world) : world(world)
{

}

void ShadowPass::setup(RenderGraphBuilder& builder)
{
    shadow_map = builder.createRenderTarget("ShadowMapD", ResourceType::DepthStencil, DXGI_FORMAT_R32_FLOAT);

    builder.declareWrite(shadow_map);
}

void ShadowPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{

}
