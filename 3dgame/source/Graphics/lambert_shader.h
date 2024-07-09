//#pragma once
//
//#include <memory>
//#include <wrl.h>
//#include "shader.h"
//
//class LambertShader : public Shader
//{
//public:
//	LambertShader(ID3D11Device* device);
//	~LambertShader() override {}
//
//	void Begin(ID3D11DeviceContext* dc, const RenderContext& rc) override;
//	void Draw(ID3D11DeviceContext* dc, const Model* model) override;
//	void End(ID3D11DeviceContext* dc) override;
//
//private:
//	static const int MaxBones = 128;
//
//	struct CbScene
//	{
//		DirectX::XMFLOAT4X4	viewProjection;
//		DirectX::XMFLOAT4	lightDirection;
//	};
//
//	struct CbMesh
//	{
//		DirectX::XMFLOAT4X4	boneTransforms[MaxBones];
//	};
//
//	struct CbSubset
//	{
//		DirectX::XMFLOAT4	materialColor;
//	};
//
//
//	Microsoft::WRL::ComPtr<ID3D11Buffer>			scene_constant_buffer;
//	Microsoft::WRL::ComPtr<ID3D11Buffer>			mesh_constant_buffer;
//	Microsoft::WRL::ComPtr<ID3D11Buffer>			subset_constant_buffer;
//
//	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertex_shader;
//	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixel_shader;
//	Microsoft::WRL::ComPtr<ID3D11InputLayout>		input_layout;
//
//	Microsoft::WRL::ComPtr<ID3D11BlendState>		blend_state;
//	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizer_state;
//	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depth_stencil_state;
//
//	Microsoft::WRL::ComPtr<ID3D11SamplerState>		sampler_state;
//};