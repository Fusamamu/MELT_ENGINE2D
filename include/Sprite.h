#pragma once
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "yaml-cpp/yaml.h"

struct Sprite
{
    bool isAnimated;

    SDL_Rect srcRect;
    SDL_Rect dstRect;
    SDL_Texture* spriteSheet;

    int clipIndex;
    float clipElapsed;
    float animationSpeed;
    std::vector<SDL_Rect> animationClips;

    const char* name;
    const char* path;

    Sprite();
    void Render(SDL_Renderer* _render);
    void PlayAnimation(float _dt);
    void StopAnimation();
    void Reset();
    YAML::Node Serialize() const;
};