#include "TileGrid.h"

TileGrid::TileGrid():
gridWidth(0),
gridHeight(0)
{

}

TileGrid::TileGrid(int _width ,int _height):
        gridWidth(_width),
        gridHeight(_height)
{
    GenerateTiles();
}

void TileGrid::GenerateTiles()
{
    int _totalWidth  = gridWidth  * tileSize;
    int _totalHeight = gridHeight * tileSize;

    int _halfWidth  = _totalWidth  / 2;
    int _halfHeight = _totalHeight / 2;

    _xOffset = 800 - _halfWidth;
    _yOffset = 450 - _halfHeight;

    leftFrame   = _xOffset;
    rightFrame  = _xOffset + _totalWidth;
    topFrame    = _yOffset;
    bottomFrame = _yOffset + _totalHeight;

    for(std::size_t _layer = 0; _layer < MAX_LAYERS; ++_layer)
    {
        TileLayers.emplace_back();

        for(int _i = 0; _i < gridWidth; ++_i)
            for(int _j = 0; _j < gridHeight; ++_j)
            {
                Tile _newTile { };

                _newTile.rowIndex    = _i;
                _newTile.columnIndex = _j;

                _newTile.sprite.dstRect.x = _i * tileSize + _xOffset;
                _newTile.sprite.dstRect.y = _j * tileSize + _yOffset;
                _newTile.sprite.dstRect.w = tileSize;
                _newTile.sprite.dstRect.h = tileSize;

                _newTile.sprite.spriteSheet = nullptr;

                TileLayers[_layer].push_back(_newTile);
            }
    }
}

void TileGrid::MousePosToGridCoord(int _mouseX, int _mouseY, int& _row, int& _column) const
{
    _row    = (_mouseX - _xOffset) / tileSize;
    _column = (_mouseY - _yOffset) / tileSize + 1;
}

void TileGrid::AddTile(const int& _row, const int& _column,
             int _texCoordX,
             int _texCoordY,
             int _tileSize,
             SDL_Texture* _texture)
{
    Tile& _tile = TileLayers[currentLayer][_row * gridHeight + _column];

    _tile.sprite.srcRect.x = _texCoordX;
    _tile.sprite.srcRect.y = _texCoordY;
    _tile.sprite.srcRect.w = _tileSize;
    _tile.sprite.srcRect.h = _tileSize;

    if(_tile.sprite.spriteSheet == nullptr)
        _tile.sprite.spriteSheet = _texture;
}

void TileGrid::AddTile(
        const int& _row,
        const int& _column,
        int _texCoordX,
        int _texCoordY,
        TileSpriteSheetInfo& _spriteSheetInfo)
{
    Tile& _tile = TileLayers[currentLayer][_row * gridHeight + _column];

    _tile.sprite.srcRect.x = _texCoordX;
    _tile.sprite.srcRect.y = _texCoordY;
    _tile.sprite.srcRect.w = _spriteSheetInfo.tileUnitPixelSize;
    _tile.sprite.srcRect.h = _spriteSheetInfo.tileUnitPixelSize;

    _tile.sprite.spriteSheet = _spriteSheetInfo.texture;
    _tile.sprite.name        = _spriteSheetInfo.spriteSheetName;
    _tile.sprite.path        = _spriteSheetInfo.spriteSheetPath;
}

void TileGrid::AddAnimatedTile(
        const int& _row,
        const int& _column,
        int _texCoordX,
        int _texCoordY,
        TileSpriteSheetInfo& _spriteSheetInfo,
        const std::string& _animationName)
{
    Tile& _tile = TileLayers[currentLayer][_row * gridHeight + _column];

    _tile.sprite.isAnimated     = true;
    _tile.sprite.spriteSheet    = _spriteSheetInfo.texture;
    _tile.sprite.name           = _spriteSheetInfo.spriteSheetName;
    _tile.sprite.path           = _spriteSheetInfo.spriteSheetPath;
    _tile.sprite.animationClips = _spriteSheetInfo.animationClips[_animationName].clips;
}

void TileGrid::RemoveTile(const int& _row, const int& _column)
{
    Tile& _tile = TileLayers[currentLayer][_row * gridHeight + _column];

    _tile.sprite.Reset();
    _tile.hasCollider = false;
    _tile.flags = Tile::Flags::NONE;
}

void TileGrid::SetTileCollider(const int& _row, const int& _column, const bool& _value)
{
    Tile& _tile = TileLayers[currentLayer][_row * gridHeight + _column];
    _tile.hasCollider = _value;
}

void TileGrid::SetTileFlag(const int& _row, const int& _column, Tile::Flags _flag)
{
    Tile& _tile = TileLayers[currentLayer][_row * gridHeight + _column];
    _tile.flags = _flag;
}

Tile& TileGrid::GetTileAt(const LAYER& _layer, const int& _row, const int& _column)
{
    return TileLayers[_layer][_row * gridHeight + _column];
}

bool TileGrid::CheckTileCollider(const int& _row, const int& _column) const
{
    const Tile& _tile = TileLayers[0][_row * gridHeight + _column];
    return _tile.hasCollider;
}

void TileGrid::ClearGrid()
{
    for(const auto& _layer : TileLayers)
    {
        for(auto _tile : _layer)
        {
            _tile.sprite.Reset();
            _tile.hasCollider = false;
            _tile.flags = Tile::Flags::NONE;
        }
    }

    TileLayers.clear();
}

void TileGrid::Render(SDL_Renderer* _render)
{
    for(std::vector<Tile>& _tiles: TileLayers)
    {
        for(Tile& _tile : _tiles)
        {
            if(_tile.sprite.isAnimated)
                _tile.sprite.PlayAnimation(0.016f);

            _tile.sprite.Render(_render);
        }
    }
}