#include "GBuffer.h"
#include "Graphics.h"
#include "GraphicsState.h"
#include "..//imgui/imgui.h"
#include "shader.h"
#include "texture.h"

GBuffer::GBuffer(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scene_shader_resource_view)
{
    ID3D11Device* device = Graphics::Instance().Get_device();

	// makeGBuffer
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = /*Graphics::Instance().Get_screen_width()*/1280.0f;
		texture2d_desc.Height = /*Graphics::Instance().Get_screen_height()*/720.0f;
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
		}
	}


	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		create_vs_from_cso(device, ".//Data//Shader//sprite_vs.cso", sprite_vertex_shader.GetAddressOf(), sprite_input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
		create_ps_from_cso(device, ".//Data//Shader//sprite_ps.cso", sprite_pixel_shader.GetAddressOf());

		create_ps_from_cso(device, ".//Data//Shader//deferred_rendering_emissive_ps.cso", deferred_rendering_emissive_pixel_shader.GetAddressOf());
		create_ps_from_cso(device, ".//Data//Shader//deferred_rendering_indirect_ps.cso", deferred_rendering_indirect_pixel_shader.GetAddressOf());
		create_ps_from_cso(device, ".//Data//Shader//deferred_rendering_directional_ps.cso", deferred_rendering_directional_pixel_shader.GetAddressOf());

        create_ps_from_cso(device, ".//Data//Shader//final_pass_ps.cso", final_rendering_pixel_shader.GetAddressOf());
	}

	
		D3D11_TEXTURE2D_DESC texture2d_desc;
		load_texture_from_file(device, L".\\resources\\country_club_4k\\diffuse_iem.dds", diffuse_iem_shader_resource_view.GetAddressOf(), &texture2d_desc);
		load_texture_from_file(device, L".\\resources\\country_club_4k\\specular_pmrem.dds", specular_pmrem_shader_resource_view.GetAddressOf(), &texture2d_desc);
		load_texture_from_file(device, L".\\resources\\lut_ggx.dds", lut_ggx_shader_resource_view.GetAddressOf(), &texture2d_desc);


	deferred_rendering_sprite = std::make_unique<Sprite>(device, g_buffer_shader_resource_view[GB_BaseColor]);

	final_render_sprite = std::make_unique<Sprite>(device, scene_shader_resource_view);
}

GBuffer::~GBuffer()
{
	for (int i = GB_BaseColor; i < GB_Max; ++i)
	{
		g_buffer_shader_resource_view[i].Reset();
	}
}

void GBuffer::Begin(ID3D11DepthStencilView* dsv)
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
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(GB_Max, render_targets, dsv);
	}
}

void GBuffer::End(ID3D11DepthStencilView* dsv, ID3D11RenderTargetView* rtv, FLOAT color[])
{
	ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
	//	出力先をシーンに変更
	{
		dc->ClearRenderTargetView(rtv, color);
		//dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, nullptr);
	}
}

void GBuffer::SetCommonResource()
{
	ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
	static constexpr int GBufferSRVIndex = 0;	//	0はSprite側で指定しているのでそれを考慮

	if (lut_ggx_shader_resource_view != nullptr)
	{
		dc->PSSetShaderResources(35, 1, lut_ggx_shader_resource_view.GetAddressOf());
	}

	//	GBuffer設定
	ID3D11ShaderResourceView* shader_resource_views[GB_Max] =
	{
		g_buffer_shader_resource_view[GB_BaseColor].Get(),
		g_buffer_shader_resource_view[GB_Emissive].Get(),
		g_buffer_shader_resource_view[GB_Normal].Get(),
		g_buffer_shader_resource_view[GB_Parameters].Get(),
		g_buffer_shader_resource_view[GB_Depth].Get(),
	};
	dc->PSSetShaderResources(GBufferSRVIndex, GB_Max, shader_resource_views);

}

void GBuffer::Render()
{
	ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();

	SetCommonResource();
	//	間接光処理
	{
		//	IBLテクスチャを設定
		if(diffuse_iem_shader_resource_view != nullptr)
			dc->PSSetShaderResources(33, 1, diffuse_iem_shader_resource_view.GetAddressOf());
		if (specular_pmrem_shader_resource_view != nullptr)
			dc->PSSetShaderResources(34, 1, specular_pmrem_shader_resource_view.GetAddressOf());

		//	全画面描画
		GraphicsState::GetInstance().SetDepthStencilState(dc, DEPTH_STATE::ZT_OFF_ZW_OFF);
		GraphicsState::GetInstance().SetBlendState(dc, BLEND_STATE::ALPHA);
		GraphicsState::GetInstance().SetRasterizerState(dc, RASTER_STATE::CULL_NONE);

		dc->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
		dc->IASetInputLayout(sprite_input_layout.Get());

		dc->PSSetShader(deferred_rendering_indirect_pixel_shader.Get(), nullptr, 0);
		deferred_rendering_sprite->render(dc, 0, 0, Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
	}

	//	自己発光処理
	{
		GraphicsState::GetInstance().SetDepthStencilState(dc, DEPTH_STATE::ZT_OFF_ZW_OFF);
		GraphicsState::GetInstance().SetBlendState(dc, BLEND_STATE::ADD);
		GraphicsState::GetInstance().SetRasterizerState(dc, RASTER_STATE::CULL_NONE);

		dc->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
		dc->IASetInputLayout(sprite_input_layout.Get());
		dc->PSSetShader(deferred_rendering_emissive_pixel_shader.Get(), nullptr, 0);
		deferred_rendering_sprite->render(dc, 0, 0, Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
	}

	ID3D11ShaderResourceView* clear_shader_resource_view[GB_Max - 1]{ nullptr, nullptr, nullptr, nullptr };
	dc->PSSetShaderResources(1, GB_Max - 1, clear_shader_resource_view);


}

void GBuffer::FinalDraw()
{
	ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
	ID3D11RenderTargetView* rtv = Graphics::Instance().Get_render_target_view();

	// 出力先をbackbufferに変更
	{
		FLOAT color[]{ 0.f, 0.f, 0.f, .0f };
		dc->ClearRenderTargetView(rtv, color);
		//dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, nullptr);
	}

	{
		GraphicsState::GetInstance().SetDepthStencilState(dc, DEPTH_STATE::ZT_OFF_ZW_OFF);
		GraphicsState::GetInstance().SetBlendState(dc, BLEND_STATE::NONE);
		GraphicsState::GetInstance().SetRasterizerState(dc, RASTER_STATE::CULL_NONE);

        dc->VSSetShader(sprite_vertex_shader.Get(), nullptr, 0);
        dc->IASetInputLayout(sprite_input_layout.Get());
        dc->PSSetShader(final_rendering_pixel_shader.Get(), nullptr, 0);
        final_render_sprite->render(dc, 0, 0, Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());
	}
}

void GBuffer::DrawGUI()
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
		ImGui::Image(Graphics::Instance().Get_render_target_resource_view(), {256, 144}, {0, 0}, {1, 1}, {1, 1, 1, 1});
		ImGui::NewLine();

		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			ImGui::Text(GBufferNames[i]);
			ImGui::Image(g_buffer_shader_resource_view[i].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
			ImGui::NewLine();
		}
		ImGui::TreePop();
	}
}

void GBuffer::ResizeRenderTarget(float width, float height)
{
	ID3D11Device* device = Graphics::Instance().Get_device();

	// makeGBuffer
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = 1280.0f;
		texture2d_desc.Height = 720.0f;
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
		}
	}
}

void GBuffer::ResetShaderResourceView()
{
	for (int i = GB_BaseColor; i < GB_Max; ++i)
	{
		g_buffer_render_target_view[i].Reset();
		g_buffer_shader_resource_view[i].Reset();
	}

	diffuse_iem_shader_resource_view.Reset();
	specular_pmrem_shader_resource_view.Reset();
	lut_ggx_shader_resource_view.Reset();
}
