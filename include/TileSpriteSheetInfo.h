#pragma once
#include <iostream>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "yaml-cpp/yaml.h"

struct TileSpriteSheetInfo
{
    struct ClipData
    {
        float animationSpeed = 5.0f;
        std::vector<SDL_Rect> clips;
    };

    SDL_Texture* texture;
    SDL_Surface* surface;
    SDL_Renderer* render;

    int tileUnitPixelSize;

    int spriteSheetWidth;
    int spriteSheetHeight;

    int numberOfTileWidth;
    int numberOfTileHeight;

    const char* spriteSheetName;
    const char* spriteSheetPath;

    unsigned int animationClipIndex;
    unsigned int animationCount;

    std::unordered_map<std::string, ClipData> animationClips;

    TileSpriteSheetInfo();
    TileSpriteSheetInfo(SDL_Texture* _texture, SDL_Renderer* _render, const char* _name, const char* _path);
    ~TileSpriteSheetInfo();

    void SetAnimationSpeed(std::string _animationName, float _speed);
    void AddAnimationClip(std::string _animationName, SDL_Rect _clip);

    YAML::Node Serialize() const;
};


