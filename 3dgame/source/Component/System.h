#pragma once
#include "Register.h"
#include "ComponentTransform.h"

class ISystem {
public:
    virtual void update(Register& reg, float dt) = 0;
    virtual void render(Register& reg) = 0;
    virtual ~ISystem() = default;
};

class TransformSystem : public ISystem {
public:
    void update(Register& reg, float dt) override {
        for (Entity e : reg.view<ComponentTransform>()) {
            auto& t = reg.getComponent<ComponentTransform>(e);
            t.position.x += dt; // ˆÚ“®‚Ì—á
        }
    }

    void render(Register&) override {} // •`‰æ‚Í‚µ‚È‚¢
};