#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <cstring>
using namespace Microsoft::WRL;

struct PipeLineState
{
public:
    // �e�X�e�[�g
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
	std::string name;		//	�V�F�[�_�[�Z�b�g��

	std::string vs_path;	//	���_�V�F�[�_�[�t�@�C���p�X
	std::string hs_path;	//	�n���V�F�[�_�[�t�@�C���p�X
	std::string ds_path;	//	�h���C���V�F�[�_�[�t�@�C���p�X
	std::string gs_path;	//	�W�I���g���V�F�[�_�[�t�@�C���p�X
	std::string ps_path;	//	�s�N�Z���V�F�[�_�[�t�@�C���p�X
	D3D_PRIMITIVE_TOPOLOGY primitive_toporogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;	//	�}�`�w��
};