#include "Graphics.h"
#include "../Framework.h"

Graphics* Graphics::instance = nullptr;

Graphics::Graphics(HWND hwnd)
{
    //インスタンス設定
    _ASSERT_EXPR(instance == nullptr, "already instantiated");
    instance = this;

    //画面サイズを取得する
    RECT rc;
    GetClientRect(hwnd, &rc);
    UINT screen_width = rc.right - rc.left;
    UINT screen_height = rc.bottom - rc.top;

    this->screen_width = static_cast<float>(screen_width);
    this->screen_height = static_cast<float>(screen_height);

    HRESULT hr = S_OK;

    //デバイス＆スワップチェーンの生成
    {
        UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL feature_levels[] =
        {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        //　スワップチェーンを生成するための設定オプション
        DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
        {
            swap_chain_desc.BufferCount = 1;
            swap_chain_desc.BufferDesc.Width = screen_width;
            swap_chain_desc.BufferDesc.Height = screen_height;
            swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
            swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
            swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
            swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

            swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swap_chain_desc.OutputWindow = hwnd;
            swap_chain_desc.SampleDesc.Count = 1;
            swap_chain_desc.SampleDesc.Quality = 0;
            swap_chain_desc.Windowed = !FULLSCREEN;		//フルスクリーン
            swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            swap_chain_desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        }

        D3D_FEATURE_LEVEL feature_level;

        //デバイス＆スワップチェーンの生成
        hr = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            create_device_flags,
            feature_levels,
            ARRAYSIZE(feature_levels),
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            swap_chain.GetAddressOf(),
            device.GetAddressOf(),
            &feature_level,
            immediate_context.GetAddressOf()
        );
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //レンダーターゲットビュー
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;

        hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, &render_target_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //深度ステンシルビューの生成
    {
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        texture2d_desc.Width = SCREEN_WIDTH;
        texture2d_desc.Height = SCREEN_HEIGHT;
        texture2d_desc.MipLevels = 1;
        texture2d_desc.ArraySize = 1;
        texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        texture2d_desc.SampleDesc.Count = 1;
        texture2d_desc.SampleDesc.Quality = 0;
        texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
        texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texture2d_desc.CPUAccessFlags = 0;
        texture2d_desc.MiscFlags = 0;
        hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};

        depth_stencil_view_desc.Format = texture2d_desc.Format;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_stencil_view_desc.Texture2D.MipSlice = 0;
        hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, &depth_stencil_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    //ビューポートの設定
    {
        D3D11_VIEWPORT viewport{};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(SCREEN_WIDTH);
        viewport.Height = static_cast<float>(SCREEN_HEIGHT);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        immediate_context->RSSetViewports(1, &viewport);
    }

    //レンダラ
    {
        imgui_renderer = std::make_unique<ImGuiRenderer>(hwnd, device.Get(),immediate_context.Get());
    }
}

Graphics::~Graphics()
{
}
