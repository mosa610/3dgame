#pragma once

#include <memory>
#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>
#include <mutex>
#include "../misc.h"
#include "ImguiRenderer.h"
#include "shader.h"

//グラフィックス
class Graphics
{
public:
    Graphics(HWND hwnd);
    ~Graphics();

	//インスタンス取得
	static Graphics& Instance() { return *instance; }

	//デバイス取得
	ID3D11Device* Get_device() const { return device.Get(); }

	//デバイスコンテキスト取得
	ID3D11DeviceContext* Get_device_context() const { return immediate_context.Get(); }

	//スワップチェーン取得
	IDXGISwapChain* Get_swap_chain() const { return swap_chain.Get(); }

	//レンダーターゲットビュー取得
	ID3D11RenderTargetView* Get_render_target_view() const { return render_target_view.Get(); }
	

	//デプスステンシルビュー取得
	ID3D11DepthStencilView* Get_depth_stencil_view() const { return depth_stencil_view.Get(); }

	// シェーダー取得
	//Shader* Get_shader() const { return shader.get(); }

	//スクリーン幅取得
	float Get_screen_width() const { return screen_width; }

	//スクリーン高さ取得
	float Get_screen_height() const { return screen_height; }

	// ImGuiレンダラ取得
	ImGuiRenderer* Get_ImGui_renderer() const { return imgui_renderer.get(); }
private:
    static Graphics* instance;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediate_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swap_chain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	render_target_view;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depth_stencil_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depth_stencil_view;

	//std::unique_ptr<Shader>							shader;
	std::unique_ptr<ImGuiRenderer>					imgui_renderer;

	float	screen_width;
	float	screen_height;
};