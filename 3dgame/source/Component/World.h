#pragma once

#include "Register.h"
#include "System.h"
#include "..//Graphics/RenderGraph.h"


class World
{
public:
    World();
    ~World();
public:
    void initialize();

    void update(float dt);

    void render();

    void setRenderContext(RenderResourceContext* context);

    void drawDebugGUI();

    template<typename T, typename... Args>
    void addSystem(Args&&... args);

    template<typename T>
    T* getSystem();

    template<typename T>
    bool hasSystem() const;

    Register& getRegister();

    RenderGraph& getRenderGraph();

private:
    void registerComponentCallback();

    void setupRenderPasses();

private:

    Register reg;
    RenderGraph render_graph;
    std::vector<std::unique_ptr<ISystem>> systems;
    std::unordered_map<std::type_index, ISystem*> systemCache;
};

template<typename T, typename ...Args>
inline void World::addSystem(Args && ...args) {
    //systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));

    auto system = std::make_unique<T>(std::forward<Args>(args)...);
    T* systemPtr = system.get();

    systems.push_back(std::move(system));

    // キャッシュに追加（オプション）
    systemCache[std::type_index(typeid(T))] = systemPtr;
}

template<typename T>
inline T* World::getSystem() {
    auto it = systemCache.find(std::type_index(typeid(T)));
    if (it != systemCache.end()) {
        return static_cast<T*>(it->second);
    }
    return nullptr;
}

template<typename T>
inline bool World::hasSystem() const {
    return systemCache.find(std::type_index(typeid(T))) != systemCache.end();
}
