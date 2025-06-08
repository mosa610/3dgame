#include "BloomExtractPass.h"
#include "Graphics.h"
#include "shader.h"
#include "..//Component/ComponentBloom.h"

void BloomExtractPass::setup(RenderGraphBuilder& builder)
{
    deferred_input = builder.createRenderTarget("DeferredLighting");

    builder.declareRead(deferred_input);

    bloom_extract = builder.createRenderTarget("BloomExtract");

    builder.declareWrite(bloom_extract);

    ID3D11Device* device = Graphics::Instance().Get_device();

    bit_block_transfer = std::make_unique<fullscreen_quad>(device);

    create_ps_from_cso(device, ".//Data//Shader//glow_extraction_ps.cso", glow_extraction_ps.GetAddressOf());

    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(bloom_constants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    HRESULT hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    auto scene = world->getRegister().getEntityByName("Scene");
    if (scene != INVALID_ENTITY)
    {
        world->getRegister().addComponent(scene, ComponentBloom());
    }
}

void BloomExtractPass::execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources)
{
    ID3D11ShaderResourceView* srv = resources.getSRV(deferred_input);
    ID3D11RenderTargetView* bloom_extract_rtv = resources.getRTV(bloom_extract);

    FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
    ctx->ClearRenderTargetView(bloom_extract_rtv, color);
    ctx->OMSetRenderTargets(1, &bloom_extract_rtv, nullptr);

    bloom_constants data{};
    if(!world->getRegister().view<ComponentBloom>().empty())
    {
        auto& bloom = world->getRegister().getComponent<ComponentBloom>(world->getRegister().view<ComponentBloom>().front());
        data.bloom_extraction_threshold = bloom.bloom_extraction_threshold;
        data.bloom_intensity = bloom.bloom_intensity;
    }
    else
    {
        data.bloom_extraction_threshold = 1.f;
        data.bloom_intensity = 1.f;
    }
    ctx->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    ctx->PSSetConstantBuffers(8, 1, constant_buffer.GetAddressOf());

    bit_block_transfer->blit(ctx, &srv, 0, 1, glow_extraction_ps.Get());
}
