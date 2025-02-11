#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

enum class MoveDir
{
    NONE,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct TransformComponent
{
    float posX;
    float posY;
    float velX;
    float velY;

    int rowIndex;
    int columnIndex;

    bool isMoving;

    SDL_Rect targetRect;

    MoveDir moveDir = MoveDir::NONE;
};

struct TileComponent
{
    int xCoord;
    int yCoord;

    bool hasCollider;
};

struct SpriteComponent
{
    SDL_Rect src;
    SDL_Rect dst;
    SDL_Texture* texture;

    bool isAnimated;
    int clipIndex;
    float clipElapsed;
    float animationSpeed;
    std::vector<SDL_Rect> animationClips;
};

struct InputComponent
{

};