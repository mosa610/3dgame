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

            DirectX::XMMATRIX S = { DirectX::XMMatrixScaling(t.scale.x, t.scale.y, t.scale.z) };
            DirectX::XMMATRIX R = { DirectX::XMMatrixRotationRollPitchYaw(t.rotation.x, t.rotation.y, t.rotation.z) };
            DirectX::XMMATRIX T = { DirectX::XMMatrixTranslation(t.position.x, t.position.y, t.position.z) };

            DirectX::XMStoreFloat4x4(&t.world_transform, S * R * T);
        }
    }

    void render(Register&) override {} // •`‰æ‚Í‚µ‚È‚¢
};