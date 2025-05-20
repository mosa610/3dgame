#pragma once
#include "Entity.h"
#include "SparseSet.h"

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
	std::vector<Entity> view() {
		return getSet<T>().entities();
	}

private:
	Entity nextEntity = 1;
	std::unordered_map<std::type_index, std::unique_ptr<void>> componentPools;

	template<typename T>
	SparseSet<T> getSet() {
		std::type_index type = typeid(T);
		if(componentPools.find(type) == 0) {
            componentPools[type] = std::make_unique<SparseSet<T>>();
        }
        return *static_cast<SparseSet<T>*>(componentPools[type].get());
	}
};