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
	uint32_t id = 0;

	std::string vs_path;	//	頂点シェーダーファイルパス
	std::string hs_path;	//	ハルシェーダーファイルパス
	std::string ds_path;	//	ドメインシェーダーファイルパス
	std::string gs_path;	//	ジオメトリシェーダーファイルパス
	std::string ps_path;	//	ピクセルシェーダーファイルパス
	D3D_PRIMITIVE_TOPOLOGY primitive_toporogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	//	図形指定
};

class PiplineManager {
private:
	PiplineManager() {}
	~PiplineManager() {}
public:
	static PiplineManager& instance() {
		static PiplineManager instance;
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

	bool AddPipelineState(ID3D11Device* device, PipelineStateDesc desc)
	{
		auto it = pipeline_states.find(desc.id);
		if (it != pipeline_states.end())
			return false;

		// descに各ステートの使用するステートをかんりするIdのような変数を作ってIdによってステートをGraphicStateから取り出せる関数を作りstateに設定もしIDがなければDefaultのステートを呼ぶようにする。

		//const std::map<std::string, buffer_view>& vertex_buffer_views{ meshes.at(0).primitives.at(0).vertex_buffer_views };
		D3D11_INPUT_ELEMENT_DESC input_element_desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "JOINTS",   0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		PipeLineState state;
		if (!desc.vs_path.empty())
			create_vs_from_cso(device, desc.vs_path.data(), state.vertex_shader.ReleaseAndGetAddressOf(), state.input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
		if (!desc.hs_path.empty())
			create_hs_from_cso(device, desc.hs_path.data(), state.hull_shader.ReleaseAndGetAddressOf());
		if (!desc.ds_path.empty())
			create_ds_from_cso(device, desc.ds_path.data(), state.domain_shader.ReleaseAndGetAddressOf());
		if (!desc.gs_path.empty())
			create_gs_from_cso(device, desc.gs_path.data(), state.geometry_shader.ReleaseAndGetAddressOf());
		if (!desc.ps_path.empty())
			create_ps_from_cso(device, desc.ps_path.data(), state.pixel_shader.ReleaseAndGetAddressOf());
		state.primitive_toporogy = desc.primitive_toporogy;

		pipeline_states.insert_or_assign(desc.id, state);
		return true;
	}

private:
	std::unordered_map<uint32_t, std::shared_ptr<PipeLineState>> pipeline_states;
};