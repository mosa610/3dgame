#include "SystemGbuffer.h"
#include "..//Graphics/Graphics.h"

void SystemGbufferRendering::Initialize(Register& reg)
{
	ID3D11Device* device = Graphics::Instance().Get_device();

	// makeGBuffer
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = Graphics::Instance().Get_screen_width();
		texture2d_desc.Height = Graphics::Instance().Get_screen_height();
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;

		DXGI_FORMAT formats[] =
		{
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R32_FLOAT,
		};
		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			texture2d_desc.Format = formats[i];

			Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};
			HRESULT hr = device->CreateTexture2D(&texture2d_desc, NULL, color_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//	レンダーターゲットビュー生成
			hr = device->CreateRenderTargetView(color_buffer.Get(), NULL, g_buffer_render_target_view[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//	シェーダーリソースビュー生成
			hr = device->CreateShaderResourceView(color_buffer.Get(), NULL, g_buffer_shader_resource_view[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//m_context->gbuffer_shader_resource_view[i] = g_buffer_shader_resource_view[i].Get();
		}
	}
}

void SystemGbufferRendering::update(Register& reg, float elapsed_time)
{
	
}

void SystemGbufferRendering::render(Register& reg)
{
	ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();

	//	サンプラステート設定
	ID3D11SamplerState* sampler_states[] =
	{
		GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::POINT).Get(),
		GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR).Get(),
		GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::ANISOTROPIC).Get(),
		GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR_BORDER_BLACK).Get(),
		GraphicsState::GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR_BORDER_WHITE).Get(),
	};
	dc->PSSetSamplers(0, _countof(sampler_states), sampler_states);


	//	出力先をGBufferに変更
	{
		ID3D11RenderTargetView* render_targets[GB_Max] =
		{
			g_buffer_render_target_view[GB_BaseColor].Get(),
			g_buffer_render_target_view[GB_Emissive].Get(),
			g_buffer_render_target_view[GB_Normal].Get(),
			g_buffer_render_target_view[GB_Parameters].Get(),
			g_buffer_render_target_view[GB_Depth].Get(),
		};
		FLOAT clear_color[]{ 0.f, 0.f, 0.f, .0f };
		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			if (i == GB_Depth) {
				clear_color[0] = 1.f;
			}
			else {
				clear_color[0] = 0.f;
			}
			dc->ClearRenderTargetView(render_targets[i], clear_color);
		}
		dc->ClearDepthStencilView(m_context->depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(GB_Max, render_targets, m_context->depth_stencil_view.Get());
	}
}

void SystemGbufferRendering::drawDebugGUI(Register& reg)
{
	if (ImGui::TreeNode("texture")) {
		//	GBufferを確認する
		static const char* GBufferNames[]
		{
			"base_color",
			"emissive",
			"normal",
			"parameters",
			"depth",
		};
		//ImGui::Image(Graphics::Instance().Get_render_target_resource_view(), {256, 144}, {0, 0}, {1, 1}, {1, 1, 1, 1});
		ImGui::NewLine();

		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			ImGui::Text(GBufferNames[i]);
			//ImGui::Image(g_buffer_shader_resource_view[i].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
			ImGui::Image((ImTextureID)g_buffer_shader_resource_view[i].Get(), ImVec2(256, 144), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
			ImGui::NewLine();
		}
		ImGui::TreePop();
	}
}
