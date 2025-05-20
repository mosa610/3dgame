#pragma once
#include "Entity.h"
#include "SparseSet.h"

class IComponentPool {
public:
    virtual ~IComponentPool() = default;
};

class IComponentPoolRemovable : public IComponentPool {
public:
    virtual void remove(Entity e) = 0;
};

template<typename T>
class ComponentPool : public IComponentPoolRemovable {
public:
    SparseSet<T> set;

    void add(Entity e, const T& value) {
        set.add(e, value);
    }

    bool has(Entity e) const {
        return set.has(e);
    }

    T& get(Entity e) {
        return set.get(e);
    }

    const std::vector<Entity>& entities() const {
        return set.entities();
    }

    void remove(Entity e) override {
        set.remove(e);
    }
};