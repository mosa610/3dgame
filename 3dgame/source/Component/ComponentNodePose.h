#pragma once

#include <DirectXMath.h>
#include <vector>

struct NodePose
{
    DirectX::XMFLOAT3	position = { 0, 0, 0 };
    DirectX::XMFLOAT4	rotation = { 0, 0, 0, 1 };
    DirectX::XMFLOAT3	scale = { 1, 1, 1 };
};

struct ComponentNodePose
{
    std::vector<NodePose> poses;
};