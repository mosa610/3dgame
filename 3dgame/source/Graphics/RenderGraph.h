#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include <cassert>
#include <d3d11.h>
#include <wrl/client.h>

struct ResourceHandle {
    size_t id;
    bool is_valid() const { return id != (size_t)-1; }
    static ResourceHandle Invalid() { return { (size_t)-1 }; }
};

class RenderGraphBuilder {
public:
    ResourceHandle createRenderTarget(const std::string& name);
    void declareRead(ResourceHandle handle);
    void declareWrite(ResourceHandle handle);

    std::unordered_map<std::string, ResourceHandle> name_to_handle;
    std::vector<ResourceHandle> reads;
    std::vector<ResourceHandle> writes;
    std::vector<ResourceHandle> created;
};

class RenderGraphResources {
public:
    ID3D11RenderTargetView* getRTV(ResourceHandle handle);
    ID3D11ShaderResourceView* getSRV(ResourceHandle handle);
    void resize(UINT width, UINT height);

    struct Resource {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    };

    std::vector<Resource> resources;
    ID3D11Device* device = nullptr;
    UINT width = 0;
    UINT height = 0;

    void initialize(ID3D11Device* dev, UINT w, UINT h) {
        device = dev;
        width = w;
        height = h;
    }

    ResourceHandle create();
};

class RenderPass {
public:
    virtual void setup(RenderGraphBuilder& builder) = 0;
    virtual void execute(ID3D11DeviceContext* ctx, RenderGraphResources& res) = 0;
    virtual ~RenderPass() {}
};

class RenderGraph {
public:
    template<typename T, typename... Args>
    void addPass(const std::string& name, Args&&... args) {
        auto pass = std::make_unique<T>(std::forward<Args>(args)...);
        passes.emplace_back(std::move(pass));
    }

    void compile() {
        // 今は単純な実装: 各パスの setup を順番に呼ぶ
        for (auto& pass : passes) {
            RenderGraphBuilder builder;
            pass->setup(builder);
            // リソース登録など省略可能
        }
    }

    void execute(ID3D11DeviceContext* ctx) {
        for (auto& pass : passes) {
            pass->execute(ctx, resources);
        }
    }

    void initialize(ID3D11Device* device, UINT width, UINT height) {
        resources.initialize(device, width, height);
    }

private:
    std::vector<std::unique_ptr<RenderPass>> passes;
    RenderGraphResources resources;
};