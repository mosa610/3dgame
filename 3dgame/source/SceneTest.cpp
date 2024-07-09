#include "Graphics/Graphics.h"
#include "SceneTest.h"
#include "Camera.h"



void SceneTest::Initialize()
{
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	HRESULT hr = Graphics::Instance().Get_device()->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(30),
		graphics.Get_screen_width() / graphics.Get_screen_height(),
		0.1f,
		100.0f
	);

	sprite_batches[0] = std::make_unique<sprite_batch>(L".\\resources\\screenshot.jpg", 1);
	skinned_meshes = std::make_unique<skinned_mesh>(Graphics::Instance().Get_device(), ".\\resources\\plantune.fbx", 0, true);
	font = std::make_unique<Font>(graphics.Get_device(), ".\\Data\\Font\\MS_Gothic.fnt", 1024);
}

void SceneTest::Finalize()
{
}

void SceneTest::Update(float elapsedTime)
{
}

void SceneTest::Render(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.Get_device_context();
	ID3D11RenderTargetView* rtv = graphics.Get_render_target_view();
	ID3D11DepthStencilView* dsv = graphics.Get_depth_stencil_view();


	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);


	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

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



	scene_constants data{};

	DirectX::XMStoreFloat4x4(&data.view_projection, V * P);
	data.camera_position = { 0.0f,0.0f,-1.0f,0.0f };
	data.light_direction = rc.lightDirection;

	dc->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	dc->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	dc->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	sprite_batches[0]->begin(graphics.Get_device_context(), 0);
	sprite_batches[0]->render(graphics.Get_device_context(),
		{ 0,0 }, { 1280, 720 }, { 1,1,1,1 }, 0, { 0,0 }, { 1600,900 });
	sprite_batches[0]->end(graphics.Get_device_context());

	font->Begin(graphics.Get_device_context());
	font->Draw(0, 100, L"あああ");
	font->End(graphics.Get_device_context());

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
	DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[0])
		* DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };

	DirectX::XMMATRIX S4 = { DirectX::XMMatrixScaling(0.001f,0.001f,0.001f) };
	DirectX::XMMATRIX R4 = { DirectX::XMMatrixRotationRollPitchYaw(0,0,0) };
	DirectX::XMMATRIX T4 = { DirectX::XMMatrixTranslation(0,0,0) };

	DirectX::XMFLOAT4X4 world4;
	DirectX::XMStoreFloat4x4(&world4, C * S4 * R4 * T4);

	int clip_index{ 0 };
	int frame_index{ 0 };
	static float animation_tick{ 0 };

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
	animation::keyframe& keyframe{ animation.sequence.at(frame_index) };

	skinned_meshes->render(Graphics::Instance().Get_device_context(), world4, { 1,1,1,1 }, &keyframe);*/
}
