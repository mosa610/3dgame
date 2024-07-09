#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl.h>
#include "Graphics.h"
using namespace Microsoft::WRL;


class sprite_batch
{
private:
    ComPtr<ID3D11VertexShader> vertex_shader;
    ComPtr<ID3D11PixelShader> pixel_shader[1];
    ComPtr<ID3D11InputLayout> input_layout;
    ComPtr<ID3D11Buffer> vertex_buffer;

    ComPtr<ID3D11ShaderResourceView> shader_resource_view;

    ComPtr<ID3D11BlendState>    blend_state;
    ComPtr<ID3D11RasterizerState>   rasterizer_state;
    ComPtr<ID3D11SamplerState>	sampler_state;
    ComPtr<ID3D11DepthStencilState> depth_stencil_state;

    D3D11_TEXTURE2D_DESC texture2d_desc;

    DirectX::XMFLOAT2 texture_size;

public:
    sprite_batch(const wchar_t* filename, size_t max_sprites);
    ~sprite_batch() {};

    void render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,
        float dw, float dh,
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);

    void render(ID3D11DeviceContext* immediate_context,
        float dx, float dy,
        float dw, float dh,
        float r = 1, float g = 1, float b = 1, float a = 1,
        float angle = 0/*degree*/);

    void render(ID3D11DeviceContext* immediate_context,
        DirectX::XMFLOAT2 screen_pos,
        DirectX::XMFLOAT2 screen_WH,
        DirectX::XMFLOAT4 color,
        float angle,
        DirectX::XMFLOAT2 texture_pos,
        DirectX::XMFLOAT2 texture_WH);



    void begin(ID3D11DeviceContext* immediate_context, int pixel);
    void end(ID3D11DeviceContext* immediate_context);

    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    const size_t max_vertices;
    std::vector<vertex> vertices;
};