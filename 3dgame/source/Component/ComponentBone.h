#pragma once
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>

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
    std::unordered_map<int, std::vector<Bone>> bones;
};