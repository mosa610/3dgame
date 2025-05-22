#pragma once

#include "System.h"
#include "ComponentNode.h"

class SystemLoadModel : public ISystem
{
    void Initialize() override {
        for (Entity e : reg.view<ComponentNode>()) {
            auto& n = reg.getComponent<ComponentNode>(e);

            
        }
    }
    void Update() override {}
    void Finalize() override {}
};