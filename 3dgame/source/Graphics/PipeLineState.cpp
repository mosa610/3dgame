#include "PipeLineState.h"
#include "shader.h"

bool PipelineManager::addPipelineState(ID3D11Device* device, PipelineStateDesc desc)
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

	std::shared_ptr<PipeLineState> state = std::make_shared<PipeLineState>();

	state->raster = desc.raster;
	state->blend = desc.blend;
    state->depth = desc.depth;
	state->sampler = desc.sampler;


	if (!desc.vs_path.empty())
		create_vs_from_cso(device, desc.vs_path.data(), state->vertex_shader.ReleaseAndGetAddressOf(), state->input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
	if (!desc.hs_path.empty())
		create_hs_from_cso(device, desc.hs_path.data(), state->hull_shader.ReleaseAndGetAddressOf());
	if (!desc.ds_path.empty())
		create_ds_from_cso(device, desc.ds_path.data(), state->domain_shader.ReleaseAndGetAddressOf());
	if (!desc.gs_path.empty())
		create_gs_from_cso(device, desc.gs_path.data(), state->geometry_shader.ReleaseAndGetAddressOf());
	if (!desc.ps_path.empty())
		create_ps_from_cso(device, desc.ps_path.data(), state->pixel_shader.ReleaseAndGetAddressOf());
	state->primitive_toporogy = desc.primitive_toporogy;

	pipeline_states.insert_or_assign(desc.id, state);
	return true;
}

bool PipelineManager::setPipelineState(uint32_t id, ID3D11DeviceContext* dc)
{
	auto it = pipeline_states.find(id);
	if (it != pipeline_states.end())
	{
		GraphicsState& gs = GraphicsState::GetInstance();
		gs.SetBlendState(dc, it->second->blend);
		gs.SetDepthStencilState(dc, it->second->depth);
		gs.SetRasterizerState(dc, it->second->raster);
		gs.SetSamplerState(dc, it->second->sampler);

		if(it->second->input_layout)
            dc->IASetInputLayout(it->second->input_layout.Get());
		if (it->second->vertex_shader)
			dc->VSSetShader(it->second->vertex_shader.Get(), nullptr, 0);
		if(it->second->hull_shader)
            dc->HSSetShader(it->second->hull_shader.Get(), nullptr, 0);
		if (it->second->domain_shader)
			dc->DSSetShader(it->second->domain_shader.Get(), nullptr, 0);
		if (it->second->geometry_shader)
			dc->GSSetShader(it->second->geometry_shader.Get(), nullptr, 0);
		if (it->second->pixel_shader)
			dc->PSSetShader(it->second->pixel_shader.Get(), nullptr, 0);
		if (it->second->compute_shader)
			dc->CSSetShader(it->second->compute_shader.Get(), nullptr, 0);
	}
	return true;
}
