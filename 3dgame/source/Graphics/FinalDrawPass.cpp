#include "FinalDrawPass.h"
#include "Graphics.h"
#include "Shader.h"


void FinalDrawPass::setup(RenderGraphBuilder& builder)
{
    bloom_combine = builder.createRenderTarget("BloomCombine");

    builder.declareRead(bloom_combine);

    ID3D11Device* device = Graphics::Instance().Get_device();
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, ".//Data//Shader//sprite_vs.cso", sprite_vertex_shader.GetAddressOf(), sprite_input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, ".//Data//Shader//final_pass_ps.cso", final_rendering_pixel_shader.GetAddressOf());

    ID3D11ShaderResourceView* scene_shader_resource_view = nullptr;
    final_render_sprite = std::make_unique<Sprite>(device, scene_shader_resource_view);
}

void FinalDrawPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11RenderTargetView* backbuffer = Graphics::Instance().Get_render_target_view();

    FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
    ctx->ClearRenderTargetView(backbuffer, color);
    ctx->OMSetRenderTargets(1, &backbuffer, nullptr);

    ID3D11ShaderResourceView* srv = resources.getSRV(bloom_combine);
    final_render_sprite->setShaderResourceView(srv);

    GraphicsState::GetInstance().SetDepthStencilState(ctx, DEPTH_STATE::ZT_OFF_ZW_OFF);
    GraphicsState::GetInstance().SetBlendState(ctx, BLEND_STATE::NONE);
    GraphicsState::GetInstance().SetRasterizerState(ctx, RASTER_STATE::CULL_NONE);

    ctx->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
    ctx->IASetInputLayout(sprite_input_layout.Get());
    ctx->PSSetShader(final_rendering_pixel_shader.Get(), nullptr, 0);
    final_render_sprite->render(ctx, 0, 0, Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
}
