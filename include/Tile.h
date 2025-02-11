#pragma once
#include <string>
#include <SDL2/SDL.h>
#include "Sprite.h"
#include "yaml-cpp/yaml.h"

struct Tile
{
    enum struct Flags{
        NONE,
        GROUND,
        WALL,
        LADDER,
    };

    Sprite sprite;

    int rowIndex;
    int columnIndex;

    Flags flags = Flags::NONE;

    bool hasCollider = false;

    std::string static FlagToString(Flags _flag)
    {
        switch(_flag)
        {
            case Flags::NONE:
                return "None";
            case Flags::GROUND:
                return "Ground";
            case Flags::WALL:
                return "Wall";
            case Flags::LADDER:
                return "Ladder";
        }
    }

    YAML::Node Serialize() const;
};