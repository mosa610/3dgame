//#include "lambert_shader.h"
//#include "../misc.h"
//#include "texture.h"
//
//LambertShader::LambertShader(ID3D11Device* device)
//{
//
//    //頂点シェーダー
//    {
//        D3D11_INPUT_ELEMENT_DESC input_element_desc[]
//        {
//            {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
//            {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
//            {"TANGENT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
//            {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
//            {"WEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
//            {"BONES",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT},
//        };
//
//        HRESULT hr = create_vs_from_cso(
//            device, "Shader\\lambert_vs.cso",
//            vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
//            input_element_desc, _countof(input_element_desc)
//        );
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//
//    // ピクセルシェーダー
//    {
//        HRESULT hr = create_ps_from_cso(
//            device, "Shader\\lambert_ps.cso",
//            pixel_shader.GetAddressOf()
//        );
//    }
//
//    // 定数バッファ
//    {
//        // シーン用バッファ
//        D3D11_BUFFER_DESC desc;
//        ::memset(&desc, 0, sizeof(desc));
//        desc.Usage = D3D11_USAGE_DEFAULT;
//        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//        desc.CPUAccessFlags = 0;
//        desc.MiscFlags = 0;
//        desc.ByteWidth = sizeof(CbScene);
//        desc.StructureByteStride = 0;
//
//        HRESULT hr = device->CreateBuffer(&desc, 0, scene_constant_buffer.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//        // メッシュ用バッファ
//        desc.ByteWidth = sizeof(CbMesh);
//
//        hr = device->CreateBuffer(&desc, 0, scene_constant_buffer.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//        // サブセット用バッファ
//        desc.ByteWidth = sizeof(CbSubset);
//
//        hr = device->CreateBuffer(&desc, 0, scene_constant_buffer.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//
//    // ブレンドステート
//    {
//        D3D11_BLEND_DESC desc;
//        ::memset(&desc, 0, sizeof(desc));
//        desc.AlphaToCoverageEnable = false;
//        desc.IndependentBlendEnable = false;
//        desc.RenderTarget[0].BlendEnable = true;
//        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//        HRESULT hr = device->CreateBlendState(&desc, blend_state.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//
//    // 深度ステンシルステート
//    {
//        D3D11_DEPTH_STENCIL_DESC desc;
//        ::memset(&desc, 0, sizeof(desc));
//        desc.DepthEnable = true;
//        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//
//        HRESULT hr = device->CreateDepthStencilState(&desc, depth_stencil_state.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//
//    // ラスタライザーステート
//    {
//        D3D11_RASTERIZER_DESC desc;
//        ::memset(&desc, 0, sizeof(desc));
//        desc.FrontCounterClockwise = false;
//        desc.DepthBias = 0;
//        desc.DepthBiasClamp = 0;
//        desc.SlopeScaledDepthBias = 0;
//        desc.DepthClipEnable = true;
//        desc.ScissorEnable = false;
//        desc.MultisampleEnable = true;
//        desc.FillMode = D3D11_FILL_SOLID;
//        desc.CullMode = D3D11_CULL_BACK;
//        desc.AntialiasedLineEnable = false;
//
//        HRESULT hr = device->CreateRasterizerState(&desc, rasterizer_state.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//
//    // サンプラステート
//    {
//        D3D11_SAMPLER_DESC desc;
//        ::memset(&desc, 0, sizeof(desc));
//        desc.MipLODBias = 0.0f;
//        desc.MaxAnisotropy = 1;
//        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//        desc.MinLOD = -FLT_MAX;
//        desc.MaxLOD = FLT_MAX;
//        desc.BorderColor[0] = 1.0f;
//        desc.BorderColor[1] = 1.0f;
//        desc.BorderColor[2] = 1.0f;
//        desc.BorderColor[3] = 1.0f;
//        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//
//        HRESULT hr = device->CreateSamplerState(&desc, sampler_state.GetAddressOf());
//        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//    }
//}
//
//void LambertShader::Begin(ID3D11DeviceContext* dc, const RenderContext& rc)
//{
//    dc->VSSetShader(vertex_shader.Get(), nullptr, 0);
//    dc->PSSetShader(pixel_shader.Get(), nullptr, 0);
//    dc->IASetInputLayout(input_layout.Get());
//
//    ID3D11Buffer* constantBuffers[] =
//    {
//        scene_constant_buffer.Get(),
//        mesh_constant_buffer.Get(),
//        subset_constant_buffer.Get()
//    };
//    dc->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
//    dc->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
//
//    const float blend_factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    dc->OMSetBlendState(blend_state.Get(), blend_factor, 0xFFFFFFFF);
//    dc->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
//    dc->RSSetState(rasterizer_state.Get());
//    dc->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
//
//    // シーン用定数バッファ更新
//    CbScene cbScene;
//
//    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);
//    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);
//    DirectX::XMStoreFloat4x4(&cbScene.viewProjection, V * P);
//
//    cbScene.lightDirection = rc.lightDirection;
//    dc->UpdateSubresource(scene_constant_buffer.Get(), 0, 0, &cbScene, 0, 0);
//}
//
//void LambertShader::Draw(ID3D11DeviceContext* dc, const Model* model)
//{
//    const ModelResource* resource = model->GetResource();
//    const std::vector<Model::Node>& nodes = model->GetNodes();
//
//    model->render();
//}
//
//void LambertShader::End(ID3D11DeviceContext* dc)
//{
//}
