#include "RenderGraph.h"

ResourceHandle RenderGraphBuilder::createRenderTarget(const std::string& name) {
    ResourceHandle handle{ name_to_handle.size() };
    name_to_handle[name] = handle;
    created.push_back(handle);
    return handle;
}

void RenderGraphBuilder::declareRead(ResourceHandle handle) {
    reads.push_back(handle);
}

void RenderGraphBuilder::declareWrite(ResourceHandle handle) {
    writes.push_back(handle);
}

ResourceHandle RenderGraphResources::create() {
    Resource res;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    device->CreateTexture2D(&texDesc, nullptr, &texture);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
    device->CreateRenderTargetView(texture.Get(), nullptr, &rtv);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    device->CreateShaderResourceView(texture.Get(), nullptr, &srv);

    res.texture = texture;
    res.rtv = rtv;
    res.srv = srv;

    resources.push_back(std::move(res));
    return ResourceHandle{ resources.size() - 1 };
}

ID3D11RenderTargetView* RenderGraphResources::getRTV(ResourceHandle handle) {
    assert(handle.id < resources.size());
    return resources[handle.id].rtv.Get();
}

ID3D11ShaderResourceView* RenderGraphResources::getSRV(ResourceHandle handle) {
    assert(handle.id < resources.size());
    return resources[handle.id].srv.Get();
}

void RenderGraphResources::resize(UINT w, UINT h) {
    width = w;
    height = h;
    // 実リソースの再作成は省略（通常は全リソース再構築）
}