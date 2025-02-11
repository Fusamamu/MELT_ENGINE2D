#include "TileSpriteSheetInfo.h"

TileSpriteSheetInfo::TileSpriteSheetInfo():
texture(nullptr),
surface(nullptr),
render(nullptr),
tileUnitPixelSize(8),
spriteSheetWidth(0),
spriteSheetHeight(0),
spriteSheetName(""),
spriteSheetPath("")
{

}

TileSpriteSheetInfo::TileSpriteSheetInfo(
        SDL_Texture* _texture,
        SDL_Renderer* _render,
        const char* _name,
        const char* _path):
texture(_texture),
surface(nullptr),
render(_render),
tileUnitPixelSize(8),
spriteSheetWidth(0),
spriteSheetHeight(0),
spriteSheetName(_name),
spriteSheetPath(_path)
{
    SDL_QueryTexture(_texture, nullptr, nullptr, &spriteSheetWidth, &spriteSheetHeight);

    numberOfTileWidth  = spriteSheetWidth  / tileUnitPixelSize;
    numberOfTileHeight = spriteSheetHeight / tileUnitPixelSize;
}

TileSpriteSheetInfo::~TileSpriteSheetInfo()
{
}

void TileSpriteSheetInfo::SetAnimationSpeed(std::string _animationName, float _speed)
{
    animationClips[_animationName].animationSpeed = _speed;
}

void TileSpriteSheetInfo::AddAnimationClip(std::string _animationName, SDL_Rect _clip)
{
    animationClips[_animationName].clips.emplace_back(_clip);
}

YAML::Node TileSpriteSheetInfo::Serialize() const
{
    YAML::Node _node;

    for(auto _data : animationClips)
    {
        _node[_data.first]["AnimSpeed"] = _data.second.animationSpeed;

        for(int _i = 0; _i < _data.second.clips.size(); ++_i)
        {
            auto _clip = _data.second.clips[_i];

            YAML::Node _clipNode;

            _clipNode.push_back(_clip.x);
            _clipNode.push_back(_clip.y);
            _clipNode.push_back(_clip.w);
            _clipNode.push_back(_clip.h);

            _node[_data.first]["Clips"].push_back(_clipNode);
        }
    }

    return _node;
}