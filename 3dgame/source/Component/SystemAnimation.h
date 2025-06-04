#pragma once
#include "System.h"
#include "ComponentModel.h"
#include "ComponentAnimation.h"

class SystemAnimation : public ISystem
{
public:
    void Initialize(Register& reg) override {
        
    }
    void update(Register& reg, float elapsed_time) override;
    void render(Register& reg) override {}

    void finalize(Register& reg) override {}

    void drawDebugGUI(Register& reg) override {}

    void ComputeAnimation(Entity e, Register& reg, ComponentModel& model, ComponentAnimation& c_animation);
};