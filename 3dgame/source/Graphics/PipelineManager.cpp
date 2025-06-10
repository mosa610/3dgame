#include "PipelineManager.h"
#include "shader.h"

inline std::vector<D3D11_INPUT_ELEMENT_DESC> GetDefaultInputLayout() {
    return {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "JOINTS",   0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
}

bool PipelineState::Initialize(ID3D11Device* device, const PipelineStateDesc& desc) {
    desc_ = desc;

    /*const D3D11_INPUT_ELEMENT_DESC* layout = desc.input_layout_desc.empty()
        ? GetDefaultInputLayout().data()
        : desc.input_layout_desc.data();

    UINT layout_count = static_cast<UINT>(desc.input_layout_desc.empty()
        ? GetDefaultInputLayout().size()
        : desc.input_layout_desc.size());*/

    std::vector<D3D11_INPUT_ELEMENT_DESC> default_layout;

    const D3D11_INPUT_ELEMENT_DESC* layout = nullptr;
    UINT layout_count = 0;

    if (desc.input_layout_desc.empty()) {
        default_layout = GetDefaultInputLayout();  // 一時オブジェクトを保持
        layout = default_layout.data();
        layout_count = static_cast<UINT>(default_layout.size());
    }
    else {
        layout = desc.input_layout_desc.data();
        layout_count = static_cast<UINT>(desc.input_layout_desc.size());
    }

    if (!desc.vs_path.empty())
        create_vs_from_cso(device, desc.vs_path.c_str(), vs_.GetAddressOf(), input_layout_.GetAddressOf(), layout, layout_count);
    if (!desc.hs_path.empty())
        create_hs_from_cso(device, desc.hs_path.c_str(), hs_.GetAddressOf());
    if (!desc.ds_path.empty())
        create_ds_from_cso(device, desc.ds_path.c_str(), ds_.GetAddressOf());
    if (!desc.gs_path.empty())
        create_gs_from_cso(device, desc.gs_path.c_str(), gs_.GetAddressOf());
    if (!desc.ps_path.empty())
        create_ps_from_cso(device, desc.ps_path.c_str(), ps_.GetAddressOf());
    if (!desc.cs_path.empty())
        create_cs_from_cso(device, desc.cs_path.c_str(), cs_.GetAddressOf());

    return true;
}

void PipelineState::Bind(ID3D11DeviceContext* context) const {
    GraphicsState& gs = GraphicsState::GetInstance();
    gs.SetBlendState(context, desc_.blend);
    gs.SetDepthStencilState(context, desc_.depth);
    gs.SetRasterizerState(context, desc_.raster);
    gs.SetSamplerState(context, desc_.sampler);

    context->IASetPrimitiveTopology(desc_.topology);
    if (input_layout_) context->IASetInputLayout(input_layout_.Get());
    if (vs_) context->VSSetShader(vs_.Get(), nullptr, 0);
    if (hs_) context->HSSetShader(hs_.Get(), nullptr, 0);
    if (ds_) context->DSSetShader(ds_.Get(), nullptr, 0);
    if (gs_) context->GSSetShader(gs_.Get(), nullptr, 0);
    if (ps_) context->PSSetShader(ps_.Get(), nullptr, 0);
    if (cs_) context->CSSetShader(cs_.Get(), nullptr, 0);
}

PipelineManager& PipelineManager::Instance() {
    static PipelineManager instance;
    return instance;
}

bool PipelineManager::Add(const PipelineStateDesc& desc, ID3D11Device* device) {
    if (pipeline_map_.count(desc.name)) return false;

    auto pipeline = std::make_unique<PipelineState>();
    if (!pipeline->Initialize(device, desc)) return false;

    pipeline_map_[desc.name] = std::move(pipeline);
    return true;
}

PipelineState* PipelineManager::GetByName(const std::string& name) {
    auto it = pipeline_map_.find(name);
    return (it != pipeline_map_.end()) ? it->second.get() : nullptr;
}

bool PipelineManager::BindByName(const std::string& name, ID3D11DeviceContext* context) {
    auto* pipeline = GetByName(name);
    if (!pipeline) return false;
    pipeline->Bind(context);
    return true;
}
