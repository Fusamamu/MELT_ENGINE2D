#include "Sprite.h"
//#include <SDL2/SDL.h>

Sprite::Sprite() :
        isAnimated(false),
        srcRect({0, 0, 0, 0}),
        dstRect({0, 0, 0, 0}),
        spriteSheet(nullptr),
        name(nullptr),
        path(nullptr),
        clipIndex(0),
        clipElapsed(0),
        animationSpeed(10.f)
{


}

void Sprite::Render(SDL_Renderer* _render)
{
    if(spriteSheet != nullptr)
        SDL_RenderCopy(_render, spriteSheet, &srcRect, &dstRect);
}

void Sprite::PlayAnimation(float _dt)
{
    if(animationClips.empty())
        return;
    srcRect = animationClips[clipIndex];

    clipElapsed += _dt * animationSpeed;
    if(clipElapsed > 1)
    {
        clipIndex++;
        if(clipIndex > animationClips.size() - 1)
            clipIndex = 0;

        clipElapsed = 0;
    }
}

void Sprite::StopAnimation()
{

}

void Sprite::Reset()
{
    isAnimated  = false;
    srcRect     = {0, 0, 0, 0};
    spriteSheet = nullptr;
    name = "";
    path = "";
    animationClips.clear();
}

YAML::Node Sprite::Serialize() const
{
    YAML::Node _node;

    _node["srcRect"].push_back(srcRect.x);
    _node["srcRect"].push_back(srcRect.y);
    _node["srcRect"].push_back(srcRect.w);
    _node["srcRect"].push_back(srcRect.h);

    _node["dstRect"].push_back(dstRect.x);
    _node["dstRect"].push_back(dstRect.y);
    _node["dstRect"].push_back(dstRect.w);
    _node["dstRect"].push_back(dstRect.h);

    _node["Name"] = name == nullptr ? "null" : name;
    _node["Path"] = path == nullptr ? "null" : path;

    return _node;
}