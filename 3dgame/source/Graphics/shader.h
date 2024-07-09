#pragma once
#include "../misc.h"
#include <d3d11.h>
#include <directxmath.h>
#include "render_context.h"
#include "model.h"

HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
    ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);


//class Shader
//{
//public:
//	Shader() {}
//	virtual ~Shader() {}
//
//	// ï`âÊäJén
//	virtual void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) = 0;
//
//	// ï`âÊ
//	virtual void Draw(ID3D11DeviceContext* dc, const Model* model) = 0;
//
//	// ï`âÊèIóπ
//	virtual void End(ID3D11DeviceContext* context) = 0;
//};
