#pragma once
#include "System.h"

class SystemGbuffer : public ISystem
{
public:
    void Initialize(Register& reg) override;
    void update(Register& reg, float elapsed_time) override;
    void render(Register& reg) override;
};