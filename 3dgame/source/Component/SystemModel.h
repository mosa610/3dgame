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
    void render(Register& reg) override {
        ID3D11Device* device = Graphics::Instance().Get_device();
        ID3D11DeviceContext* dc = Graphics::Instance().Get_device_context();
        // begin
        {
            begin(reg, device, dc);
        }

        // draw
        {
            draw(reg, dc);
        }

        // end
        {
            end(reg, dc);
        }
    }

    void finalize(Register& reg) override {}

    void drawDebugGUI(Register& reg) override {}
private:
    void begin(Register& reg, ID3D11Device* device, ID3D11DeviceContext* dc);
    void draw(Register& reg, ID3D11DeviceContext* dc);
    void end(Register& reg, ID3D11DeviceContext* dc);

    void UpdateStructedBuffer(Entity e, Register& reg, ID3D11DeviceContext* dc);
};