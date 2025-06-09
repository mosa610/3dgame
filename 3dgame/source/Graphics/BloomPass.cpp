#include "BloomPass.h"
#include "Graphics.h"
#include "shader.h"

void BloomPass::setup(RenderGraphBuilder& builder)
{
    bloom_extract = builder.createRenderTarget("BloomExtract");

    builder.declareRead(bloom_extract);

    for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
    {
        gaussian_blur[downsampled_index][0] = builder.createRenderTarget("Bloom0." + std::to_string(downsampled_index), ResourceType::RenderTarget, DXGI_FORMAT_R16G16B16A16_FLOAT, downsampled_index);
        gaussian_blur[downsampled_index][1] = builder.createRenderTarget("Bloom1." + std::to_string(downsampled_index), ResourceType::RenderTarget, DXGI_FORMAT_R16G16B16A16_FLOAT, downsampled_index);

        builder.declareWrite(gaussian_blur[downsampled_index][0]);
        builder.declareWrite(gaussian_blur[downsampled_index][1]);
    }

    bloom = builder.createRenderTarget("Bloom", ResourceType::RenderTarget, DXGI_FORMAT_R8G8B8A8_UNORM);

    builder.declareWrite(bloom);

    ID3D11Device* device = Graphics::Instance().Get_device();
    create_ps_from_cso(device, ".//Data//Shader//gaussian_blur_downsampling_ps.cso", gaussian_blur_downsampling_ps.GetAddressOf());
    create_ps_from_cso(device, ".//Data//Shader//gaussian_blur_horizontal_ps.cso", gaussian_blur_horizontal_ps.GetAddressOf());
    create_ps_from_cso(device, ".//Data//Shader//gaussian_blur_vertical_ps.cso", gaussian_blur_vertical_ps.GetAddressOf());
    create_ps_from_cso(device, ".//Data//Shader//gaussian_blur_upsampling_ps.cso", gaussian_blur_upsampling_ps.GetAddressOf());

    bit_block_transfer = std::make_unique<fullscreen_quad>(device);
}

void BloomPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11ShaderResourceView* null_shader_resource_view{};
    ID3D11ShaderResourceView* srv = resources.getSRV(bloom_extract);

    // Downsampling
    setRTV(ctx, resources.getRTV(gaussian_blur[0][0]));
    ctx->RSSetViewports(1, resources.getViewport(gaussian_blur[0][0]));
    bit_block_transfer->blit(ctx, &srv, 0, 1, gaussian_blur_downsampling_ps.Get());
    ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);

    // Ping-pong gaussian blur
    srv = resources.getSRV(gaussian_blur[0][0]);
    setRTV(ctx, resources.getRTV(gaussian_blur[0][1]));
    ctx->RSSetViewports(1, resources.getViewport(gaussian_blur[0][1]));
    bit_block_transfer->blit(ctx, &srv, 0, 1, gaussian_blur_horizontal_ps.Get());
    ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);

    srv = resources.getSRV(gaussian_blur[0][1]);
    setRTV(ctx, resources.getRTV(gaussian_blur[0][0]));
    ctx->RSSetViewports(1, resources.getViewport(gaussian_blur[0][0]));
    bit_block_transfer->blit(ctx, &srv, 0, 1, gaussian_blur_vertical_ps.Get());
    ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);

    for (size_t downsampled_index = 1; downsampled_index < downsampled_count; ++downsampled_index)
    {
        // Downsampling
        srv = resources.getSRV(gaussian_blur[downsampled_index - 1][0]);
        setRTV(ctx, resources.getRTV(gaussian_blur[downsampled_index][0]));
        ctx->RSSetViewports(1, resources.getViewport(gaussian_blur[downsampled_index][0]));
        bit_block_transfer->blit(ctx, &srv, 0, 1, gaussian_blur_downsampling_ps.Get());
        ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);

        // Ping-pong gaussian blur
        srv = resources.getSRV(gaussian_blur[downsampled_index][0]);
        setRTV(ctx, resources.getRTV(gaussian_blur[downsampled_index][1]));
        ctx->RSSetViewports(1, resources.getViewport(gaussian_blur[downsampled_index][1]));
        bit_block_transfer->blit(ctx, &srv, 0, 1, gaussian_blur_horizontal_ps.Get());
        ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);

        srv = resources.getSRV(gaussian_blur[downsampled_index][1]);
        setRTV(ctx, resources.getRTV(gaussian_blur[downsampled_index][0]));
        ctx->RSSetViewports(1, resources.getViewport(gaussian_blur[downsampled_index][0]));
        bit_block_transfer->blit(ctx, &srv, 0, 1, gaussian_blur_vertical_ps.Get());
        ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);
    }

    setRTV(ctx, resources.getRTV(bloom));
    ctx->RSSetViewports(1, resources.getViewport(bloom));
    std::vector<ID3D11ShaderResourceView*> shader_resource_views;
    for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
    {
        shader_resource_views.push_back(resources.getSRV(gaussian_blur[downsampled_index][0]));
    }
    bit_block_transfer->blit(ctx, shader_resource_views.data(), 0, downsampled_count, gaussian_blur_upsampling_ps.Get());
    ctx->PSSetShaderResources(0, 1, &null_shader_resource_view);
}

void BloomPass::setRTV(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv)
{
    FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
    ctx->ClearRenderTargetView(rtv, color);
    ctx->OMSetRenderTargets(1, &rtv, nullptr);
}
