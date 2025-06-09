#include "RenderGraph.h"

void RenderGraphResources::initialize(ID3D11Device* dev, UINT w, UINT h) {
    device = dev;
    width = w;
    height = h;
    resources.clear();
}

ResourceHandle RenderGraphResources::create(const ResourceCreateInfo& info) {
    Resource res{};
    res.createInfo = info;

    D3D11_TEXTURE2D_DESC desc = {};
    if (info.divide_rate != 0) {
        desc.Width = static_cast<uint32_t>(width) >> info.divide_rate;
        desc.Height = static_cast<uint32_t>(height) >> info.divide_rate;
    }
    else {
        desc.Width = width;
        desc.Height = height;
    }
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = info.format;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;

    switch (info.type) {
    case ResourceType::RenderTarget:
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        break;
    case ResourceType::DepthStencil:
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        break;
    case ResourceType::UnorderedAccess:
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        break;
    }

    HRESULT hr = device->CreateTexture2D(&desc, nullptr, &res.texture);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create texture for resource: " + info.name);
    }

    // ビューを直接作成
    /*HRESULT hr = S_OK;*/

    if (info.type == ResourceType::RenderTarget) {
        hr = device->CreateRenderTargetView(res.texture.Get(), nullptr, &res.rtv);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create RTV for resource: " + info.name);
        }

        hr = device->CreateShaderResourceView(res.texture.Get(), nullptr, &res.srv);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create SRV for resource: " + info.name);
        }
    }
    else if (info.type == ResourceType::DepthStencil) {
        hr = device->CreateDepthStencilView(res.texture.Get(), nullptr, &res.dsv);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create DSV for resource: " + info.name);
        }
    }
    else if (info.type == ResourceType::UnorderedAccess) {
        hr = device->CreateUnorderedAccessView(res.texture.Get(), nullptr, &res.uav);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create UAV for resource: " + info.name);
        }

        hr = device->CreateShaderResourceView(res.texture.Get(), nullptr, &res.srv);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create SRV for resource: " + info.name);
        }
    }

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(desc.Width);
    viewport.Height = static_cast<float>(desc.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    res.viewport = viewport;

    resources.push_back(res);
    return { resources.size() - 1 };
}

ID3D11RenderTargetView* RenderGraphResources::getRTV(ResourceHandle handle) {
    return (handle.exists() && handle.id < resources.size()) ?
        resources[handle.id].rtv.Get() : nullptr;
}

ID3D11ShaderResourceView* RenderGraphResources::getSRV(ResourceHandle handle) {
    return (handle.exists() && handle.id < resources.size()) ?
        resources[handle.id].srv.Get() : nullptr;
}

ID3D11DepthStencilView* RenderGraphResources::getDSV(ResourceHandle handle) {
    return (handle.exists() && handle.id < resources.size()) ?
        resources[handle.id].dsv.Get() : nullptr;
}

ID3D11UnorderedAccessView* RenderGraphResources::getUAV(ResourceHandle handle) {
    return (handle.exists() && handle.id < resources.size()) ?
        resources[handle.id].uav.Get() : nullptr;
}

D3D11_VIEWPORT* RenderGraphResources::getViewport(ResourceHandle handle)
{
    return (handle.exists() && handle.id < resources.size()) ?
        &resources[handle.id].viewport : nullptr;
}

void RenderGraphResources::resize(UINT w, UINT h) {
    width = w;
    height = h;
    // 既存のリソースを再作成
    std::vector<ResourceCreateInfo> infos;
    for (const auto& res : resources) {
        infos.push_back(res.createInfo);
    }

    resources.clear();
    for (const auto& info : infos) {
        create(info);
    }
}

// RenderGraphBuilder実装
void RenderGraphBuilder::clear() {
    reads.clear();
    writes.clear();
    created.clear();
}

ResourceHandle RenderGraphBuilder::createRenderTarget(const std::string& name,
    ResourceType type,
    DXGI_FORMAT format,
    size_t divide_rate) {
    // グローバルマップから既存のリソースを検索
    if (globalNameToHandle && globalNameToHandle->count(name)) {
        ResourceHandle handle = (*globalNameToHandle)[name];
        return handle;
    }

    // 新しいリソースを作成
    ResourceHandle handle{ globalResourceInfos ? globalResourceInfos->size() : 0 };

    if (globalNameToHandle) {
        (*globalNameToHandle)[name] = handle;
    }

    created.push_back(handle);

    ResourceCreateInfo info;
    info.type = type;
    info.format = format;
    info.name = name;
    info.divide_rate = divide_rate;

    if (globalResourceInfos) {
        globalResourceInfos->push_back(info);
    }

    return handle;
}

void RenderGraphBuilder::declareRead(ResourceHandle handle) {
    if (handle.exists()) reads.push_back(handle);
}

void RenderGraphBuilder::declareWrite(ResourceHandle handle) {
    if (handle.exists()) writes.push_back(handle);
}

void RenderGraphBuilder::declareReadWrite(ResourceHandle handle)
{
    declareRead(handle);
    declareWrite(handle);
}

// RenderGraph実装
void RenderGraph::initialize(ID3D11Device* device, UINT w, UINT h) {
    resources.initialize(device, w, h);
}

void RenderGraph::compile() {
    using namespace std;

    // グローバルリソース管理をクリア
    globalNameToHandle.clear();
    globalResourceInfos.clear();

    // まず全パスをセットアップして、グローバルにリソースを管理
    for (size_t i = 0; i < passes.size(); ++i) {
        RenderGraphBuilder builder;
        // グローバルリソース管理への参照を設定
        builder.setGlobalResourceMap(&globalNameToHandle, &globalResourceInfos);

        passes[i]->setup(builder);

        passes[i]->reads = builder.reads;
        passes[i]->writes = builder.writes;
        passes[i]->creates = builder.created;
    }

    // リソースを実際に作成
    resources.clear();
    for (const auto& info : globalResourceInfos) {
        resources.create(info);
    }

    // 依存関係の解析とトポロジカルソート
    unordered_map<ResourceHandle, vector<size_t>> readers, writers;

    for (size_t i = 0; i < passes.size(); ++i) {
        for (auto h : passes[i]->reads) readers[h].push_back(i);
        for (auto h : passes[i]->writes) writers[h].push_back(i);
    }

    unordered_map<size_t, unordered_set<size_t>> edges;
    vector<size_t> indegree(passes.size(), 0);

    for (const auto& [res, writeIndices] : writers) {
        for (size_t writer : writeIndices) {
            if (readers.count(res)) {
                for (size_t reader : readers[res]) {
                    if (reader != writer && edges[writer].insert(reader).second) {
                        indegree[reader]++;
                    }
                }
            }
        }
    }

    queue<size_t> q;
    for (size_t i = 0; i < passes.size(); ++i) {
        if (indegree[i] == 0) q.push(i);
    }

    sortedPasses.clear();
    while (!q.empty()) {
        size_t current = q.front();
        q.pop();
        sortedPasses.push_back(passes[current].get());

        for (size_t next : edges[current]) {
            if (--indegree[next] == 0) {
                q.push(next);
            }
        }
    }

    if (sortedPasses.size() != passes.size()) {
        throw std::runtime_error("Cycle detected in RenderGraph");
    }
}

void RenderGraph::execute(ID3D11DeviceContext* ctx) {
    for (auto* pass : sortedPasses) {
        pass->execute(ctx, resources);
    }
}

void RenderGraph::debug()
{
    for (auto* pass : sortedPasses) {
        pass->debug(resources);
    }
}

void RenderGraph::reset() {
    passes.clear();
    passNames.clear();
    sortedPasses.clear();
    globalNameToHandle.clear();
    globalResourceInfos.clear();
    resources.clear();
}

