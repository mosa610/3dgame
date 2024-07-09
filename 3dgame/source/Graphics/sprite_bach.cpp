#include "sprite_bach.h"
#include "shader.h"
#include "texture.h"


inline auto rotate(float& x, float& y, float cx, float cy, float angle)
{
    x -= cx;
    y -= cy;

    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
    float tx{ x }, ty{ y };
    x = cos * tx + (-sin) * ty;
    y = sin * tx + cos * ty;

    x += cx;
    y += cy;
};


sprite_batch::sprite_batch(
    const wchar_t* filename, size_t max_sprites)
    : max_vertices(max_sprites * 6)
{
    ID3D11Device* device = Graphics::Instance().Get_device();

    HRESULT hr{ S_OK };

    //頂点バッファオブジェクトの生成
    {
        D3D11_BUFFER_DESC buffer_desc{};
        buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;
        buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&buffer_desc, NULL, vertex_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //頂点シェーダー
    {
        D3D11_INPUT_ELEMENT_DESC input_element_desc[]
        {
            {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
            D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
            {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
            D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
            {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
            D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        };

        hr = create_vs_from_cso(
            device, "Shader\\Sprite_vs.cso",
            vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
            input_element_desc, _countof(input_element_desc)
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //ピクセルシェーダー
    {
        hr = create_ps_from_cso(
            device, "Shader\\Sprite_ps.cso",
            pixel_shader[0].GetAddressOf()
        );
    }

    //ブレンドステート
    {
        D3D11_BLEND_DESC blend_desc{};
        ::memset(&blend_desc, 0, sizeof(blend_desc));
        blend_desc.AlphaToCoverageEnable = FALSE;
        blend_desc.IndependentBlendEnable = FALSE;
        blend_desc.RenderTarget[0].BlendEnable = TRUE;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        hr = device->CreateBlendState(&blend_desc, &blend_state);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //深度ステンシルステート
    {
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
        memset(&depth_stencil_desc, 0, sizeof(depth_stencil_desc));
        depth_stencil_desc.DepthEnable = false;	//深度テストのon,off
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	//深度ステンシルバッファへの書き込みのon,off
        //depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        hr = device->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //ラスタライザーステート
    {
        D3D11_RASTERIZER_DESC rasterizer_desc{};
        ::memset(&rasterizer_desc, 0, sizeof(rasterizer_desc));
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        rasterizer_desc.FrontCounterClockwise = TRUE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0;
        rasterizer_desc.SlopeScaledDepthBias = 0;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.AntialiasedLineEnable = FALSE;

        hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //サンプラーステート
    {
        D3D11_SAMPLER_DESC sampler_desc;
        ::memset(&sampler_desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MipLODBias = 0;
        sampler_desc.MaxAnisotropy = 16;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.BorderColor[0] = 1.0f;
        sampler_desc.BorderColor[1] = 1.0f;
        sampler_desc.BorderColor[2] = 1.0f;
        sampler_desc.BorderColor[3] = 1.0f;
        sampler_desc.MinLOD = 0;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&sampler_desc, &sampler_state);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //テクスチャの生成
    {
        hr = load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(),
            &texture2d_desc);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        texture_size = {
            (float)texture2d_desc.Width,
            (float)texture2d_desc.Height
        };
    }
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context,
    float dx, float dy, float dw, float dh,
    float r, float g, float b, float a, float angle,
    float sx, float sy, float sw, float sh)
{
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    //left-top
    float x0{ dx };
    float y0{ dy };
    //right-top
    float x1{ dx + dw };
    float y1{ dy };
    //left-bottom
    float x2{ dx };
    float y2{ dy + dh };
    //right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };

    //回転の中心を矩形の中心にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;

    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    //スクリーン座標系からNDCへの座標返還を行う
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    float u0{ sx / texture2d_desc.Width };
    float v0{ sy / texture2d_desc.Height };
    float u1{ (sx + sw) / texture2d_desc.Width };
    float v1{ (sy + sh) / texture2d_desc.Height };

    vertices.push_back({ {x0,y0,0},{r,g,b,a},{u0,v0} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x3,y3,0},{r,g,b,a},{u1,v1} });
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
{
    render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle,
        0, 0, texture2d_desc.Width, texture2d_desc.Height);
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context,
    DirectX::XMFLOAT2 screen_pos, DirectX::XMFLOAT2 screen_WH,
    DirectX::XMFLOAT4 color,
    float angle,
    DirectX::XMFLOAT2 texture_pos,
    DirectX::XMFLOAT2 texture_WH)
{
    render(immediate_context,
        screen_pos.x, screen_pos.y,
        screen_WH.x, screen_WH.y,
        color.x, color.y, color.z, color.w,
        angle,
        texture_pos.x, texture_pos.y,
        texture_WH.x, texture_WH.y);
}

void sprite_batch::begin(ID3D11DeviceContext* immediate_context, int pixel)
{
    vertices.clear();
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader[pixel].Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
}

void sprite_batch::end(ID3D11DeviceContext* immediate_context)
{
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    size_t vertex_count = vertices.size();
    _ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
    vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (data != nullptr)
    {
        const vertex* p = vertices.data();
        memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
    }
    immediate_context->Unmap(vertex_buffer.Get(), 0);

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
    immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
    immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
    immediate_context->RSSetState(rasterizer_state.Get());


    immediate_context->Draw(static_cast<UINT>(vertex_count), 0);
}
