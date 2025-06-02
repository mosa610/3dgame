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

private:
	// RenderContextクラスを作成し中にSceneのRTVなどを持たせる（GbufferのRTVなどはSystemで作る）RenderContextにWindowのサイズも持たせておけばサイズが変わった際に作り直せる。
	// SceneのRTVを持ったRenderContextを作成すればSceneのRTVが再作成されたときにSpriteに設定しなおさなくてもよくなるかも？
};