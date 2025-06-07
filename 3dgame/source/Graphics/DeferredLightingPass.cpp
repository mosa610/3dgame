#include "DeferredLightingPass.h"
#include "Graphics.h"
#include "shader.h"
#include "..//Component/ComponentIBL.h"

DeferredLightingPass::DeferredLightingPass(World* w) : world(w)
{
}

void DeferredLightingPass::setup(RenderGraphBuilder& builder)
{
    albedo = builder.createRenderTarget("gbuffer_albedo", ResourceType::RenderTarget);
    emissive = builder.createRenderTarget("gbuffer_emissive", ResourceType::RenderTarget);
    normal = builder.createRenderTarget("gbuffer_normal", ResourceType::RenderTarget, DXGI_FORMAT_R32G32B32A32_FLOAT);
    parameter = builder.createRenderTarget("gbuffer_parameter", ResourceType::RenderTarget);
    depth = builder.createRenderTarget("gbuffer_depth", ResourceType::RenderTarget, DXGI_FORMAT_R32_FLOAT);

    deferred_lighting = builder.createRenderTarget("DeferredLighting", ResourceType::RenderTarget, DXGI_FORMAT_R32G32B32A32_FLOAT);

    builder.declareRead(albedo);
    builder.declareRead(emissive);
    builder.declareRead(normal);
    builder.declareRead(parameter);
    builder.declareRead(depth);

    ID3D11Device* device = Graphics::Instance().Get_device();
    create_ps_from_cso(device, ".//Data//Shader//deferred_rendering_emissive_ps.cso", deferred_rendering_emissive_pixel_shader.GetAddressOf());
    create_ps_from_cso(device, ".//Data//Shader//deferred_rendering_indirect_ps.cso", deferred_rendering_indirect_pixel_shader.GetAddressOf());
    create_ps_from_cso(device, ".//Data//Shader//deferred_rendering_directional_ps.cso", deferred_rendering_directional_pixel_shader.GetAddressOf());

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    create_vs_from_cso(device, ".//Data//Shader//sprite_vs.cso", sprite_vertex_shader.GetAddressOf(), sprite_input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, ".//Data//Shader//sprite_ps.cso", sprite_pixel_shader.GetAddressOf());

    deferred_rendering_sprite = std::make_unique<Sprite>(device, sprite_shader_resource_view.Get());
}

void DeferredLightingPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    setCommonResources(ctx, resources);

    //	間接光処理
    {
        //	IBLテクスチャを設定
        for(auto e : world->getRegister().view<ComponentIBL>())
        {
            auto& ibl = world->getRegister().getComponent<ComponentIBL>(e);
            if (ibl.diffuse_iem_shader_resource_view != nullptr)
            {
                ctx->PSSetShaderResources(33, 1, ibl.diffuse_iem_shader_resource_view.GetAddressOf());
            }
            if (ibl.specular_pmrem_shader_resource_view != nullptr)
            {
                ctx->PSSetShaderResources(34, 1, ibl.specular_pmrem_shader_resource_view.GetAddressOf());
            }
            if (ibl.lut_ggx_shader_resource_view != nullptr)
            {
                ctx->PSSetShaderResources(35, 1, ibl.lut_ggx_shader_resource_view.GetAddressOf());
            }
        }

        //	全画面描画
        GraphicsState::GetInstance().SetDepthStencilState(ctx, DEPTH_STATE::ZT_OFF_ZW_OFF);
        GraphicsState::GetInstance().SetBlendState(ctx, BLEND_STATE::ALPHA);
        GraphicsState::GetInstance().SetRasterizerState(ctx, RASTER_STATE::CULL_NONE);

        ctx->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
        ctx->IASetInputLayout(sprite_input_layout.Get());

        ctx->PSSetShader(deferred_rendering_indirect_pixel_shader.Get(), nullptr, 0);
        deferred_rendering_sprite->render(ctx, 0, 0, Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
    }

    //	自己発光処理
    {
        GraphicsState::GetInstance().SetDepthStencilState(ctx, DEPTH_STATE::ZT_OFF_ZW_OFF);
        GraphicsState::GetInstance().SetBlendState(ctx, BLEND_STATE::ADD);
        GraphicsState::GetInstance().SetRasterizerState(ctx, RASTER_STATE::CULL_NONE);

        ctx->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
        ctx->IASetInputLayout(sprite_input_layout.Get());
        ctx->PSSetShader(deferred_rendering_emissive_pixel_shader.Get(), nullptr, 0);
        deferred_rendering_sprite->render(ctx, 0, 0, Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
    }
}

void DeferredLightingPass::debug(RenderGraphResources& resources)
{
    // ここでDebugしてみる
    ID3D11ShaderResourceView* srvs[] = {
        resources.getSRV(albedo),
        resources.getSRV(emissive),
        resources.getSRV(normal),
        resources.getSRV(parameter),
        resources.getSRV(depth)
    };

    if (ImGui::TreeNode("DeferredLightingPass")) {
        static const char* GBufferNames[] = {
            "base_color",
            "emissive",
            "normal",
            "parameters",
            "depth",
        };

        ImGui::NewLine();

        for (int i = 0; i < 5; ++i) {
            ImGui::Text(GBufferNames[i]);
            ImGui::Image((ImTextureID)srvs[i], ImVec2(256, 144), ImVec2(0, 0), ImVec2(1, 1),
                ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
            ImGui::NewLine();
        }
        ImGui::TreePop();
    }
}

void DeferredLightingPass::setCommonResources(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    static constexpr int GBufferSRVIndex = 0;
    ID3D11ShaderResourceView* shader_resource_views[] =
    {
        resources.getSRV(albedo),
        resources.getSRV(emissive),
        resources.getSRV(normal),
        resources.getSRV(parameter),
        resources.getSRV(depth)
    };
    ctx->PSSetShaderResources(GBufferSRVIndex, ARRAYSIZE(shader_resource_views), shader_resource_views);

    deferred_rendering_sprite->setShaderResourceView(resources.getSRV(albedo));
}
