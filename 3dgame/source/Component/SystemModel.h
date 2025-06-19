#pragma once

#include "System.h"
#include "..//Graphics/Graphics.h"
#include "..//Graphics/ModelResource.h"
#include "Entity.h"

class Register;

class SystemModel : public ISystem {
public:
    void Initialize(Register& reg) override;

    void update(Register& reg, float dt) override {}
    void render(Register& reg) override;

    void finalize(Register& reg) override {}

    void drawDebugGUI(Register& reg) override {}
private:
    void begin(Entity e, Register& reg, ID3D11Device* device, ID3D11DeviceContext* dc);
    void draw(Entity e, Register& reg, ID3D11DeviceContext* dc);
    void end(ID3D11DeviceContext* dc);

    void UpdateStructedBuffer(Entity e, Register& reg, ID3D11DeviceContext* dc);
};