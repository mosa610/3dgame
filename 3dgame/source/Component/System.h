#pragma once
#include "Register.h"
#include "ComponentTransform.h"
#include "..//Graphics/RenderResourceContext.h"

class ISystem {
public:
    virtual void Initialize(Register& reg) = 0;
    virtual void update(Register& reg, float elapsed_time) = 0;
    virtual void render(Register& reg) = 0;
    virtual void finalize(Register& reg) = 0;
    virtual void drawDebugGUI(Register& reg) {}
    virtual ~ISystem() = default;

    void SetRenderContext( RenderResourceContext* context) { m_context = context; }

protected:
    RenderResourceContext* m_context;
};