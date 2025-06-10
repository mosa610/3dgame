#pragma once
#include "RenderGraph.h"
#include "Sprite.h"
#include "..//Component/World.h"
#include "../Component/ComponentLight.h"

class DeferredLightingPass : public RenderPass
{
public:
    DeferredLightingPass(World* w);

    ResourceHandle deferred_lighting;

    ResourceHandle albedo;
    ResourceHandle emissive;
    ResourceHandle normal;
    ResourceHandle parameter;
    ResourceHandle depth;

    ResourceHandle shadow_map;
	
    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;
    void debug(RenderGraphResources& resources) override;
private:

    void setCommonResources(ID3D11DeviceContext* ctx, RenderGraphResources& resources);

    void directionalShadowRendering(ID3D11DeviceContext* ctx, RenderGraphResources& resources, directional_light_constants& light_data);

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_emissive_pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_indirect_pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_directional_pixel_shader;

    // Sprite
    Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

    std::unique_ptr<Sprite> deferred_rendering_sprite;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sprite_shader_resource_view;

    World* world;
};