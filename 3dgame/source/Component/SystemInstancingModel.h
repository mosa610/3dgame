#pragma once

#include "System.h"

class SystemInstancingModel : public ISystem
{
public:
    void Initialize(Register& reg) override;
    void update(Register& reg, float elapsed_time) override;
    void render(Register& reg) override;
    void finalize(Register& reg) override {}

    void drawDebugGUI(Register& reg) override {}
private:
    void begin(Register& reg, ID3D11Device* device, ID3D11DeviceContext* dc);
    void draw(Register& reg, ID3D11DeviceContext* dc);
    void end(Register& reg, ID3D11DeviceContext* dc);

    void UpdateStructedBuffer(Entity e, Register& reg, ID3D11DeviceContext* dc);

    void UpdateInstancingBuffers(Register& reg, ID3D11DeviceContext* dc, Entity scene_entity);
};