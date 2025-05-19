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
    void Insert(Entity entity, const Component& component)
    {
        if (entity >= sparse.size())
        {
            sparse.resize(entity + 1, static_cast<size_t>(-1));
        }

        if (Contains(entity)) return;

        sparse[entity] = dense.size();
        dense.push_back(entity);
        components.push_back(component);
    }

    void Erase(Entity entity)
    {
        if(!Contains(entity)) return;

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

    Component* Get(Entity entity)
    {
        if (!Contains(entity)) return nullptr;
        return &components[sparse[entity]];
    }

    bool Contains(Entity entity) const
    {
        return entity < sparse.size() && sparse[entity] != static_cast<size_t>(-1);
    }

    auto begin() { return dense.begin(); }
    auto end() { return dense.end(); }

    Component& AtDenseIndex(size_t i)
    {
        return conponents[i];
    }

    Entity EntityAtDenseIndx(size_t i)
    {
        return dense[i];
    }

    size_t Size() const { return dense.size(); }
};