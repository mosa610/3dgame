#pragma once
#include "..//Graphics/LightData.h"
#include <vector>

struct directional_light_constants
{
    directional_lights directional_light;
    int use_shadow{ false }; //影を使うかどうか
    float shadow_attenuation{ 0.0f }; //影の色
    float shadow_bias{ 0.0f }; // 深度バイアス
    float shadow_dummy;
    DirectX::XMFLOAT4X4 light_view_projection;
};

struct ComponentLight
{
    std::vector<directional_light_constants> directional_lights;
};