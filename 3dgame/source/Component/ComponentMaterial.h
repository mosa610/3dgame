#pragma once

#include <wrl.h>
#include <DirectXMath.h>
#include "..//Graphics/ModelResource.h"

struct MaterialData
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

class ComponentMaterial
{
public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    material_resource_view;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                material_buffer;

    std::vector<ModelResource::Material::CBuffer>       material_dates;
};