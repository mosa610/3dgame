#pragma once
#include "System.h"
#include "Register.h"

class SystemTransform : public ISystem {
public:
    void Initialize(Register& reg) override {}
    void update(Register& reg, float dt) override;

    void render(Register&) override {} // •`‰æ‚Í‚µ‚È‚¢

    void finalize(Register& reg) override {}

    void drawDebugGUI(Register& reg) override {}
};