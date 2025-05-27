#pragma once

#include "System.h"

class SystemModel : public ISystem {
public:
    void Initialize(Register& reg) override {}
    void update(Register& reg, float dt) override {}
    void render(Register& reg) override {}
};