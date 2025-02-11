#pragma once
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include "Tile.h"
#include "TileSpriteSheetInfo.h"

using LAYER = int;

struct TileGrid
{
    int tileSize = 30;

    int gridWidth;
    int gridHeight;

    int _xOffset;
    int _yOffset;

    int leftFrame;
    int rightFrame;
    int topFrame;
    int bottomFrame;

    const LAYER MAX_LAYERS = 5;
    LAYER currentLayer = 0;
    std::vector<std::vector<Tile>> TileLayers;

    TileGrid();
    TileGrid(int _width ,int _height);

    void GenerateTiles();
    void MousePosToGridCoord(int _mouseX, int _mouseY, int& _row, int& _column) const;
    void AddTile(const int& _row, const int& _column,
                 int _texCoordX,
                 int _texCoordY,
                 int _tileSize,
                 SDL_Texture* _texture);

    void AddTile(const int& _row,
                 const int& _column,
                 int _texCoordX,
                 int _texCoordY,
                 TileSpriteSheetInfo& _spriteSheetInfo);

    void AddAnimatedTile(const int& _row,
                         const int& _column,
                         int _texCoordX,
                         int _texCoordY,
                         TileSpriteSheetInfo& _spriteSheetInfo,
                         const std::string& _animationName);

    void RemoveTile(const int& _row, const int& _column);
    void SetTileCollider(const int& _row, const int& _column, const bool& _value);
    void SetTileFlag(const int& _row, const int& _column, Tile::Flags _flag);

    Tile& GetTileAt(const LAYER& _layer, const int& _row, const int& _column);

    bool CheckTileCollider(const int& _row, const int& _column) const;
    void ClearGrid();
    void Render(SDL_Renderer* _render);
};