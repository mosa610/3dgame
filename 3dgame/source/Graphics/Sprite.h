#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

//#include "Easing.h"

struct SpriteParameters
{
	DirectX::XMFLOAT2 pos = {};
	DirectX::XMFLOAT2 size = {1.0f,1.0f};
	float angle = 0.0f;
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	std::string name="NoneName";
};

// スプライト
class Sprite
{
public:
	Sprite();
	Sprite(const char* filename);

	Sprite(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view);
	~Sprite() 
	{
		if (!isLoadFile)
			shaderResourceView->Release();
		/*if (patternShaderResourceView)
			patternShaderResourceView->Release();*/
	}

	void InitPattern(ID3D11Device* device, const wchar_t* filename);

	struct Vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

	// 描画実行
	void Render(ID3D11DeviceContext *dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;
	void Render(ID3D11DeviceContext* dc,
		DirectX::XMFLOAT2 pos,
		DirectX::XMFLOAT2 endpos,
		DirectX::XMFLOAT2 size,
		DirectX::XMFLOAT2 endsize,
		float angle,
		DirectX::XMFLOAT4 color ) const;

	void Render(ID3D11DeviceContext* dc,
		DirectX::XMFLOAT2 pos,
		DirectX::XMFLOAT2 scale,
		float angle,
		DirectX::XMFLOAT4 color) const;



	void RenderCenter(ID3D11DeviceContext* dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;

	void RenderCenter(ID3D11DeviceContext* dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r[4], float g[4], float b[4], float a[4]) const;
	void RenderCenter(ID3D11DeviceContext* dc,
		SpriteParameters params,
		DirectX::XMFLOAT4 color[4]) const;
	void RenderCenter(ID3D11DeviceContext* dc,
		SpriteParameters params,
		DirectX::XMFLOAT2 cutin,
        DirectX::XMFLOAT2 cutout,
		DirectX::XMFLOAT4 color[4]) const;

	void RenderCenter(ID3D11DeviceContext* dc,
		DirectX::XMFLOAT2 pos,
		DirectX::XMFLOAT2 scale,
		float angle,
		DirectX::XMFLOAT4 color) const;

	void Render(ID3D11DeviceContext* dc,
		SpriteParameters params) const;

	void Render(ID3D11DeviceContext* dc,
			float dx, float dy,
			float dw, float dh,
			float sx, float sy,
			float sw, float sh,
			float angle,
			float r[4], float g[4], float b[4], float a[4]) const;


	void Render(ID3D11DeviceContext* dc,
			SpriteParameters params,
			DirectX::XMFLOAT4 color[4]) const;

	void RenderCenter(ID3D11DeviceContext* dc,
		SpriteParameters params) const;

	void RenderPatternSprite(ID3D11DeviceContext* dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r, float g, float b, float a) const;
	void RenderPatternSprite(ID3D11DeviceContext* dc,
		SpriteParameters params) const;
	void RenderPatternSprite(ID3D11DeviceContext* dc,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle,
		float r[4], float g[4], float b[4], float a[4]) const;
	void RenderPatternSprite(ID3D11DeviceContext* dc,
		SpriteParameters params,
		DirectX::XMFLOAT4 color[4]) const;

	void ImGuiRender(SpriteParameters& params);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh,
		float r, float g, float b, float a, float angle/*degree*/, float sx, float sy, float sw, float sh);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

	void textout(ID3D11DeviceContext* immediate_context, std::string s,
		float x, float y, float w, float h, DirectX::XMFLOAT4 color);

	// テクスチャ幅取得
	int GetTextureWidth() const { return textureWidth; }

	// テクスチャ高さ取得
	int GetTextureHeight() const { return textureHeight; }

	void UpdateScrollData(ID3D11DeviceContext* dc,DirectX::XMFLOAT2 scrollSpeed, float time, float alpha, DirectX::XMFLOAT2 uv = {});
private:
	struct ScrollData
	{
		DirectX::XMFLOAT2 scrollSpeed = { 0.0f,0.0f };
		float time = 0.0f;
		float alpha = 1.0f;
		DirectX::XMFLOAT2 uvScale = { 1.0f,1.0f };
		DirectX::XMFLOAT2 dummy;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			patternPixelShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				ScrollDataBuffer;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	patternShaderResourceView;
	D3D11_TEXTURE2D_DESC texture2d_desc;
	D3D11_TEXTURE2D_DESC patternTexture2d_desc;

	int textureWidth = 0;
	int textureHeight = 0;

	int patternTextureWidth;
	int patternTextureHeight;
	bool isLoadFile = true;
};