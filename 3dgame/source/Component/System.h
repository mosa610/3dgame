#pragma once
#include "Register.h"
#include "ComponentTransform.h"

class ISystem {
public:
    virtual void Initialize(Register& reg) = 0;
    virtual void update(Register& reg, float elapsed_time) = 0;
    virtual void render(Register& reg) = 0;
    virtual ~ISystem() = default;
};