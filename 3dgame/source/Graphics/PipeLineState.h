#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <cstring>
#include <string>
#include <unordered_map>
#include <memory>
#include "GraphicsState.h"

using namespace Microsoft::WRL;

struct PipeLineState
{
private:
    // 各ステート
   ComPtr<ID3D11RasterizerState> rasterizer_state;
   ComPtr<ID3D11BlendState> blend_state;
   ComPtr<ID3D11DepthStencilState> depth_stencil_state; 
   ComPtr<ID3D11SamplerState> sampler_state;
public:

   SAMPLER_STATE sampler = SAMPLER_STATE::ALL;
   DEPTH_STATE depth = DEPTH_STATE::ZT_ON_ZW_ON;
   BLEND_STATE blend = BLEND_STATE::NONE;
   RASTER_STATE raster = RASTER_STATE::CULL_NONE;

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
	uint32_t id = 0;

	SAMPLER_STATE sampler = SAMPLER_STATE::ALL;
	DEPTH_STATE depth = DEPTH_STATE::ZT_ON_ZW_ON;
	BLEND_STATE blend = BLEND_STATE::NONE;
	RASTER_STATE raster = RASTER_STATE::CULL_NONE;

	std::string vs_path;	//	頂点シェーダーファイルパス
	std::string hs_path;	//	ハルシェーダーファイルパス
	std::string ds_path;	//	ドメインシェーダーファイルパス
	std::string gs_path;	//	ジオメトリシェーダーファイルパス
	std::string ps_path;	//	ピクセルシェーダーファイルパス
	D3D_PRIMITIVE_TOPOLOGY primitive_toporogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	//	図形指定
	D3D11_INPUT_ELEMENT_DESC* input_element_desc;
};

class PipelineManager {
private:
	PipelineManager() {}
	~PipelineManager() {}
public:
	static PipelineManager& instance() {
		static PipelineManager instance;
		return instance;
	}

	void add(uint32_t id, std::shared_ptr<PipeLineState> state) {
		if (pipeline_states.find(id) == pipeline_states.end())
		{
			pipeline_states.emplace(id, state);
		}
	}

	PipeLineState* get(uint32_t id) {
		auto it = pipeline_states.find(id);
		if (it != pipeline_states.end())
		{
			return it->second.get();
		}
		return nullptr;
	}

	bool addPipelineState(ID3D11Device* device, PipelineStateDesc desc, D3D11_INPUT_ELEMENT_DESC input_element_desc[] = nullptr, UINT element_count = 0);

	bool setPipelineState(uint32_t id, ID3D11DeviceContext* dc);

private:
	std::unordered_map<uint32_t, std::shared_ptr<PipeLineState>> pipeline_states;
};