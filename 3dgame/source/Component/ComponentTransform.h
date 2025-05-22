#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct ComponentTransform
{
    ComponentTransform(XMFLOAT3 position = {}, XMFLOAT3 rotation = {}, XMFLOAT3 scale = {1, 1, 1}) : position(position), rotation(rotation), scale(scale) {}
    XMFLOAT3 position;
    XMFLOAT3 rotation;
    XMFLOAT3 scale;

    XMFLOAT4X4 world_transform;
};