#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include <memory>
#include <queue>
#include <cassert>
#include <stdexcept>
#include <d3d11.h>
#include <wrl/client.h>

enum class ResourceType {
    RenderTarget,
    DepthStencil,
    UnorderedAccess
};

struct ResourceHandle {
    size_t id;
    bool exists() const { return id != static_cast<size_t>(-1); }
    static ResourceHandle Invalid() { return { static_cast<size_t>(-1) }; }
    bool operator==(const ResourceHandle& other) const { return id == other.id; }
};

// リソース作成情報を保持する構造体
struct ResourceCreateInfo {
    ResourceType type = ResourceType::RenderTarget;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    std::string name;
    size_t divide_rate = 0;
};

namespace std {
    template<>
    struct hash<ResourceHandle> {
        size_t operator()(const ResourceHandle& h) const noexcept {
            return hash<size_t>()(h.id);
        }
    };
}

class RenderGraphResources {
public:
    struct Resource {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
        ResourceCreateInfo createInfo;
    };

    void initialize(ID3D11Device* dev, UINT w, UINT h);
    ResourceHandle create(const ResourceCreateInfo& info);

    ID3D11RenderTargetView* getRTV(ResourceHandle handle);
    ID3D11ShaderResourceView* getSRV(ResourceHandle handle);
    ID3D11DepthStencilView* getDSV(ResourceHandle handle);
    ID3D11UnorderedAccessView* getUAV(ResourceHandle handle);

    void resize(UINT w, UINT h);
    void clear() { resources.clear(); }

private:
    std::vector<Resource> resources;
    ID3D11Device* device = nullptr;
    UINT width = 0;
    UINT height = 0;
};

class RenderGraphBuilder {
public:
    void clear();

    ResourceHandle createRenderTarget(const std::string& name,
        ResourceType type = ResourceType::RenderTarget,
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
        size_t divide_rate = 0
    );

    void declareRead(ResourceHandle handle);
    void declareWrite(ResourceHandle handle);

    // RenderGraphからグローバルリソース管理への参照を設定
    void setGlobalResourceMap(std::unordered_map<std::string, ResourceHandle>* globalMap,
        std::vector<ResourceCreateInfo>* globalInfos) {
        globalNameToHandle = globalMap;
        globalResourceInfos = globalInfos;
    }

private:
    friend class RenderGraph;

    // グローバルリソース管理への参照
    std::unordered_map<std::string, ResourceHandle>* globalNameToHandle = nullptr;
    std::vector<ResourceCreateInfo>* globalResourceInfos = nullptr;

    std::vector<ResourceHandle> reads;
    std::vector<ResourceHandle> writes;
    std::vector<ResourceHandle> created;
};

class RenderPass {
public:
    std::vector<ResourceHandle> reads;
    std::vector<ResourceHandle> writes;
    std::vector<ResourceHandle> creates;

    virtual void setup(RenderGraphBuilder& builder) = 0;
    virtual void execute(ID3D11DeviceContext* ctx, RenderGraphResources& resources) = 0;

    virtual void debug(RenderGraphResources& resources) {}
    virtual ~RenderPass() = default;
};

class RenderGraph {
public:
    void initialize(ID3D11Device* device, UINT width, UINT height);

    template<typename T, typename... Args>
    void addPass(const std::string& name, Args&&... args) {
        auto pass = std::make_unique<T>(std::forward<Args>(args)...);
        passNames.push_back(name);
        passes.emplace_back(std::move(pass));
    }

    void compile();
    void execute(ID3D11DeviceContext* ctx);
    void reset();  // パスをクリアして再利用可能にする

    void debug();

private:
    std::vector<std::unique_ptr<RenderPass>> passes;
    std::vector<std::string> passNames;
    std::vector<RenderPass*> sortedPasses;
    RenderGraphResources resources;

    // グローバルなリソース名管理
    std::unordered_map<std::string, ResourceHandle> globalNameToHandle;
    std::vector<ResourceCreateInfo> globalResourceInfos;
};