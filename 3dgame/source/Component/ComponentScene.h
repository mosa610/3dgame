#pragma once
#include <string>
#include "..//Scene.h"
#include "ComponentInstancing.h"

struct ComponentScene
{
    ComponentScene(const std::string& name = "", Scene* scene = nullptr) : scene_name(name), scene(scene) {}
    std::string scene_name;
    float timer;
    Scene*  scene;
    std::vector<int> instance_model_count;
    std::vector<InstanceLocation> deferred_instance_location;
};