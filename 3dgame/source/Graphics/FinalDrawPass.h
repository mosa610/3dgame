#pragma once
#include "RenderGraph.h"
#include "Sprite.h"

class FinalDrawPass : public RenderPass {
public:
    ResourceHandle bloom_combine;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> final_rendering_pixel_shader;
    std::unique_ptr<Sprite> final_render_sprite;
};