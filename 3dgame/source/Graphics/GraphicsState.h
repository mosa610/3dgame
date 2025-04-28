#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>

enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK/*UNIT.32*/, LINEAR_BORDER_WHITE/*UNIT.32*/,ALL, End };
enum class DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF, End };
enum class BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY, End };
enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE, End };

class GraphicsState
{
private:
    GraphicsState();
    ~GraphicsState();

public:
    void Initialize(ID3D11Device* device);

    void DeInitialize();

    void SetBlendState(ID3D11DeviceContext* dc, BLEND_STATE state);

    void SetDepthStencilState(ID3D11DeviceContext* dc, DEPTH_STATE state);

    void SetRasterizerState(ID3D11DeviceContext* dc, RASTER_STATE state);

    void SetSamplerState(ID3D11DeviceContext* dc, SAMPLER_STATE state);

    static GraphicsState& GetInstance()
    {
        static GraphicsState instance;
        return instance;
    }

public:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

    Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];

public:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(SAMPLER_STATE state)
    {
        return sampler_states[static_cast<size_t>(state)];
    }

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState(DEPTH_STATE state)
    {
        return depth_stencil_states[static_cast<size_t>(state)];
    }

    Microsoft::WRL::ComPtr<ID3D11BlendState> GetBlendState(BLEND_STATE state)
    {
        return blend_states[static_cast<size_t>(state)];
    }

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState(RASTER_STATE state)
    {
        return rasterizer_states[static_cast<size_t>(state)];
    }
};