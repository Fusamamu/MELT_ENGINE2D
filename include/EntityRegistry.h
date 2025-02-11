#pragma once
#include <iostream>
#include <unordered_map>
#include <EntityComponent.h>

using ENTITY = std::size_t;

struct EntityRegistry
{
    std::unordered_map<ENTITY, TransformComponent> Transforms;
    std::unordered_map<ENTITY, TileComponent>      Tiles;
    std::unordered_map<ENTITY, SpriteComponent>    Sprites;
    std::unordered_map<ENTITY, InputComponent>     Inputs;

    void Reset()
    {
        Transforms.clear();
        Sprites.clear();
    }
};
