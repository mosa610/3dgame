#pragma once
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include "..//Graphics/ConstantBuffer.h"

class Node;

struct Bone
{
    int						nodeIndex;
    DirectX::XMFLOAT4X4		offsetTransform;
    Node* node = nullptr;
};

static constexpr int MAX_BONES = 512;
struct SkeletonCB
{
    DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES];
};

class ComponentBone
{
public:
    std::unordered_map<int, std::vector<Bone>> bones;
    ConstantBuffer<SkeletonCB> skeleton_constant_buffer;
};