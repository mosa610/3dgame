#pragma once
#include "..//Graphics/ModelResource.h"

struct ComponentModel
{
    ComponentModel(const char* file_name) : file_name(file_name) {}
    const char* file_name;
    std::shared_ptr<ModelResource> resource;
};