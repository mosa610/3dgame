#include "Framework.h"
#include "Graphics/GraphicsState.h"
#include "SceneManager.h"
#include "SceneTest.h"
#include "Mouse.h"


Framework::Framework(HWND hwnd, BOOL fullscreen)
    :hwnd(hwnd)
	, graphics(hwnd)
	, fullscreen_mode(fullscreen)
	, windowed_style(static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)))
{
	SceneManager::Instance().ChangeScene(new SceneTest);
	GraphicsState::GetInstance().Initialize(graphics.Get_device());
	if (fullscreen)
	{
		on_size_changed(static_cast<UINT64>(GetSystemMetrics(SM_CXSCREEN)), GetSystemMetrics(SM_CYSCREEN));
		stylize_window(TRUE);
	}
}

Framework::~Framework()
{
	stylize_window(FALSE);
	SceneManager::Instance().Clear();
	swap_chain.Reset();
	d2d1_device_context.Reset();
	for (int i = 0; i < 8; ++i)
	{
		dwrite_text_formats[i].Reset();
		d2d_solid_color_brushes[i].Reset();
	}
	adapter.Reset();

}

void Framework::Update(float elapsedTime)
{
	Mouse::GetInstance().Update(hwnd);

	SceneManager::Instance().Update(elapsedTime);

#if 1
	if (GetAsyncKeyState(VK_RETURN) & 1 && GetAsyncKeyState(VK_MENU) & 1)
	{
		stylize_window(!fullscreen_mode);
	}
#endif

#ifdef USE_IMGUI
	graphics.Get_ImGui_renderer()->NewFrame();
#endif


#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
	if (ImGui::TreeNode("test01")) {
		ImGui::Text("framed.x : %d, framed.y : %d",framebuffer_dimensions.cx, framebuffer_dimensions.cy);
		ImGui::Text("Screen.x : %f, Screen.y : %f", graphics.Get_screen_width(), graphics.Get_screen_height());
		ImGui::TreePop();
	}
	ImGui::End();

	SceneManager::Instance().DrawGUI();
#endif
}

void Framework::Render(float elapsedTime)
{
	HRESULT hr{ S_OK };
	elapsed_time = elapsedTime;

	SceneManager::Instance().Render(elapsedTime);

	ID3D11RenderTargetView* rtv = graphics.Get_render_target_view();
	ID3D11DepthStencilView* dsv = graphics.Get_depth_stencil_view();
	graphics.Get_device_context()->OMSetRenderTargets(1, &rtv, dsv);

	graphics.Get_ImGui_renderer()->Render(graphics.Get_device_context());

	//UINT sync_interval{ 0 };
	//graphics.Get_swap_chain()->Present(sync_interval, 0);
	UINT sync_interval{ vsync ? 1U : 0U };
	UINT flags = (tearing_supported && !fullscreen_mode && !vsync) ? DXGI_PRESENT_ALLOW_TEARING : 0;
	hr = graphics.Get_swap_chain1()->Present(sync_interval, flags);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void Framework::calculate_frame_stats()
{
	static uint32_t frames = 0;
	static float elapsed_time = 0.0f;

	if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
		SetWindowTextW(hwnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

void Framework::stylize_window(BOOL fullscreen)
{
	//IDXGISwapChain1* swap_chain = graphics.Get_swap_chain1();
	fullscreen_mode = fullscreen;
	if (fullscreen)
	{
		GetWindowRect(hwnd, &windowed_rect);
		SetWindowLongPtrA(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

		RECT fullscreen_window_rect;

		HRESULT hr{ E_FAIL };
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain = graphics.Get_swap_chain1();
		if (swap_chain)
		{
			Microsoft::WRL::ComPtr<IDXGIOutput> dxgi_output;
			hr = swap_chain->GetContainingOutput(&dxgi_output);
			if (hr == S_OK)
			{
				DXGI_OUTPUT_DESC output_desc;
				hr = dxgi_output->GetDesc(&output_desc);
				if (hr == S_OK)
				{
					fullscreen_window_rect = output_desc.DesktopCoordinates;
				}
			}
			dxgi_output.Reset();
		}
		if (hr != S_OK)
		{
			DEVMODE devmode = {};
			devmode.dmSize = sizeof(DEVMODE);
			EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

			fullscreen_window_rect = {
				devmode.dmPosition.x,
				devmode.dmPosition.y,
				devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
				devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight)
			};
		}
		SetWindowPos(
			hwnd,
			NULL,
			fullscreen_window_rect.left,
			fullscreen_window_rect.top,
			fullscreen_window_rect.right,
			fullscreen_window_rect.bottom,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd, SW_MAXIMIZE);
	}
	else
	{
		SetWindowLongPtrA(hwnd, GWL_STYLE, windowed_style);
		SetWindowPos(
			hwnd,
			HWND_NOTOPMOST,
			windowed_rect.left,
			windowed_rect.top,
			windowed_rect.right - windowed_rect.left,
			windowed_rect.bottom - windowed_rect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		ShowWindow(hwnd, SW_NORMAL);
	}
}

void Framework::on_size_changed(UINT64 width, UINT height)
{
	HRESULT hr{ S_OK };
	if (width != framebuffer_dimensions.cx || height != framebuffer_dimensions.cy)
	{
		framebuffer_dimensions.cx = static_cast<LONG>(width);
		framebuffer_dimensions.cy = height;

		// Release all objects that hold shader resource views here.
		SceneManager::Instance().ResetShaderResource();
#ifdef ENABLE_DIRECT2D
		d2d1_device_context.Reset();
#endif

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain = graphics.Get_swap_chain1();
		hr = swap_chain->GetParent(IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		graphics.create_swap_chain(dxgi_factory6.Get(), hwnd, tearing_supported, framebuffer_dimensions);
		//swap_chain = graphics.Get_swap_chain1();
		dxgi_factory6.Reset();

		// Recreate all objects that hold shader resource views here.
		SceneManager::Instance().RemakeShaderResource(framebuffer_dimensions.cx, framebuffer_dimensions.cy);
#ifdef ENABLE_DIRECT2D
		//create_direct2d_objects();
#endif
	}
}

#ifdef ENABLE_DIRECT2D
void Framework::create_direct2d_objects()
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr <ID3D11Device> device = graphics.Get_device();

	Microsoft::WRL::ComPtr<IDXGIDevice2> dxgi_device2;
	hr = device.As(&dxgi_device2);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<ID2D1Factory1> d2d_factory1;
	D2D1_FACTORY_OPTIONS factory_options{};
#ifdef _DEBUG
	factory_options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory_options, d2d_factory1.GetAddressOf());

	Microsoft::WRL::ComPtr<ID2D1Device> d2d_device;
	hr = d2d_factory1->CreateDevice(dxgi_device2.Get(), d2d_device.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2d1_device_context.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = dxgi_device2->SetMaximumFrameLatency(1);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<IDXGISurface2> dxgi_surface2;
	hr = graphics.Get_swap_chain1()->GetBuffer(0, IID_PPV_ARGS(dxgi_surface2.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap1;
	hr = d2d1_device_context->CreateBitmapFromDxgiSurface(dxgi_surface2.Get(),
		D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), d2d_bitmap1.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	d2d1_device_context->SetTarget(d2d_bitmap1.Get());


	Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory;
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwrite_factory.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = dwrite_factory->CreateTextFormat(L"Meiryo", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11, L"", dwrite_text_formats[0].ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = dwrite_text_formats[0]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = dwrite_factory->CreateTextFormat(L"Impact", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24, L"", dwrite_text_formats[1].ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = dwrite_text_formats[1]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = d2d1_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), d2d_solid_color_brushes[0].ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = d2d1_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), d2d_solid_color_brushes[1].ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
#endif

int Framework::Run()
{
    MSG msg = {};




	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tictoc.tick();
			calculate_frame_stats();
			Update(tictoc.time_interval());
			Render(tictoc.time_interval());
		}
	}

#ifdef USE_IMGUI

#endif


	return static_cast<int>(msg.wParam);
}

LRESULT Framework::Handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
	graphics.Get_ImGui_renderer()->HandleMessage(hwnd, msg, wparam, lparam);
#endif

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_ENTERSIZEMOVE:
		tictoc.stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc.start();
		break;
	case WM_SIZE:
	{
#if 1
		RECT client_rect{};
		GetClientRect(hwnd, &client_rect);
		on_size_changed(static_cast<UINT64>(client_rect.right - client_rect.left), client_rect.bottom - client_rect.top);
		graphics.UpdateScreenSize(hwnd);
#endif
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}
