#pragma once

#include <sstream>
#include <d3d11.h>
#include <wrl.h>
#include "Scene.h"
#include "Graphics/Graphics.h"
#include "Graphics/sprite_bach.h"
#include "Graphics/Sprite.h"
#include "Graphics/shader.h"
#include "Graphics/skinned_mesh.h"
#include "Graphics/gltf_model.h"
#include "Graphics/Font.h"
#include "Graphics/framebuffer.h"
#include "Graphics/fullscreen_quad.h"
#include "Graphics/Model.h"
#include "Graphics/LightData.h"
#include "Graphics/GBuffer.h"
#include "CameraController.h"
#include "Graphics/Skymap.h"

class SceneTest : public Scene
{
public:
    SceneTest() {}
    ~SceneTest() override {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(float elapsedTime) override;

	void DrawGUI() override;

	void ResetShaderResource() override;

	void RemakeShaderResource(float width, float height) override;

	

private:
	struct LightConstants
	{
		directional_lights directional_light;
		point_lights point_light[8];			//	点光源
		spot_lights spot_light[8];				//	スポットライト
	};

	struct Param
	{
		float extraction_threshold;
		float gaussian_sigma;
		float bloom_intensity;
		float exposure;
		float extraction_threshold_end;
		DirectX::XMFLOAT3 padding;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> param_buffer;
	float gaussian_sigma = 1.0f;
	float bloom_intensity = 1.0f;
	float extraction_threshold = 0.8f;
	float exposure = 1.0f;
	float extraction_threshold_end = 1.0f;

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

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_buffer_render_target_view[GB_Max];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_buffer_shader_resource_view[GB_Max];

private:
	std::unique_ptr<sprite_batch> sprite_batches[8];
	std::unique_ptr<skinned_mesh> skinned_meshes;
	std::unique_ptr<gltf_model> gltf_models;
	std::unique_ptr<Model> model;
	std::unique_ptr<Font> font;
	std::unique_ptr<framebuffer> framebuffers[8];
	std::unique_ptr<fullscreen_quad> bit_block_transfer;
	std::unique_ptr<Skymap> skymap;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_emissive_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_indirect_pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferred_rendering_directional_pixel_shader;
	std::unique_ptr<Sprite> deferred_rendering_sprite;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];

	std::unique_ptr<Shader> shader;

	DirectX::XMFLOAT3 object_pos;
	DirectX::XMFLOAT3 object_rot;
	DirectX::XMFLOAT3 object_scale = { 0.1f,0.1f,0.1f };
	DirectX::XMFLOAT4 directional_light = { 0,0,1,0 };

	DirectX::XMFLOAT2 cursor_position = { 0,0 };
	DirectX::XMFLOAT3 camera_position = { 0,0,-10.0f };

	float animeTimer = 0;

	std::unique_ptr<GBuffer> g_buffer;

	CameraController camera_controller;

	float timer = { 0.0f };

	float modelAlpha = 1.0f;
};