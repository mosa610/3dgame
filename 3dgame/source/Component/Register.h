#pragma once
#include "Entity.h"
#include "SparseSet.h"
#include "ComponentPool.h"

#include <unordered_map>
#include <typeindex>
#include <memory>

class Register
{
public:
	Register() {}
	~Register() {}

public:
	Entity createEntity() { 
		return nextEntity++;
	}

	template<typename T>
	void addComponent(Entity entity, const T& value) {
		getSet<T>().add(entity, value);
	}

	template<typename T>
	bool hasComponent(Entity e) {
		return getSet<T>().has(e);
	}

	template<typename T>
	T& getComponent(Entity e) {
		return getSet<T>().get(e);
	}

	template<typename T>
	void removeComponent(Entity e) {
		getSet<T>().remove(e);
	}

	template<typename T>
	std::vector<Entity> view() {
		return getSet<T>().entities();
	}

	void removeEntity(Entity e) {
		for (auto& [type, pool] : componentPools) {
			// 動的キャストして remove を試みる
			auto remover = dynamic_cast<IComponentPoolRemovable*>(pool.get());
			if (remover) {
				remover->remove(e);
			}
		}
	}

private:
	Entity nextEntity = 1;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> componentPools;

	template<typename T>
	ComponentPool<T>& getSet() {
		std::type_index type = typeid(T);
		if(componentPools.count(type) == 0) {
            componentPools[type] = std::make_unique<ComponentPool<T>>();
        }
        return *static_cast<ComponentPool<T>*>(componentPools[type].get());
	}
};