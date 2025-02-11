#pragma once
#include <iostream>
#include <unordered_map>
#include "EntitySystem.h"

class Engine;

class EntityManager
{
public:
    explicit EntityManager(Engine* _engine);
    ~EntityManager();

    void ProcessInput();
    void Update();
    void Render(SDL_Renderer* _render);
    void Reset();

    ENTITY CreateEntity();

    const size_t MAX_ENTITY_COUNT = 1000;
    size_t EntityCount;

    EntityRegistry Registry;

private:
    Engine* m_Engine;

    SpriteSystem         m_SpriteSystem        {};
    PlayerMovementSystem m_PlayerMovementSystem{};
};