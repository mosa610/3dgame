#include "DeferredLightingPass.h"
#include "Graphics.h"
#include "shader.h"
#include "..//Component/ComponentScene.h"
#include "..//Component/ComponentIBL.h"
#include "..//Component/SystemModel.h"
#include "render_context.h"
#include "..//Mouse.h"
#include "..//Camera.h"

CONST LONG SHADOWMAP_WIDTH{ 1024 };
CONST LONG SHADOWMAP_HEIGHT{ 1024 };
CONST float SHADOWMAP_DRAWRECT{ 50 };


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

    builder.declareRead(albedo);
    builder.declareRead(emissive);
    builder.declareRead(normal);
    builder.declareRead(parameter);
    builder.declareRead(depth);

    shadow_map = builder.createRenderTarget("ShadowMap", ResourceType::DepthStencilSRV, DXGI_FORMAT_R32_TYPELESS);

    builder.declareWrite(shadow_map);

    deferred_lighting = builder.createRenderTarget("DeferredLighting", ResourceType::RenderTarget, DXGI_FORMAT_R32G32B32A32_FLOAT);

    builder.declareWrite(deferred_lighting);

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
    ID3D11RenderTargetView* rtv = resources.getRTV(deferred_lighting);

    FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
    ctx->ClearRenderTargetView(rtv, color);
    ctx->OMSetRenderTargets(1, &rtv, nullptr);

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

void DeferredLightingPass::directionalShadowRendering(ID3D11DeviceContext* ctx, RenderGraphResources& resources, directional_light_constants& light_data)
{
    ID3D11DepthStencilView* dsv = resources.getDSV(shadow_map);
    ctx->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
    ctx->OMSetRenderTargets(0, nullptr, dsv);

    D3D11_VIEWPORT* viewport = resources.getViewport(shadow_map);
    viewport->TopLeftX = 0.0f;
    viewport->TopLeftY = 0.0f;
    viewport->Width = SHADOWMAP_WIDTH;
    viewport->Height = SHADOWMAP_HEIGHT;
    viewport->MinDepth = 0.0f;
    viewport->MaxDepth = 1.0f;
    ctx->RSSetViewports(1, resources.getViewport(shadow_map));

    // ライトの位置から見た視線行列を生成
    DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat4(&light_data.directional_light.direction);
    LightPosition = DirectX::XMVectorScale(LightPosition, -50);
    DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
        DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
        DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

    // シャドウマップに描画したい範囲の射影行列を生成
    DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(SHADOWMAP_DRAWRECT, SHADOWMAP_DRAWRECT, 0.1f, 200.0f);
    // ライトビュー行列を保存
    DirectX::XMStoreFloat4x4(&light_data.light_view_projection, V * P);

    // 定数バッファの更新
    if(auto e_scene = world->getRegister().getEntityByName("Scene"); e_scene != INVALID_ENTITY)
    {
        // 0番はメッシュ側で更新している
        if(world->getRegister().hasComponent<ComponentScene>(e_scene) && world->getRegister().hasComponent<ComponentLight>(e_scene))
        {
            auto& c_scene = world->getRegister().getComponent<ComponentScene>(e_scene);
            auto& c_light = world->getRegister().getComponent<ComponentLight>(e_scene);

            gbuffer_scene_constants scene{};

            DirectX::XMFLOAT2 cursor_position = Mouse::GetInstance().GetCursorPosition();
            scene.options.x = static_cast<float>(cursor_position.x);
            scene.options.y = static_cast<float>(cursor_position.y);
            scene.options.z = c_scene.timer;
            scene.options.w = false;
            DirectX::XMFLOAT3 camera_position = Camera::Instance().GetPosition();
            scene.camera_position.x = camera_position.x;
            scene.camera_position.y = camera_position.y;
            scene.camera_position.z = camera_position.z;
            scene.camera_position.w = 1;
            DirectX::XMStoreFloat4x4(&scene.view_transform, V);
            DirectX::XMStoreFloat4x4(&scene.projection_transform, P);
            scene.view_projection_transform = light_data.light_view_projection;
            DirectX::XMStoreFloat4x4(&scene.inverse_view_transform, DirectX::XMMatrixInverse(nullptr, V));
            DirectX::XMStoreFloat4x4(&scene.inverse_projection_transform, DirectX::XMMatrixInverse(nullptr, P));
            DirectX::XMStoreFloat4x4(&scene.inverse_view_projection_transform, DirectX::XMMatrixInverse(nullptr, V * P));

            ctx->UpdateSubresource(c_light.shadow_buffer.Get(), 0, 0, &scene, 0, 0);
            ctx->VSSetConstantBuffers(1, 1, c_light.shadow_buffer.GetAddressOf());
            ctx->PSSetConstantBuffers(1, 1, c_light.shadow_buffer.GetAddressOf());
        }
    }
    GraphicsState::GetInstance().SetBlendState(ctx, BLEND_STATE::ALPHA);
    GraphicsState::GetInstance().SetDepthStencilState(ctx, DEPTH_STATE::ZT_ON_ZW_ON);

    world->getSystem<SystemModel>()->render(world->getRegister());
}
