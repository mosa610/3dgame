#pragma once
#include <string>
#include "..//Scene.h"

struct ComponentScene
{
    ComponentScene(const std::string& name = "", Scene* scene = nullptr) : scene_name(name), scene(scene) {}
    std::string scene_name;
    float timer;
    Scene*  scene;
};