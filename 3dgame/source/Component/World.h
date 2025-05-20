#pragma once

#include "Register.h"
#include "System.h"

class World
{
public:
    World() {}
    ~World() {}
public:
    void update(float dt) {
        for (auto& sys : systems) sys->update(reg, dt);
    }

    void render() {
        for (auto& sys : systems) sys->render(reg);
    }

    template<typename T, typename... Args>
    void addSystem(Args&&... args) {
        systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    Register& getRegister() { return reg; }

private:
    Register reg;
    std::vector<std::unique_ptr<ISystem>> systems;
};
