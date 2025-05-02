#include "Graphics.h"
#include "../Framework.h"
#include "GraphicsState.h"

Graphics* Graphics::instance = nullptr;


void acquire_high_performance_adapter(IDXGIFactory6* dxgi_factory6, IDXGIAdapter3** dxgi_adapter3)
{
    HRESULT hr{ S_OK };

    Microsoft::WRL::ComPtr<IDXGIAdapter3> enumerated_adapter;
    for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != dxgi_factory6->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumerated_adapter.ReleaseAndGetAddressOf())); ++adapter_index)
    {
        DXGI_ADAPTER_DESC1 adapter_desc;
        hr = enumerated_adapter->GetDesc1(&adapter_desc);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
        {
            OutputDebugStringW((std::wstring(adapter_desc.Description) + L" has been selected.\n").c_str());
            OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapter_desc.VendorId) + '\n').c_str());
            OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapter_desc.DeviceId) + '\n').c_str());
            OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapter_desc.SubSysId) + '\n').c_str());
            OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapter_desc.Revision) + '\n').c_str());
            OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapter_desc.DedicatedVideoMemory) + '\n').c_str());
            OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapter_desc.DedicatedSystemMemory) + '\n').c_str());
            OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapter_desc.SharedSystemMemory) + '\n').c_str());
            OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapter_desc.AdapterLuid.HighPart) + '\n').c_str());
            OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapter_desc.AdapterLuid.LowPart) + '\n').c_str());
            OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapter_desc.Flags) + '\n').c_str());
            break;
        }
    }
    *dxgi_adapter3 = enumerated_adapter.Detach();
}

Graphics::Graphics(HWND hwnd)
{
    //インスタンス設定
    _ASSERT_EXPR(instance == nullptr, "already instantiated");
    instance = this;

    //画面サイズを取得する
    //RECT rc;
    GetClientRect(hwnd, &rc);
    UINT screen_width = rc.right - rc.left;
    UINT screen_height = rc.bottom - rc.top;

    this->screen_width = static_cast<float>(screen_width);
    this->screen_height = static_cast<float>(screen_height);

    RECT client_rect;
    GetClientRect(hwnd, &client_rect);
    framebuffer_dimensions.cx = client_rect.right - client_rect.left;
    framebuffer_dimensions.cy = client_rect.bottom - client_rect.top;

    HRESULT hr = S_OK;

    //デバイス＆スワップチェーンの生成
    {
        UINT create_device_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        create_device_flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

        UINT create_factory_flags{};
#ifdef _DEBUG
        create_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
        Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
        hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        acquire_high_performance_adapter(dxgi_factory6.Get(), adapter.GetAddressOf());

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
        //DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
        //{
        //    swap_chain_desc.BufferCount = 1;
        //    swap_chain_desc.BufferDesc.Width = screen_width;
        //    swap_chain_desc.BufferDesc.Height = screen_height;
        //    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        //    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
        //    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
        //    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        //    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        //    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        //    swap_chain_desc.OutputWindow = hwnd;
        //    swap_chain_desc.SampleDesc.Count = 1;
        //    swap_chain_desc.SampleDesc.Quality = 0;
        //    swap_chain_desc.Windowed = !FULLSCREEN;		//フルスクリーン
        //    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        //    swap_chain_desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        //}

        hr = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, create_device_flags, feature_levels, 1, D3D11_SDK_VERSION, device.GetAddressOf(), NULL, immediate_context.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        create_swap_chain(dxgi_factory6.Get(), hwnd, tearing_supported, framebuffer_dimensions);

        D3D_FEATURE_LEVEL feature_level;

        //デバイス＆スワップチェーンの生成
        /*hr = D3D11CreateDeviceAndSwapChain(
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
        );*/
        dxgi_factory6.Reset();
    }

    //レンダーターゲットビュー
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;

        hr = swap_chain1->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, &render_target_view);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = device->CreateShaderResourceView(back_buffer.Get(), NULL, &shader_resource_view);

        
        //back_buffer.Get()->Release();
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

    //swap_chain.As(&swap_chain1);
}

Graphics::~Graphics()
{
    GraphicsState::GetInstance().DeInitialize();


    // リソース解放の順序
    immediate_context->ClearState(); // 最初に状態をクリア
    immediate_context->Flush();      // 全てのコマンドをフラッシュ

    // 依存リソースのリセット
    render_target_view.Reset();
    depth_stencil_view.Reset();
    depth_stencil_buffer.Reset();
    shader_resource_view.Reset();

    // スワップチェーンと関連リソース
    swap_chain1->SetFullscreenState(FALSE, nullptr); // フルスクリーン状態を解除
    swap_chain1.Reset();
    swap_chain.Reset();

    immediate_context.Reset(); // デバイスコンテキストを解放

    /*if (device)
    {
        Microsoft::WRL::ComPtr<ID3D11Debug> debugDevice;
        if (SUCCEEDED(device.As(&debugDevice)))
        {
            debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        }
    }*/
    //device.Reset();
}

void Graphics::create_swap_chain(IDXGIFactory6* dxgi_factory6,
    CONST HWND hwnd, BOOL tearing_supported, SIZE framebuffer_dimensions)
{
    HRESULT hr{ S_OK };

    if (swap_chain1)
    {
        ID3D11RenderTargetView* null_render_target_view{};
        immediate_context->OMSetRenderTargets(1, &null_render_target_view, NULL);
        render_target_view.Reset();
        immediate_context->OMSetDepthStencilState(nullptr, 0);
        depth_stencil_view.Reset();
#if 0
        immediate_context->Flush();
        immediate_context->ClearState();
#endif
        DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
        swap_chain1->GetDesc(&swap_chain_desc);
        hr = swap_chain1->ResizeBuffers(swap_chain_desc.BufferCount, framebuffer_dimensions.cx, framebuffer_dimensions.cy, swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
        hr = swap_chain1->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        D3D11_TEXTURE2D_DESC texture2d_desc;
        render_target_buffer->GetDesc(&texture2d_desc);

        hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, render_target_view.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        render_target_buffer.Reset();
        null_render_target_view = nullptr;
    }
    else
    {
        BOOL allow_tearing = FALSE;
        if (SUCCEEDED(hr))
        {
            hr = dxgi_factory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
        }
        tearing_supported = SUCCEEDED(hr) && allow_tearing;

        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
        swap_chain_desc1.Width = framebuffer_dimensions.cx;
        swap_chain_desc1.Height = framebuffer_dimensions.cy;
        swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        swap_chain_desc1.Stereo = FALSE;
        swap_chain_desc1.SampleDesc.Count = 1;
        swap_chain_desc1.SampleDesc.Quality = 0;
        swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc1.BufferCount = 2;
        swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
        swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swap_chain_desc1.Flags = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
        hr = dxgi_factory6->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc1, NULL, NULL, swap_chain1.ReleaseAndGetAddressOf());
#if 0
        swap_chain_desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        hr = dxgi_factory6->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
        hr = swap_chain1->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
        hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, render_target_view.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    if (depth_stencil_view)
    {
        depth_stencil_view.Reset();
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        texture2d_desc.Width = framebuffer_dimensions.cx;
        texture2d_desc.Height = framebuffer_dimensions.cy;
        texture2d_desc.MipLevels = 1;
        texture2d_desc.ArraySize = 1;
        texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        texture2d_desc.SampleDesc.Count = 1;
        texture2d_desc.SampleDesc.Quality = 0;
        texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
        texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texture2d_desc.CPUAccessFlags = 0;
        texture2d_desc.MiscFlags = 0;
        hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
        depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_stencil_view_desc.Texture2D.MipSlice = 0;
        ID3D11DepthStencilView* dsv = nullptr;
        hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, &dsv);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        depth_stencil_view.Attach(dsv);
    }
    else
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        texture2d_desc.Width = framebuffer_dimensions.cx;
        texture2d_desc.Height = framebuffer_dimensions.cy;
        texture2d_desc.MipLevels = 1;
        texture2d_desc.ArraySize = 1;
        texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        texture2d_desc.SampleDesc.Count = 1;
        texture2d_desc.SampleDesc.Quality = 0;
        texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
        texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        texture2d_desc.CPUAccessFlags = 0;
        texture2d_desc.MiscFlags = 0;
        hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
        depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depth_stencil_view_desc.Texture2D.MipSlice = 0;
        hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(framebuffer_dimensions.cx);
    viewport.Height = static_cast<float>(framebuffer_dimensions.cy);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    immediate_context->RSSetViewports(1, &viewport);
}

void Graphics::DebugDraw() const
{
    Microsoft::WRL::ComPtr<ID3D11Debug> debugDevice;
    device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debugDevice.GetAddressOf()));
    debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}
