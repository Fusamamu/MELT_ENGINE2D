#include "Tile.h"

 YAML::Node Tile::Serialize() const
{
    YAML::Node _node;
    _node["row idx"] = rowIndex;
    _node["col idx"] = columnIndex;
    _node["sprite"]  = sprite.Serialize();
    _node["collider"] = hasCollider;
    return _node;
}
