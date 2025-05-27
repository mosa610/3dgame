#pragma once
#include <DirectXMath.h>
#include <vector>

class Node;

struct Bone
{
    int						nodeIndex;
    DirectX::XMFLOAT4X4		offsetTransform;
    Node* node = nullptr;
};

class ComponentBone
{
public:
    std::vector<Bone> bones;
};