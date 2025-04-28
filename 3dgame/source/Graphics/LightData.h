#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

struct directional_lights
{
	DirectX::XMFLOAT4 direction{ 1.0f, -1.0f, -1.0f, 1.0f };
	DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

struct point_lights
{
	DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };
	DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };
	float range{ 0 };
	DirectX::XMFLOAT3 dummy;
};

struct spot_lights
{
	DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };
	DirectX::XMFLOAT4 direction{ 0, 0, 1, 0 };
	DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };
	float range{ 0 };
	float innerCorn{ DirectX::XMConvertToRadians(30) };
	float outerCorn{ DirectX::XMConvertToRadians(45) };
	float dummy;
};

struct hemisphere_lights
{
	DirectX::XMFLOAT4 sky_color{ 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 ground_color{ 0.0f, 0.0f, 1.0f, 1.0f };
	float weight{ 0.0f };
	DirectX::XMFLOAT3 dummy;
};