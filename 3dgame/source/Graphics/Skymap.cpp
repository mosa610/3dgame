#include "Skymap.h"
#include "..//misc.h"
#include "texture.h"
#include "Shader.h"
#include "Graphics.h"
#include "..//Camera.h"

Skymap::Skymap(ID3D11Device *device)
{
    HRESULT hr = S_OK;
    // サンプラステートの生成
    {
        D3D11_SAMPLER_DESC sampler_desc{};
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        /*sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;*/
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MipLODBias = 0;
        sampler_desc.MaxAnisotropy = 16;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.BorderColor[0] = 0;
        sampler_desc.BorderColor[1] = 0;
        sampler_desc.BorderColor[2] = 0;
        sampler_desc.BorderColor[3] = 0;
        sampler_desc.MinLOD = 0;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
    }
    // 深度ステンシルステートの生成
    {
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
        depth_stencil_desc.DepthEnable = TRUE;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        hr = device->CreateDepthStencilState(&depth_stencil_desc, skymap_depth_stencil_state.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    skymap_depth_stencil_state.Reset();

    // 定数バッファの生成
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(scene_constants);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        hr = device->CreateBuffer(&desc, 0, scene_constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        desc.ByteWidth = sizeof(skymap_constants);
        hr = device->CreateBuffer(&desc, 0, skymap_constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // スカイマップ用に深度値を書き込まない深度ステンシルステートの生成
    {
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
        depth_stencil_desc.DepthEnable = TRUE;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        hr = device->CreateDepthStencilState(&depth_stencil_desc, skymap_depth_stencil_state.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // スカイマップ用のテクスチャ及びスプライトを準備
    load_texture_from_file(device, L".\\Data\\Skymap\\specular_pmrem.dds",
        skymap_shader_resource_view.ReleaseAndGetAddressOf(), &skymap_texture2d_desc);
    skymap_sprite = std::make_unique<Sprite>(device, skymap_shader_resource_view);

    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    create_vs_from_cso(device, "Data\\Shader\\sky_box_vs.cso", skymap_vertex_shader.GetAddressOf(), skymap_input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, "Data\\Shader\\sky_box_gbuffer_ps.cso", skymap_pixel_shader.GetAddressOf());

}

Skymap::~Skymap()
{
    skymap_constant_buffer.Reset();
    scene_constant_buffer.Reset();
    skymap_vertex_shader.Reset();
    skymap_pixel_shader.Reset();
    skymap_input_layout.Reset();
    skymap_depth_stencil_state.Reset();
    skymap_shader_resource_view.Reset();
    sampler_state.Reset();
}

void Skymap::update()
{

}

void Skymap::Render(ID3D11DeviceContext* immediate_context, const RenderContext& rc)
{
    // 定数バッファの更新
    {
        scene_constants scene_constant;
        DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
        DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
        DirectX::XMStoreFloat4x4(&scene_constant.view_projection, V * P);
        DirectX::XMStoreFloat4x4(&scene_constant.inverse_view_projection, DirectX::XMMatrixInverse(nullptr, V * P));
        Camera& main_camera = Camera::Instance();
        scene_constant.camera_position = { main_camera.GetEye().x,main_camera.GetEye().y,main_camera.GetEye().z,0 };


        /*immediate_context->UpdateSubresource(scene_constant_buffer.Get(), 0, 0, &scene_constant, 0, 0);
        immediate_context->VSSetConstantBuffers(1, 1, scene_constant_buffer.GetAddressOf());
        immediate_context->PSSetConstantBuffers(1, 1, scene_constant_buffer.GetAddressOf());*/

        skymap_constants skymap{};
        DirectX::XMStoreFloat4x4(&skymap.inverse_view_projection, DirectX::XMMatrixInverse(nullptr, V * P));
        immediate_context->UpdateSubresource(skymap_constant_buffer.Get(), 0, 0, &skymap, 0, 0);
        immediate_context->VSSetConstantBuffers(7, 1, skymap_constant_buffer.GetAddressOf());
        immediate_context->PSSetConstantBuffers(7, 1, skymap_constant_buffer.GetAddressOf());
    }

    float screenWidth = Graphics::Instance().Get_screen_width();
    float screenHeight = Graphics::Instance().Get_screen_height();

    //	空描画
    if (skymap_sprite)
    {
        immediate_context->IASetInputLayout(skymap_input_layout.Get());
        immediate_context->VSSetShader(skymap_vertex_shader.Get(), nullptr, 0);
        immediate_context->PSSetShader(skymap_pixel_shader.Get(), nullptr, 0);
        immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
        immediate_context->OMSetDepthStencilState(skymap_depth_stencil_state.Get(), 0);

        skymap_sprite->render(immediate_context, 0, 0, screenWidth, screenHeight);

        //immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
    }

    immediate_context->VSSetShader(nullptr, nullptr, 0);
    immediate_context->GSSetShader(nullptr, nullptr, 0);
    immediate_context->PSSetShader(nullptr, nullptr, 0);
    immediate_context->IASetInputLayout(nullptr);
}
