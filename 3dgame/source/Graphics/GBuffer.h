#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Sprite.h"

enum GBufferId
{
	GB_BaseColor = 0,
	GB_Emissive,
	GB_Normal,
	GB_Parameters,
	GB_Depth,
	//	必要なら追加(シェーダー側の対応も必須)
	GB_Max,
};

class GBuffer
{
public:
    GBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scene_shader_resource_view = nullptr);
    ~GBuffer();
public:
	void Begin(ID3D11DepthStencilView* dsv);
	void End(ID3D11DepthStencilView* dsv, ID3D11RenderTargetView* rtv, FLOAT color[]);

	void SetCommonResource();

	void Render();

	void FinalDraw();

	void DrawGUI();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView(GBufferId id) { return g_buffer_shader_resource_view[id]; }
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_buffer_render_target_view[GB_Max];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_buffer_shader_resource_view[GB_Max];

	// deferred
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_emissive_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_indirect_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_directional_pixel_shader;

	std::unique_ptr<Sprite> deferred_rendering_sprite;

	// Sprite
	Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

	// IBL
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse_iem_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specular_pmrem_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lut_ggx_shader_resource_view;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> final_rendering_pixel_shader;
	std::unique_ptr<Sprite> final_render_sprite;
};