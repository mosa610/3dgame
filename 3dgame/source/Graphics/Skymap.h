#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <memory>
#include "Sprite.h"
#include "render_context.h"

class Skymap
{
public:
    Skymap(ID3D11Device *device);
    ~Skymap();

    void update();
	void Render(ID3D11DeviceContext* immediate_context, const RenderContext& rc);

private:
	struct scene_constants
	{
		DirectX::XMFLOAT4X4 view_projection;
		DirectX::XMFLOAT4 options;	//	xy : マウスの座標値, z : タイマー, w : フラグ
		DirectX::XMFLOAT4 camera_position;
		DirectX::XMFLOAT4X4 inverse_view_projection;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;

	struct skymap_constants
	{
		DirectX::XMFLOAT4X4 inverse_view_projection;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> skymap_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> skymap_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> skymap_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skymap_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skymap_depth_stencil_state;
	D3D11_TEXTURE2D_DESC skymap_texture2d_desc;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap_shader_resource_view;
	std::unique_ptr<Sprite> skymap_sprite;

	//POINT cursor_position;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
};