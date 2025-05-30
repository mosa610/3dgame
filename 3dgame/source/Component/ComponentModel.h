#pragma once
#include "..//Graphics/ModelResource.h"
#include "..//Graphics/ConstantBuffer.h"

struct MeshCB
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 previous_world;
    float materialIndex{ -1 };
    float padding1;
    float skin{ -1 };
    float adjustalpha;
};

struct ComponentModel
{
    ComponentModel(const char* file_name) : file_name(file_name) {}

    const char* file_name;
    std::shared_ptr<ModelResource> resource;
    ConstantBuffer<MeshCB>      mesh_constant_buffer;
};