#pragma once

#include <memory>
#include <wrl.h>

#include "render_context.h"
#include "shader.h"
#include "Model.h"
#include "ModelResource.h"
#include "ConstantBuffer.h"

class ModelRenderer : public Shader
{
public:
    ModelRenderer(ID3D11Device* device, ID3D11DeviceContext* dc);
    ~ModelRenderer() override {}

    void Begin(ID3D11DeviceContext* dc, const RenderContext& rc, Model* model, const char* pipline_state_name = " ") override;
    void Draw(ID3D11DeviceContext* dc, Model* model, float alpha = 1.0f) override;
    void End(ID3D11DeviceContext* dc) override;

private:
    static constexpr int MAX_BONES = 512;

    struct MeshCB
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4X4 previous_world;
        float materialIndex{ -1 };
        float padding1;
        float skin{ -1 };
        float adjustalpha;
    };

    struct SkeletonCB
    {
        DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES];
    };

    /*StructedBuffer*/
    struct MaterialCB
    {
        DirectX::XMFLOAT3 emissiveColor = { 0,0,0 };
        int   alphaMode = ModelResource::AlphaMode::Opaque;
        float alphaCutOff = 0.5f;
        bool  doubleSided = false;
        ModelResource::PbrMetallicRoughness pbrMetallicRoughness;
        ModelResource::NormalTextureInfo    normalTexture;
        ModelResource::OcclusionTextureInfo occlusionTexture;
        ModelResource::TextureInfo          emissiveTexture;
    };

    ConstantBuffer<MeshCB>      _meshCB;
    ConstantBuffer<SkeletonCB>  _skeletonCB;
    StracturedBuffer<MaterialCB>_materialCB;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  _pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  _inputLayout;
};