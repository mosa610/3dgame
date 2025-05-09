#include "SceneTest.h"
#include <array>
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "Graphics/shader.h"
#include "Graphics/GraphicsState.h"
#include "Graphics/ModelRenderer.h"
#include "Mouse.h"


void SceneTest::Initialize()
{
	ID3D11Device* device = Graphics::Instance().Get_device();

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(LightConstants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = Graphics::Instance().Get_device()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = sizeof(Param);
	hr = Graphics::Instance().Get_device()->CreateBuffer(&buffer_desc, nullptr, param_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.Get_screen_width() / graphics.Get_screen_height(),
		0.1f,
		1000.0f
	);
	camera.SetPosition(camera_position);
	
	camera_controller.SyncCameraToController(camera);

	float width = graphics.Get_screen_width();
    float height = graphics.Get_screen_height();
	framebuffers[0] = std::make_unique<framebuffer>(graphics.Get_device(), width, height);
	framebuffers[1] = std::make_unique<framebuffer>(graphics.Get_device(), width / 2, height / 2);

	sprite_batches[0] = std::make_unique<sprite_batch>(L".\\resources\\screenshot.jpg", 1);
	//skinned_meshes = std::make_unique<skinned_mesh>(Graphics::Instance().Get_device(), ".\\resources\\plantune.fbx", 0, true);
	gltf_models = std::make_unique<gltf_model>(graphics.Get_device(),
		/*".\\glTF-Sample-Models-main\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf"*/
		/*".//resources//TestAssets//Wraith.gltf"*/
		".\\glTF-Sample-Models-main\\2.0\\BrainStem\\glTF\\BrainStem.gltf");

	model = std::make_unique<Model>(graphics.Get_device(),
		/*".\\glTF-Sample-Models-main\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf"*/
		".\\resources\\gltfobject\\set costume_02 sd unity-chan wgs.gltf"
		/*".\\glTF-Sample-Models-main\\2.0\\BrainStem\\glTF\\BrainStem.gltf"*/
		/*".\\glTF-Sample-Models-main\\2.0\\Duck\\glTF\\Duck.gltf"*/);
	model->GetNodePoses(model->_nodePoses);

	shader = std::make_unique<ModelRenderer>(graphics.Get_device(),graphics.Get_device_context());

	font = std::make_unique<Font>(graphics.Get_device(), ".\\Data\\Font\\MS_Gothic.fnt", 1024);

	create_ps_from_cso(graphics.Get_device(), ".//Data//Shader//luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());
	create_ps_from_cso(graphics.Get_device(), ".//Data//Shader//blur_ps.cso", pixel_shaders[1].GetAddressOf());

	bit_block_transfer = std::make_unique<fullscreen_quad>(graphics.Get_device());

	skymap = std::make_unique<Skymap>(graphics.Get_device());

	//// makeGBuffer
	//{
	//	D3D11_TEXTURE2D_DESC texture2d_desc{};
	//	texture2d_desc.Width = Graphics::Instance().Get_screen_width();
	//	texture2d_desc.Height = Graphics::Instance().Get_screen_height();
	//	texture2d_desc.MipLevels = 1;
	//	texture2d_desc.ArraySize = 1;
	//	texture2d_desc.SampleDesc.Count = 1;
	//	texture2d_desc.SampleDesc.Quality = 0;
	//	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	//	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	texture2d_desc.CPUAccessFlags = 0;
	//	texture2d_desc.MiscFlags = 0;

	//	DXGI_FORMAT formats[] =
	//	{
	//		DXGI_FORMAT_R8G8B8A8_UNORM,
	//		DXGI_FORMAT_R8G8B8A8_UNORM,
	//		DXGI_FORMAT_R32G32B32A32_FLOAT,
	//		DXGI_FORMAT_R8G8B8A8_UNORM,
	//		DXGI_FORMAT_R32_FLOAT,
	//	};
	//	for (int i = GB_BaseColor; i < GB_Max; ++i)
	//	{
	//		texture2d_desc.Format = formats[i];

	//		Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};
	//		hr = device->CreateTexture2D(&texture2d_desc, NULL, color_buffer.GetAddressOf());
	//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//		//	レンダーターゲットビュー生成
	//		hr = device->CreateRenderTargetView(color_buffer.Get(), NULL, g_buffer_render_target_view[i].GetAddressOf());
	//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//		//	シェーダーリソースビュー生成
	//		hr = device->CreateShaderResourceView(color_buffer.Get(), NULL, g_buffer_shader_resource_view[i].GetAddressOf());
	//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//	}
	//}
	
	/*create_ps_from_cso(graphics.Get_device(), ".//Data//Shader//deferred_rendering_emissive_ps.cso", deferred_rendering_emissive_pixel_shader.GetAddressOf());
	create_ps_from_cso(graphics.Get_device(), ".//Data//Shader//deferred_rendering_indirect_ps.cso", deferred_rendering_indirect_pixel_shader.GetAddressOf());
	create_ps_from_cso(graphics.Get_device(), ".//Data//Shader//deferred_rendering_directional_ps.cso", deferred_rendering_directional_pixel_shader.GetAddressOf());*/

	g_buffer = std::make_unique<GBuffer>(GetSceneShaderResourceView());

	deferred_rendering_sprite = std::make_unique<Sprite>(graphics.Get_device(), g_buffer_shader_resource_view[GB_BaseColor]);
}

void SceneTest::Finalize()
{
	framebuffers[0]->ResetShaderResourceView();
	framebuffers[1]->ResetShaderResourceView();
}

void SceneTest::Update(float elapsedTime)
{
	Scene::SetSceneConstant(1, DirectX::XMFLOAT2(Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height()), true);
	timer += elapsedTime;

	// シーン用定数バッファ設定

	/*framebuffers[0]->UpdateWindowSize(Graphics::Instance().Get_device(),
		Graphics::Instance().Get_screen_width(), Graphics::Instance().Get_screen_height());*/

	Camera& camera = Camera::Instance();

	camera_controller.Update();
	camera_controller.SyncControllerToCamera(camera);
	
	model->ComputeAnimation(0, animeTimer, model->_nodePoses);

	// アニメーション更新
	const ModelResource::Animation& animation = model->_resource->GetAnimations().at(0);
	animeTimer += elapsedTime;
	if (animeTimer > animation.secondsLength)
	{
		animeTimer -= animation.secondsLength;
	}
	model->SetNodePoses(model->_nodePoses);
	model->Update(elapsedTime);

	skymap->update();

}


void SceneTest::Render(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.Get_device_context();
	ID3D11RenderTargetView* rtv = graphics.Get_render_target_view();
	ID3D11DepthStencilView* dsv = graphics.Get_depth_stencil_view();

	GraphicsState& GState = GraphicsState::GetInstance();

	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	dc->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	dc->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	dc->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	/*dc->ClearRenderTargetView(scene_render_target_view.Get(), color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, scene_render_target_view.GetAddressOf(), dsv);*/

	dc->ClearRenderTargetView(scene_render_target_view.Get(), color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, scene_render_target_view.GetAddressOf(), dsv);


	// 描画処理
	RenderContext rc;
	rc.lightDirection = { directional_light.x, directional_light.y, directional_light.z,directional_light.w };	// ライト方向

	//カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	dc->RSGetViewports(&num_viewports, &viewport);

	float aspect_ratio{ viewport.Width / viewport.Height };
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&rc.projection);

	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&rc.view);

	// 定数バッファ更新
	{
		Param param;
		param.bloom_intensity = bloom_intensity;
		param.gaussian_sigma = gaussian_sigma;
		param.exposure = exposure;
		param.extraction_threshold = extraction_threshold;
		param.extraction_threshold_end = extraction_threshold_end;
		dc->UpdateSubresource(param_buffer.Get(), 0, 0, &param, 0, 0);
		dc->VSSetConstantBuffers(3, 1, param_buffer.GetAddressOf());
		dc->PSSetConstantBuffers(3, 1, param_buffer.GetAddressOf());

		scene_constants data{};

		scene_previous_view_projection = scene_view_projection;
		DirectX::XMStoreFloat4x4(&scene_view_projection, V * P);
		/*scene_view_projection = data.view_projection;
		DirectX::XMFLOAT3 cp = camera.GetPosition();
		data.camera_position = DirectX::XMFLOAT4(cp.x, cp.y, cp.z, 0);
		data.light_direction = rc.lightDirection;*/

		LightConstants data_l{};
		data_l.directional_light.direction = rc.lightDirection;
		data_l.directional_light.color = DirectX::XMFLOAT4(1, 1, 1, 1);

		dc->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data_l, 0, 0);
		dc->VSSetConstantBuffers(4, 1, constant_buffers[0].GetAddressOf());
		dc->PSSetConstantBuffers(4, 1, constant_buffers[0].GetAddressOf());
	}

	dc->PSSetSamplers(3, 1, GState.GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR_BORDER_BLACK).GetAddressOf());
	dc->PSSetSamplers(4, 1, GState.GetInstance().GetSamplerState(SAMPLER_STATE::LINEAR_BORDER_WHITE).GetAddressOf());


	//	出力先をGBufferに変更
	{
		/*ID3D11RenderTargetView* render_targets[GB_Max] =
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
		dc->OMSetRenderTargets(GB_Max, render_targets, dsv);*/
		g_buffer->Begin(dsv);
	}
	



	DirectX::XMFLOAT4X4 world1;
	DirectX::XMFLOAT4X4 world4;
	{
		const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
			{
				-1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			},	//0:RHS Y-UP
			{
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			},	//1:LHS Y-UP
			{
				-1,0,0,0,
				0,0,-1,0,
				0,1,0,0,
				0,0,0,1
			},	//2:RHS Z-UP
			{
				1,0,0,0,
				0,0,1,0,
				0,1,0,0,
				0,0,0,1
			},	//3:LHS Z-UP
		};

		const float scale_factor = 1.0f;
		DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[1])
			* DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };

		DirectX::XMMATRIX S4 = { DirectX::XMMatrixScaling(object_scale.x, object_scale.y, object_scale.z) };
		DirectX::XMMATRIX R4 = { DirectX::XMMatrixRotationRollPitchYaw(object_rot.x, object_rot.y, object_rot.z) };
		DirectX::XMMATRIX T4 = { DirectX::XMMatrixTranslation(object_pos.x, object_pos.y, object_pos.z) };

		DirectX::XMStoreFloat4x4(&world4,/*C * */S4 * R4 * T4);

		DirectX::XMMATRIX S1 = { DirectX::XMMatrixScaling(0.001f,0.001f,0.001f) };
		DirectX::XMMATRIX R1 = { DirectX::XMMatrixRotationRollPitchYaw(0,0,0) };
		DirectX::XMMATRIX T1 = { DirectX::XMMatrixTranslation(object_pos.x + 10, object_pos.y, object_pos.z) };

		DirectX::XMStoreFloat4x4(&world1, C* S1* R1* T1);
	}

	//アニメーション更新
	int clip_index{ 0 };
	int frame_index{ 0 };
	static float animation_tick{ 0 };

	static std::vector<gltf_model::node> animated_nodes{ gltf_models->nodes };
	//static float time{ 0 };
	//gltf_models->animate(0/*animation index*/, time += elapsedTime, animated_nodes);
	//if (gltf_models->animations.at(0/*animation index*/).duration < time)
	//{
	//	time = 0; // Repeat playback
	//}
	{

		/*animation& animation{ skinned_meshes->animation_clips.at(clip_index) };
		frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
		if (frame_index > animation.sequence.size() - 1)
		{
			frame_index = 0;
			animation_tick = 0;
		}
		else
		{
			animation_tick += elapsedTime;
		}
		animation::keyframe& keyframe{ animation.sequence.at(frame_index) };*/

		//static float time{ 0 };
		//gltf_models->animate(0/*animation index*/, time += elapsedTime, animated_nodes);
		//if (gltf_models->animations.at(0/*animation index*/).duration < time)
		//{
		//	time = 0; // Repeat playback
		//}
	}
	//skinned_meshes->render(Graphics::Instance().Get_device_context(), world4, { 1,1,1,1 }, &keyframe);

	//gltf_models->render(graphics.Get_device_context(), world4, animated_nodes);
	//gltf_models->render_bounding_box(graphics.Get_device_context(), world4, rc.view, rc.projection, { 1,0,0,1 });


	{
		skymap->Render(dc, rc);
	}

	model->_worldTransform = world4;
	model->UpdateTransform(world4);

	shader->Begin(dc, rc,model.get(), "main");
	shader->Draw(dc, model.get(), modelAlpha);
    shader->End(dc);

	//	出力先をシーンに変更
	{
		/*dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);*/

		g_buffer->End(dsv, scene_render_target_view.Get(), color);
	}

	framebuffers[0]->clear(dc, 0.4f, 0.4f, 0.4f);
	framebuffers[0]->activate(dc);



	{
		/*dc->PSSetShader(deferred_rendering_indirect_pixel_shader.Get(), nullptr, 0);
		deferred_rendering_sprite->render(dc, 0, 0, graphics.Get_screen_width(), graphics.Get_screen_height());*/
		g_buffer->Render();
	}




	//// 2D描画
	//{
	//	sprite_batches[0]->begin(graphics.Get_device_context(), 0);
	//	sprite_batches[0]->render(graphics.Get_device_context(),
	//		{ 0,0 }, { graphics.Get_screen_width(), graphics.Get_screen_height() }, { 1,1,1,1 }, 0, { 0,0 }, { 1600,900 });
	//	sprite_batches[0]->end(graphics.Get_device_context());

	//}
	framebuffers[0]->deactivate(dc);

	ID3D11ShaderResourceView* srv = graphics.Get_render_target_resource_view();


	framebuffers[1]->clear(dc);
	framebuffers[1]->activate(dc);
	dc->OMSetDepthStencilState(GState.GetDepthStencilState(DEPTH_STATE::ZT_OFF_ZW_OFF).Get(), 0);
	dc->RSSetState(GState.GetRasterizerState(RASTER_STATE::CULL_NONE).Get());
	dc->OMSetBlendState(GState.GetBlendState(BLEND_STATE::ALPHA).Get(), nullptr, 0xFFFFFFFF);
	bit_block_transfer->blit(dc,
		framebuffers[0]->shader_resource_view[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
	framebuffers[1]->deactivate(dc);

	dc->OMSetDepthStencilState(GState.GetDepthStencilState(DEPTH_STATE::ZT_OFF_ZW_OFF).Get(), 0);
	dc->RSSetState(GState.GetRasterizerState(RASTER_STATE::CULL_NONE).Get());

	ID3D11ShaderResourceView* shader_resource_views[2] =
	{ framebuffers[0]->shader_resource_view[0].Get(), framebuffers[1]->shader_resource_view[0].Get()};
	bit_block_transfer->blit(dc, shader_resource_views, 0, 2, pixel_shaders[1].Get());


	font->Begin(graphics.Get_device_context());
	font->Draw(0, 100, L"あああ");
	font->End(graphics.Get_device_context());

	g_buffer->FinalDraw();
}

void SceneTest::DrawGUI()
{
	ImGui::Begin("ImGUI");
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
	if (ImGui::TreeNode("test02")) {
		ImGui::SliderFloat3("objPos", &object_pos.x, -10.0f, 10.0f, "%.1f");
		ImGui::SliderFloat3("objRot", &object_rot.x, -DirectX::XM_2PI, DirectX::XM_2PI, "%.1f");
		ImGui::SliderFloat3("objScale", &object_scale.x, 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat4("light", &directional_light.x, -20.0f, 20.0f, "%.1f");
		ImGui::SliderFloat("gaussian_sigma", { &gaussian_sigma }, 0.0f, 10.0f);
		ImGui::SliderFloat("bloom_intensity", { &bloom_intensity }, 0.0f, 10.0f);
		ImGui::SliderFloat("extraction", &extraction_threshold, 0.0f, 5.0f, "%.1f");
		ImGui::SliderFloat("extraction_end", &extraction_threshold_end, 0.0f, 5.0f, "%.1f");
		ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f, "%.1f");
		ImGui::SliderFloat("alpha", &modelAlpha, 0.0f, 1.0f, "%.1f");
		ImGui::NewLine();
		ImGui::Text("scene");
		ImGui::Image(scene_shader_resource_view.Get(),{ 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Text("framebuffer");
		ImGui::Image(framebuffers[1]->shader_resource_view->Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Camera"))
	{
		ImGui::Text("near : %.2f", Camera::Instance().GetNearClipDistance());
        ImGui::Text("far : %.2f", Camera::Instance().GetFarClipDistance());
		DirectX::XMFLOAT3 pos = Camera::Instance().GetPosition();
		DirectX::XMFLOAT3 focus = Camera::Instance().GetFocus();
		ImGui::SliderFloat3("pos", &pos.x, -10.0f, 10.0f, "%.1f");
		ImGui::SliderFloat3("focus", &focus.x, -10.0f, 10.0f, "%.1f");

		ImGui::TreePop();
	}

	g_buffer->DrawGUI();

	model->DrawGUI();

	ImGui::End();
}

void SceneTest::ResetShaderResource()
{
	framebuffers[0]->ResetShaderResourceView();
	framebuffers[1]->ResetShaderResourceView();
	g_buffer->ResetShaderResourceView();
	//scene_shader_resource_view.Reset();
	//scene_render_target_view.Reset();
}

void SceneTest::RemakeShaderResource(float width, float height)
{

	ID3D11Device* device = Graphics::Instance().Get_device();
	if(0)
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = 1280.0f;
		texture2d_desc.Height = 720.0f;
		texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> color_buffer{};

		HRESULT hr = device->CreateTexture2D(&texture2d_desc, NULL, color_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateRenderTargetView(color_buffer.Get(), NULL, scene_render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateShaderResourceView(color_buffer.Get(), NULL, scene_shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	/*{
		Camera& camera = Camera::Instance();
		camera.SetLookAt(
			DirectX::XMFLOAT3(0, 10, -10),
			DirectX::XMFLOAT3(0, 0, 0),
			DirectX::XMFLOAT3(0, 1, 0)
		);
		camera.SetPerspectiveFov(
			DirectX::XMConvertToRadians(45),
			width / height,
			0.1f,
			1000.0f
		);
		camera.SetPosition(camera_position);

		camera_controller.SyncCameraToController(camera);
	}*/

	framebuffers[0]->UpdateWindowSize(device, width, height);
	framebuffers[1]->UpdateWindowSize(device, width / 2.0f, height / 2.0f);
	g_buffer->ResizeRenderTarget(width, height);
}

void Scene::SetSceneConstant(int start_slot, DirectX::XMFLOAT2 viewport_size, bool is_update_resource)
{
	if (is_update_resource)
	{
		gbuffer_scene_constants scene;
		DirectX::XMFLOAT2 cursor_position = Mouse::GetInstance().GetCursorPosition();
		scene.options.x = static_cast<float>(cursor_position.x);
		scene.options.y = static_cast<float>(cursor_position.y);
		scene.options.z = timer;
		scene.options.w = false;
		float far_clip_distance = Camera::Instance().GetFarClipDistance();
		float near_clip_distance = Camera::Instance().GetNearClipDistance();
		scene.z_buffer_parameteres.y = far_clip_distance / near_clip_distance;
		scene.z_buffer_parameteres.x = 1.0f - scene.z_buffer_parameteres.y;
		scene.z_buffer_parameteres.z = scene.z_buffer_parameteres.x / far_clip_distance;
		scene.z_buffer_parameteres.w = scene.z_buffer_parameteres.y / far_clip_distance;
		DirectX::XMFLOAT3 camera_position = Camera::Instance().GetPosition();
		scene.camera_position.x = camera_position.x;
		scene.camera_position.y = camera_position.y;
		scene.camera_position.z = camera_position.z;
		scene.camera_position.w = 1;
		DirectX::XMFLOAT3 camera_focus = Camera::Instance().GetFocus();
		scene.camera_direction.x = camera_focus.x - camera_position.x;
		scene.camera_direction.y = camera_focus.y - camera_position.y;
		scene.camera_direction.z = camera_focus.z - camera_position.z;
		float d = sqrtf(scene.camera_direction.x * scene.camera_direction.x + scene.camera_direction.y * scene.camera_direction.y + scene.camera_direction.z * scene.camera_direction.z);
		scene.camera_direction.x /= d;
		scene.camera_direction.y /= d;
		scene.camera_direction.z /= d;
		scene.camera_direction.w = 1;
		scene.camera_clip_distance.x = near_clip_distance;
		scene.camera_clip_distance.y = far_clip_distance;
		scene.camera_clip_distance.z = near_clip_distance * far_clip_distance;
		scene.camera_clip_distance.w = far_clip_distance - near_clip_distance;
		scene.viewport_size.x = viewport_size.x;
		scene.viewport_size.y = viewport_size.y;
		scene.viewport_size.z = 1.0f / viewport_size.x;
		scene.viewport_size.w = 1.0f / viewport_size.y;
		scene.view_transform = Camera::Instance().GetView();
		scene.projection_transform = Camera::Instance().GetProjection();
		/*DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&scene.view_transform);
		DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&scene.projection_transform);
		DirectX::XMStoreFloat4x4(&scene.view_projection_transform, V * P);*/
		scene.view_projection_transform = scene_view_projection;
		scene.previous_view_projection_transform = scene_previous_view_projection;

		DirectX::XMStoreFloat4x4(&scene.inverse_view_transform, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&scene.view_transform)));
		DirectX::XMStoreFloat4x4(&scene.inverse_projection_transform, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&scene.projection_transform)));
		DirectX::XMStoreFloat4x4(&scene.inverse_view_projection_transform, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&scene.view_projection_transform)));

		Graphics::Instance().Get_device_context()->UpdateSubresource(scene_constant_buffer.Get(), 0, 0, &scene, 0, 0);
	}
	Graphics::Instance().Get_device_context()->VSSetConstantBuffers(start_slot, 1, scene_constant_buffer.GetAddressOf());
	Graphics::Instance().Get_device_context()->PSSetConstantBuffers(start_slot, 1, scene_constant_buffer.GetAddressOf());

}
