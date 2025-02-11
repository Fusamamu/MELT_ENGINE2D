#include "EntityManager.h"

EntityManager::EntityManager(Engine* _engine) : EntityCount(0), m_Engine(_engine)
{
    m_SpriteSystem.engine         = m_Engine;
    m_PlayerMovementSystem.engine = m_Engine;
}

EntityManager::~EntityManager()
{

}

ENTITY EntityManager::CreateEntity()
{
    if(EntityCount >= MAX_ENTITY_COUNT)
        return -1;
    ++EntityCount;
    return EntityCount;
}

void EntityManager::ProcessInput()
{

}

void EntityManager::Update()
{
    m_SpriteSystem        .Update(EntityCount, Registry);
    m_PlayerMovementSystem.Update(EntityCount, Registry);
}

void EntityManager::Render(SDL_Renderer* _render)
{
    m_SpriteSystem.Render(EntityCount, Registry, _render);
}

void EntityManager::Reset()
{
    EntityCount = 0;
    Registry.Reset();
}