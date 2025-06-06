#include "GbufferRenderPass.h"
#include "GraphicsState.h"
#include "../Component/World.h"
#include "..//Component/SystemModel.h"
#include "..//Component/SystemSkymap.h"
#include "..//imgui/imgui.h"

// コンストラクタ
GbufferRenderPass::GbufferRenderPass(World* w)
    : world(w) {
}

void GbufferRenderPass::setup(RenderGraphBuilder& builder)
{
    albedo = builder.createRenderTarget("gbuffer_albedo", ResourceType::RenderTarget);
    emissive = builder.createRenderTarget("gbuffer_emissive", ResourceType::RenderTarget);
    normal = builder.createRenderTarget("gbuffer_normal", ResourceType::RenderTarget, DXGI_FORMAT_R32G32B32A32_FLOAT);
    parameter = builder.createRenderTarget("gbuffer_parameter", ResourceType::RenderTarget);
    depth = builder.createRenderTarget("gbuffer_depth", ResourceType::RenderTarget, DXGI_FORMAT_R32_FLOAT);

    scene = builder.createRenderTarget("Scene", ResourceType::RenderTarget);
    depthStencil = builder.createRenderTarget("DepthStencil", ResourceType::DepthStencil);

    builder.declareWrite(albedo);
    builder.declareWrite(emissive);
    builder.declareWrite(normal);
    builder.declareWrite(parameter);
    builder.declareWrite(depth);

    builder.declareRead(scene);
    builder.declareRead(depthStencil);
}

void GbufferRenderPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    begin(ctx, resources);
    world->getSystem<SystemSkymap>()->render(world->getRegister());
    world->getSystem<SystemModel>()->render(world->getRegister());
    end(ctx, resources);
}

void GbufferRenderPass::debug(RenderGraphResources& resources)
{
    ID3D11ShaderResourceView* srvs[] = {
        resources.getSRV(albedo),
        resources.getSRV(emissive),
        resources.getSRV(normal),
        resources.getSRV(parameter),
        resources.getSRV(depth)
    };

    if (ImGui::TreeNode("texture")) {
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

void GbufferRenderPass::begin(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11SamplerState* sampler_states[] =
    {
        GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::POINT).Get(),
        GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR).Get(),
        GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::ANISOTROPIC).Get(),
        GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR_BORDER_BLACK).Get(),
        GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR_BORDER_WHITE).Get(),
    };
    ctx->PSSetSamplers(0, _countof(sampler_states), sampler_states);

    ID3D11RenderTargetView* rtvs[] = {
        resources.getRTV(albedo),
        resources.getRTV(emissive),
        resources.getRTV(normal),
        resources.getRTV(parameter),
        resources.getRTV(depth)
    };

    FLOAT clear_color[]{ 0.f, 0.f, 0.f, 0.f };
    for (int i = 0; i < 5; ++i) {
        if (i == 4) {
            clear_color[0] = 1.f;
        }
        else {
            clear_color[0] = 0.f;
        }
        ctx->ClearRenderTargetView(rtvs[i], clear_color);
    }
    ID3D11DepthStencilView* dsv = resources.getDSV(depthStencil);
    ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    ctx->OMSetRenderTargets(5, rtvs, dsv);
}

void GbufferRenderPass::end(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11RenderTargetView* rtv = resources.getRTV(scene);

    FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
    //	出力先をシーンに変更
    {
        ctx->ClearRenderTargetView(rtv, color);
        //dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        ctx->OMSetRenderTargets(1, &rtv, nullptr);
    }
}
