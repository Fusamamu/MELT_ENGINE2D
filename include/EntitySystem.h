#pragma once
#include "EntityRegistry.h"

class Engine;

struct SpriteSystem
{
    Engine* engine;
    void Update(const std::size_t& _entityCount, EntityRegistry& _registry) const;
    void Render(const std::size_t& _entityCount, EntityRegistry& _registry, SDL_Renderer* _render) const;
};

struct PlayerMovementSystem
{
    Engine* engine;

    const Uint8* keys;

    bool wKeyPressedPrevFrame = false;
    bool aKeyPressedPrevFrame = false;
    bool sKeyPressedPrevFrame = false;
    bool dKeyPressedPrevFrame = false;

    void Update(const std::size_t& _entityCount, EntityRegistry& _registry);
};