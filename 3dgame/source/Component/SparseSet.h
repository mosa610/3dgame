#pragma once

#include <vector>

using Entity = std::uint32_t;

template<typename Component>
class SparseSet
{
    std::vector<Entity> dense;
    std::vector<size_t> sparse;
    std::vector<Component> components;

public:
    void add(Entity entity, const Component& component)
    {
        if (entity >= sparse.size())
        {
            sparse.resize(entity + 1, static_cast<size_t>(-1));
        }

        if (has(entity)) return;

        sparse[entity] = dense.size();
        dense.push_back(entity);
        components.push_back(component);
    }

    void remove(Entity entity)
    {
        if(!has(entity)) return;

        size_t index = sparse[entity];
        size_t last_index = dense.size() - 1;
        Entity last_entity = dense[last_index];

        // ƒXƒƒbƒv‚µ‚Äíœ
        dense[index] = last_entity;
        components[index] = components[last_index];
        sparse[last_entity] = index;

        dense.pop_back();
        components.pop_back();
        sparse[entity] = static_cast<size_t>(-1);
    }

    Component* et(Entity entity)
    {
        if (!has(entity)) return nullptr;
        return &components[sparse[entity]];
    }

    bool has(Entity entity) const
    {
        return entity < sparse.size() && sparse[entity] != static_cast<size_t>(-1);
    }

    auto begin() { return dense.begin(); }
    auto end() { return dense.end(); }

    Component& atDenseIndex(size_t i)
    {
        return conponents[i];
    }

    Entity entityAtDenseIndx(size_t i)
    {
        return dense[i];
    }

    const std::vector<Entity>& entities() const { return dense; }

    size_t size() const { return dense.size(); }
};