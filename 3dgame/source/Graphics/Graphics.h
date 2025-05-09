#pragma once

#include <memory>
#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>
#include <mutex>
#include <dxgi1_6.h>
#include "../misc.h"
#include "ImguiRenderer.h"
#include "shader.h"

#define ENABLE_DIRECT2D
#ifdef ENABLE_DIRECT2D
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#endif

//グラフィックス
class Graphics
{
public:
    Graphics(HWND hwnd);
    ~Graphics();

	void create_swap_chain(IDXGIFactory6* dxgi_factory6,
		CONST HWND hwnd, BOOL tearing_supported, SIZE framebuffer_dimensions);

	//インスタンス取得
	static Graphics& Instance() { return *instance; }

	//デバイス取得
	ID3D11Device* Get_device() const { return device.Get(); }

	//デバイスコンテキスト取得
	ID3D11DeviceContext* Get_device_context() const { return immediate_context.Get(); }

	//スワップチェーン取得
	IDXGISwapChain* Get_swap_chain() const { return swap_chain.Get(); }

	IDXGISwapChain1* Get_swap_chain1() const { return swap_chain1.Get(); }

	//レンダーターゲットビュー取得
	ID3D11RenderTargetView* Get_render_target_view() const { return render_target_view.Get(); }

	ID3D11ShaderResourceView* Get_render_target_resource_view() const { return shader_resource_view.Get(); }

	//デプスステンシルビュー取得
	ID3D11DepthStencilView* Get_depth_stencil_view() const { return depth_stencil_view.Get(); }

	// シェーダー取得
	//Shader* Get_shader() const { return shader.get(); }

	//スクリーン幅取得
	float Get_screen_width() const { return screen_width; }

	//スクリーン高さ取得
	float Get_screen_height() const { return screen_height; }

	float GetCurrentWindowWidth() const { return current_window_width; }

    float GetCurrentWindowHeight() const { return current_window_height; }

	void UpdateScreenSize(HWND hwnd)
	{
		GetClientRect(hwnd, &rc);
		UINT screen_width = rc.right - rc.left;
		UINT screen_height = rc.bottom - rc.top;

		this->screen_width = static_cast<float>(screen_width);
		this->screen_height = static_cast<float>(screen_height);
	}

	// ImGuiレンダラ取得
	ImGuiRenderer* Get_ImGui_renderer() const { return imgui_renderer.get(); }

	void DebugDraw() const;

private:
    static Graphics* instance;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swap_chain;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>			swap_chain1;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depth_stencil_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;
	Microsoft::WRL::ComPtr<IDXGIAdapter3>			adapter;

	//std::unique_ptr<Shader>							shader;
	std::unique_ptr<ImGuiRenderer>					imgui_renderer;

	float	screen_width;
	float	screen_height;

	float	current_window_width;
	float	current_window_height;

	BOOL tearing_supported{ FALSE };

	SIZE framebuffer_dimensions;

	RECT rc;
};