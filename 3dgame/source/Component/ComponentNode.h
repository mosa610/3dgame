#pragma once

#include <string>
#include <DirectXMath.h>
#include <memory>

struct Node
{
    ;
    std::string			name;
    int                 myIndex = -1;
    int					parentIndex = -1;
    int                 jointIndex = -1;
    DirectX::XMFLOAT3	position = { 0, 0, 0 };
    DirectX::XMFLOAT4	rotation = { 0, 0, 0, 1 };
    DirectX::XMFLOAT3	scale = { 1, 1, 1 };

    DirectX::XMFLOAT4X4	localTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    DirectX::XMFLOAT4X4	globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    DirectX::XMFLOAT4X4	worldTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

    Node* parent = nullptr;
    std::vector<Node*>	children;
};

struct ComponentNode
{
    std::vector<Node> nodes;
};