#pragma once
#include "System.h"

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

class SystemGbufferRendering : public ISystem
{
public:
    void Initialize(Register& reg) override;
    void update(Register& reg, float elapsed_time) override;
    void render(Register& reg) override;
	void finalize(Register& reg) override {
		for (int i = GB_BaseColor; i < GB_Max; ++i)
		{
			g_buffer_shader_resource_view[i].Reset();
		}
	}

	void drawDebugGUI(Register& reg) override;

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_buffer_render_target_view[GB_Max];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_buffer_shader_resource_view[GB_Max];
	// RenderContextクラスを作成し中にSceneのRTVなどを持たせる（GbufferのRTVなどはSystemで作る）RenderContextにWindowのサイズも持たせておけばサイズが変わった際に作り直せる。
	// SceneのRTVを持ったRenderContextを作成すればSceneのRTVが再作成されたときにSpriteに設定しなおさなくてもよくなるかも？
};