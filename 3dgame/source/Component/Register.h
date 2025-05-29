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
	Entity createEntity(const std::string& name = "") {
		if (!free_entities.empty()) {
			Entity e = free_entities.front();
			free_entities.pop();
			if (!name.empty()) {
				nameToEntity[name] = e;
				entityToName[e] = name;
			}
			return e;
		}
		Entity e = next_entity++;
		if (!name.empty()) {
			nameToEntity[name] = e;
			entityToName[e] = name;
		}
		return e;
	}

	Entity getEntityByName(const std::string& name) const {
		auto it = nameToEntity.find(name);
		if (it != nameToEntity.end()) return it->second;
		return INVALID_ENTITY; // または例外を投げる
	}

	std::string getEntityName(Entity e) const {
		auto it = entityToName.find(e);
		if (it != entityToName.end()) return it->second;
		return "";
	}

	void destroyEntity(Entity e) {
		free_entities.push(e);
	}

	template<typename T>
	void addComponent(Entity entity, const T& value) {
		if(hasComponent<T>(entity)) return;

		getSet<T>().add(entity, value);

		auto it = onAddedCallbacks.find(typeid(T));
		if (it != onAddedCallbacks.end()) {
			void* ptr = &getSet<T>().get(entity);
			it->second(entity, ptr);
		}
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
	/*template<typename T>
	std::vector<Entity> view() {
		return getSet<T>().entities();
	}*/

	template<typename... Components>
	std::vector<Entity> view() {
		std::vector<Entity> result;

		// 最も小さいSetをベースにしてループ（SparseSetの前提）
		const auto& baseSet = getSet<std::tuple_element_t<0, std::tuple<Components...>>>();
		for (Entity e : baseSet.entities()) {
			if ((getSet<Components>().has(e) && ...)) {
				result.push_back(e);
			}
		}

		return result;
	}

	template<typename T>
    void setOnComponentAdded(std::function<void(Register&, Entity, T&)> callback) {
		onAddedCallbacks[typeid(T)] = [this, callback](Entity e, void* ptr) {
			callback(*this, e, *static_cast<T*>(ptr));
			};
    }

	void removeEntity(Entity e) {
		for (auto& [type, pool] : component_pools) {
			// 動的キャストして remove を試みる
			auto remover = dynamic_cast<IComponentPoolRemovable*>(pool.get());
			if (remover) {
				remover->remove(e);
			}
		}

		auto it = entityToName.find(e);
		if (it != entityToName.end()) {
			nameToEntity.erase(it->second);
			entityToName.erase(it);
		}

		destroyEntity(e);
	}

private:
	Entity next_entity = 1;
	std::queue<Entity> free_entities;
	std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> component_pools;

	std::unordered_map<std::string, Entity> nameToEntity;
	std::unordered_map<Entity, std::string> entityToName;

	std::unordered_map<std::type_index, std::function<void(Entity, void*)>> onAddedCallbacks;

	template<typename T>
	ComponentPool<T>& getSet() {
		std::type_index type = typeid(T);
		if(component_pools.count(type) == 0) {
            component_pools[type] = std::make_unique<ComponentPool<T>>();
        }
        return *static_cast<ComponentPool<T>*>(component_pools[type].get());
	}
};