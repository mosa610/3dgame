#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <unordered_map>
#include <memory>
#include "GraphicsState.h"

using Microsoft::WRL::ComPtr;

struct PipelineStateDesc {
    std::string name;
    SAMPLER_STATE sampler = SAMPLER_STATE::ALL;
    DEPTH_STATE depth = DEPTH_STATE::ZT_ON_ZW_ON;
    BLEND_STATE blend = BLEND_STATE::NONE;
    RASTER_STATE raster = RASTER_STATE::CULL_NONE;

    std::string vs_path;
    std::string hs_path;
    std::string ds_path;
    std::string gs_path;
    std::string ps_path;
    std::string cs_path;

    D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_desc;
};

class PipelineState {
public:
    bool Initialize(ID3D11Device* device, const PipelineStateDesc& desc);

    void Bind(ID3D11DeviceContext* context) const;

private:
    PipelineStateDesc desc_;
    ComPtr<ID3D11InputLayout> input_layout_;
    ComPtr<ID3D11VertexShader> vs_;
    ComPtr<ID3D11HullShader> hs_;
    ComPtr<ID3D11DomainShader> ds_;
    ComPtr<ID3D11GeometryShader> gs_;
    ComPtr<ID3D11PixelShader> ps_;
    ComPtr<ID3D11ComputeShader> cs_;
};

class PipelineManager {
public:
    static PipelineManager& Instance();

    bool Add(const PipelineStateDesc& desc, ID3D11Device* device);
    PipelineState* GetByName(const std::string& name);
    bool BindByName(const std::string& name, ID3D11DeviceContext* context);

private:
    std::unordered_map<std::string, std::unique_ptr<PipelineState>> pipeline_map_;
};
