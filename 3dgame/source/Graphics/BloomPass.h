#pragma once
#include "RenderGraph.h"
#include "fullscreen_quad.h"

class BloomPass : public RenderPass
{
private:
    static const size_t downsampled_count = 6;
public:
    ResourceHandle bloom_extract;
    ResourceHandle gaussian_blur[downsampled_count][2];
    ResourceHandle bloom;

    void setup(RenderGraphBuilder& builder) override;
    void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) override;

private:
    void setRTV(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_downsampling_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_horizontal_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_vertical_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_upsampling_ps;

    std::unique_ptr<fullscreen_quad> bit_block_transfer;
};