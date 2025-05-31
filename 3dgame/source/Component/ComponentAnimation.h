#pragma once

struct ComponentAnimation
{
    ComponentAnimation(int index = 0): animation_index(index)
    {}
    int animation_index = 0;
    float animation_time = 0.0f;
};