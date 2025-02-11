#include "Engine.h"
#include "EntitySystem.h"

void SpriteSystem::Update(const std::size_t& _entityCount, EntityRegistry& _registry) const
{
    for(ENTITY _e = 1; _e <= _entityCount; ++_e)
    {
        if(_registry.Transforms.find(_e) != _registry.Transforms.end() &&
           _registry.Sprites   .find(_e) != _registry.Sprites.end())
        {
            _registry.Sprites[_e].dst.x = static_cast<int>(_registry.Transforms[_e].posX);
            _registry.Sprites[_e].dst.y = static_cast<int>(_registry.Transforms[_e].posY);
        }
    }
}

void SpriteSystem::Render(const std::size_t &_entityCount, EntityRegistry &_registry, SDL_Renderer *_render) const
{
    for(ENTITY _e = 1; _e <= _entityCount; ++_e)
    {
        if(_registry.Sprites.find(_e) != _registry.Sprites.end())
        {
            SpriteComponent& _sprite = _registry.Sprites[_e];

            if(_sprite.isAnimated)
            {
                if(_sprite.animationClips.empty())
                    return;
                _sprite.src = _sprite.animationClips[_sprite.clipIndex];

                _sprite.clipElapsed += engine->m_Time.deltaTime * _sprite.animationSpeed;
                if(_sprite.clipElapsed > 1)
                {
                    _sprite.clipIndex++;
                    if(_sprite.clipIndex > _sprite.animationClips.size() - 1)
                        _sprite.clipIndex = 0;

                    _sprite.clipElapsed = 0;
                }
            }

            SDL_RenderCopy(
                    _render,
                    _registry.Sprites[_e].texture,
                    &_registry.Sprites[_e].src,
                    &_registry.Sprites[_e].dst
            );
         }
    }
}

void PlayerMovementSystem::Update(const std::size_t &_entityCount, EntityRegistry &_registry)
{
    keys = SDL_GetKeyboardState(nullptr);

    for(ENTITY _e = 1; _e <= _entityCount; ++_e)
    {
        if(_registry.Transforms.find(_e) != _registry.Transforms.end() &&
           _registry.Inputs    .find(_e) != _registry.Inputs.end())
        {
            TransformComponent& _transform = _registry.Transforms[_e];

            if(keys[SDL_SCANCODE_A] && !aKeyPressedPrevFrame)
            {
                std::cout << "Press" << '\n';

                int _row    = _transform.rowIndex - 1;
                int _column = _transform.columnIndex;

                if(!engine->m_TileGrid->CheckTileCollider(_row, _column))
                {
                    _transform.velX = -1.0f;
                    _transform.rowIndex = _row;
                    _transform.isMoving = true;
                    _transform.moveDir = MoveDir::LEFT;

                    _transform.targetRect = engine->m_TileGrid->GetTileAt(0, _row, _column).sprite.dstRect;
                }
            }

            if(keys[SDL_SCANCODE_D] && !dKeyPressedPrevFrame)
            {
                int _row    = _transform.rowIndex + 1;
                int _column = _transform.columnIndex;

                if(!engine->m_TileGrid->CheckTileCollider(_row, _column))
                {
                    _transform.velX = 1.0f;
                    _transform.rowIndex = _row;
                    _transform.isMoving = true;
                    _transform.moveDir = MoveDir::RIGHT;

                    _transform.targetRect = engine->m_TileGrid->GetTileAt(0, _row, _column).sprite.dstRect;
                }
            }

            if(keys[SDL_SCANCODE_W] && !wKeyPressedPrevFrame)
                _transform.velY =  1.0f;
            if(keys[SDL_SCANCODE_S] && !sKeyPressedPrevFrame)
                _transform.velY = -1.0f;

            if(_transform.isMoving)
            {
                switch (_transform.moveDir)
                {
                    case MoveDir::NONE:
                        break;
                    case MoveDir::LEFT:
                        if(static_cast<int>(_transform.posX) > _transform.targetRect.x)
                            _transform.posX += _registry.Transforms[_e].velX * 20.f * engine->m_Time.deltaTime;
                        else
                        {
                            _transform.posX = static_cast<float>(_transform.targetRect.x);
                            _transform.isMoving = false;
                            _transform.moveDir = MoveDir::NONE;
                        }
                        break;
                    case MoveDir::RIGHT:
                        if(static_cast<int>(_transform.posX) < _transform.targetRect.x)
                            _transform.posX += _registry.Transforms[_e].velX * 20.f * engine->m_Time.deltaTime;
                        else
                        {
                            _transform.posX = static_cast<float>(_transform.targetRect.x);
                            _transform.isMoving = false;
                            _transform.moveDir = MoveDir::NONE;
                        }
                        break;
                    case MoveDir::UP:
                        break;
                    case MoveDir::DOWN:
                        break;
                }
            }
        }
    }

    aKeyPressedPrevFrame = keys[SDL_SCANCODE_A];
    dKeyPressedPrevFrame = keys[SDL_SCANCODE_D];
    wKeyPressedPrevFrame = keys[SDL_SCANCODE_W];
    sKeyPressedPrevFrame = keys[SDL_SCANCODE_S];
}
