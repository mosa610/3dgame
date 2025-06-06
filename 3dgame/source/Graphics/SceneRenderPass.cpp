#include "SceneRenderPass.h"

void SceneRenderPass::setup(RenderGraphBuilder& builder)
{
    scene = builder.createRenderTarget("Scene");
    depth_stencil = builder.createRenderTarget("DepthStencil", ResourceType::DepthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
    builder.declareWrite(scene);
    builder.declareWrite(depth_stencil);
}

void SceneRenderPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
    ctx->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
    ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    ctx->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
    ctx->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

    FLOAT color[4]{ 0.3f, 0.3f, 0.3f, 1.0f };
    ID3D11RenderTargetView* rtv = resources.getRTV(scene);
    ID3D11DepthStencilView* dsv = resources.getDSV(depth_stencil);
    ctx->ClearRenderTargetView(rtv, color);
    ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    ctx->OMSetRenderTargets(1, &rtv, dsv);
}

void SceneRenderPass::debug(RenderGraphResources& resources)
{
}
