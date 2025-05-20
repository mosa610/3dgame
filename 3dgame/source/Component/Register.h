#pragma once
#include "Entity.h"
#include "SparseSet.h"
#include "ComponentPool.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <queue>

class Register
{
public:
	Register() {}
	~Register() {}

public:
	Entity createEntity() { 
		if (!free_entities.empty()) {
			Entity e = free_entities.front();
			free_entities.pop();
			return e;
		}
		return next_entity++;
	}

	void destroyEntity(Entity e) {
		free_entities.push(e);
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

	// 対象のコンポーネントを持つ全てのEntity取得
	template<typename T>
	std::vector<Entity> view() {
		return getSet<T>().entities();
	}

	void removeEntity(Entity e) {
		for (auto& [type, pool] : component_pools) {
			// 動的キャストして remove を試みる
			auto remover = dynamic_cast<IComponentPoolRemovable*>(pool.get());
			if (remover) {
				remover->remove(e);
			}
		}
		destroyEntity(e);
	}

private:
	Entity next_entity = 1;
	std::queue<Entity> free_entities;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> component_pools;

	template<typename T>
	ComponentPool<T>& getSet() {
		std::type_index type = typeid(T);
		if(component_pools.count(type) == 0) {
            component_pools[type] = std::make_unique<ComponentPool<T>>();
        }
        return *static_cast<ComponentPool<T>*>(component_pools[type].get());
	}
};