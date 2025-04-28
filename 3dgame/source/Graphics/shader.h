#pragma once
#include "../misc.h"
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include "render_context.h"
#include "Model.h"

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
    ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);


HRESULT create_hs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11HullShader** hull_shader);
HRESULT create_ds_from_cso(ID3D11Device* device, const char* cso_name, ID3D11DomainShader** domain_shader);
HRESULT create_gs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader);
HRESULT create_gs_with_streamout_from_cso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader, D3D11_SO_DECLARATION_ENTRY* streamout_declaration_desc, UINT num_entry, UINT* buffer_strides, UINT num_strides, UINT rasterized_stream);
HRESULT create_cs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader);

template<typename T>
Microsoft::WRL::ComPtr<ID3D11Buffer> create_constant_buffer
(Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer, T t, ID3D11Device* device);

class Shader
{
public:
	Shader() {}
	virtual ~Shader() {}

	// ï`âÊäJén
	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc, Model* model, const char* pipline_state_name) = 0;

	// ï`âÊ
	virtual void Draw(ID3D11DeviceContext* dc, Model* model, float alpha = 1) = 0;

	// ï`âÊèIóπ
	virtual void End(ID3D11DeviceContext* context) = 0;
};
