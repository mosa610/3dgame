#pragma once
#include "System.h"

class SystemSkymap : public ISystem
{
public:
    void Initialize(Register& reg) override;
    void update(Register& reg, float elapsed_time) override;
    void render(Register& reg) override;
    void finalize(Register& reg) override;
};