#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <cstring>
using namespace Microsoft::WRL;

struct PipeLineState
{
public:
    // 各ステート
   ComPtr<ID3D11RasterizerState> rasterizer_state;
   ComPtr<ID3D11BlendState> blend_state;
   ComPtr<ID3D11DepthStencilState> depth_stencil_state; 
   ComPtr<ID3D11SamplerState> sampler_state;

   ComPtr<ID3D11PixelShader> pixel_shader;
   ComPtr<ID3D11VertexShader> vertex_shader;
   ComPtr<ID3D11HullShader> hull_shader;
   ComPtr<ID3D11GeometryShader> geometry_shader;
   ComPtr<ID3D11ComputeShader> compute_shader;
   ComPtr<ID3D11DomainShader> domain_shader;

   ComPtr<ID3D11InputLayout> input_layout;
   D3D_PRIMITIVE_TOPOLOGY primitive_toporogy;
};

struct PipelineStateDesc
{
	std::string name;		//	シェーダーセット名

	std::string vs_path;	//	頂点シェーダーファイルパス
	std::string hs_path;	//	ハルシェーダーファイルパス
	std::string ds_path;	//	ドメインシェーダーファイルパス
	std::string gs_path;	//	ジオメトリシェーダーファイルパス
	std::string ps_path;	//	ピクセルシェーダーファイルパス
	D3D_PRIMITIVE_TOPOLOGY primitive_toporogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	//	図形指定
};