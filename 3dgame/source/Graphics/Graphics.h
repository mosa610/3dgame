#pragma once

#include <memory>
#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>
#include <mutex>
#include "../misc.h"
#include "ImguiRenderer.h"
#include "shader.h"

//�O���t�B�b�N�X
class Graphics
{
public:
    Graphics(HWND hwnd);
    ~Graphics();

	//�C���X�^���X�擾
	static Graphics& Instance() { return *instance; }

	//�f�o�C�X�擾
	ID3D11Device* Get_device() const { return device.Get(); }

	//�f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* Get_device_context() const { return immediate_context.Get(); }

	//�X���b�v�`�F�[���擾
	IDXGISwapChain* Get_swap_chain() const { return swap_chain.Get(); }

	//�����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* Get_render_target_view() const { return render_target_view.Get(); }
	

	//�f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* Get_depth_stencil_view() const { return depth_stencil_view.Get(); }

	// �V�F�[�_�[�擾
	//Shader* Get_shader() const { return shader.get(); }

	//�X�N���[�����擾
	float Get_screen_width() const { return screen_width; }

	//�X�N���[�������擾
	float Get_screen_height() const { return screen_height; }

	// ImGui�����_���擾
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