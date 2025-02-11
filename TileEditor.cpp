#include "TileEditor.h"
#include "Engine.h"

TileEditor::TileEditor(Engine* _engine, TileGrid* _tileGrid):
engine(_engine),
tileGridCanvas(_tileGrid),
useAnimatedTile(false),
tileWidthCount(10),
tileHeightCount(10)
{

}

TileEditor::~TileEditor()
{

}

void TileEditor::Init(SDL_Renderer* _render)
{
    Active = true;

    TileMapPanelActive   = true;
    TileGridCanvasActive = true;
    DisplayGizmos        = true;
    DisplayGridGuideline = true;
    DisplayBoxCollider   = true;

    modifiedSpriteSheet = &engine->spriteSheetData["Monochrome"];

    scaledTileSize  = modifiedSpriteSheet->tileUnitPixelSize * mapScale;

    selectionCursor.x = 0;
    selectionCursor.y = 0;
    selectionCursor.w = scaledTileSize;
    selectionCursor.h = scaledTileSize;

    currentTileSelection.x = 0;
    currentTileSelection.y = 0;
    currentTileSelection.w = scaledTileSize;
    currentTileSelection.h = scaledTileSize;

    editCursor.x = 0;
    editCursor.y = 0;
    editCursor.w = 32;
    editCursor.h = 32;

    tileGridCanvas->tileSize   = 32;
    tileGridCanvas->gridWidth  = tileWidthCount;
    tileGridCanvas->gridHeight = tileHeightCount;
    tileGridCanvas->GenerateTiles();
}

void TileEditor::ProcessInput(SDL_Event* _event)
{
    switch(_event->type)
    {
        case SDL_KEYDOWN:

            if(_event->key.keysym.sym == SDLK_e)
                TileMapPanelActive = !TileMapPanelActive;

            if(_event->key.keysym.sym == SDLK_g)
                DisplayGridGuideline = !DisplayGridGuideline;

            if(_event->key.keysym.sym == SDLK_q)
                tileGridCanvas->ClearGrid();

            break;
    }
}

void TileEditor::Update(const SDL_Point& _mousePos)
{
    if(_mousePos.x < tileGridCanvas->leftFrame   ||
       _mousePos.x > tileGridCanvas->rightFrame  ||
       _mousePos.y > tileGridCanvas->bottomFrame ||
       _mousePos.y < tileGridCanvas->topFrame)
    {
        return;
    }

    editCursor.x = _mousePos.x / 32 * 32;
    editCursor.y = _mousePos.y / 32 * 32;

    if(engine->inputManager.mousePressed)
    {
        int _row;
        int _column;
        tileGridCanvas->MousePosToGridCoord(editCursor.x, editCursor.y, _row, _column);

        if(engine->inputManager.leftMousePressed)
        {
            switch(currentEditMode)
            {
                case EditMode::EDIT_TILE:
                    if(!useAnimatedTile)
                    {
                        tileGridCanvas->AddTile(_row, _column,
                                                currentTexCoord.x,
                                                currentTexCoord.y,
                                                *modifiedSpriteSheet);

                    }
                    else
                    {
                        if(currentEditAnimationName.empty())
                            break;

                        tileGridCanvas->AddAnimatedTile(_row, _column,
                                                        currentTexCoord.x,
                                                        currentTexCoord.y,
                                                        *modifiedSpriteSheet,
                                                        currentEditAnimationName);
                    }
                    break;
                case EditMode::EDIT_BOX_COLLIDER:
                    tileGridCanvas->SetTileCollider(_row, _column, true);
                    break;
                case EditMode::EDIT_FLAG:
                    tileGridCanvas->SetTileFlag(_row, _column, Tile::Flags::GROUND);
                    break;
                case EditMode::EDIT_OBJECT:
                    break;
            }
        }

        if(engine->inputManager.rightMousePressed)
        {
            switch(currentEditMode)
            {
                case EditMode::EDIT_TILE:
                    tileGridCanvas->RemoveTile(_row, _column);
                    break;
                case EditMode::EDIT_BOX_COLLIDER:
                    tileGridCanvas->SetTileCollider(_row, _column, false);
                    break;
                case EditMode::EDIT_FLAG:
                    tileGridCanvas->SetTileFlag(_row, _column, Tile::Flags::NONE);
                    break;
                case EditMode::EDIT_OBJECT:
                    break;
            }
        }
    }
}

void TileEditor::Render(SDL_Renderer* _render)
{
    if(TileGridCanvasActive)
    {
        tileGridCanvas->Render(_render);

        if(DisplayGizmos)
        {
            for(const Tile& _tile : tileGridCanvas->TileLayers[0])
            {
                if(DisplayGridGuideline)
                {
                    SDL_SetRenderDrawColor(_render, gridColor.r, gridColor.g, gridColor.b, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawRect(_render, &_tile.sprite.dstRect);
                }

                if(DisplayBoxCollider)
                {
                    if(_tile.hasCollider)
                    {
                        SDL_SetRenderDrawColor(_render, 255, 0, 0, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawRect(_render, &_tile.sprite.dstRect);
                    }
                }

                if(DisplayFlags)
                {
                    FlagRect.x = _tile.sprite.dstRect.x + 1;
                    FlagRect.y = _tile.sprite.dstRect.y + 1;
                    FlagRect.w = _tile.sprite.dstRect.w - 3;
                    FlagRect.h = _tile.sprite.dstRect.h - 3;

                    switch(_tile.flags)
                    {
                        case Tile::Flags::NONE:
                            break;
                        case Tile::Flags::GROUND:
                            SDL_SetRenderDrawColor(_render, 255, 255, 0, SDL_ALPHA_OPAQUE);
                            SDL_RenderDrawRect(_render, &FlagRect);
                            break;
                        case Tile::Flags::WALL:
                            SDL_SetRenderDrawColor(_render, 0, 255, 255, SDL_ALPHA_OPAQUE);
                            SDL_RenderDrawRect(_render, &FlagRect);
                            break;
                        case Tile::Flags::LADDER:
                            SDL_SetRenderDrawColor(_render, 255, 0, 255, SDL_ALPHA_OPAQUE);
                            SDL_RenderDrawRect(_render, &FlagRect);
                            break;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(_render, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(_render, &editCursor);
    }
}

void TileEditor::RenderEditorGUI()
{
    ImGui::SeparatorText("Tile animation");
    if(ImGui::Checkbox("Edit tile animation", &EditTileAnimation))
    {

    }

    if(EditTileAnimation)
    {
        if(ImGui::Button("Add animation slot"))
            modifiedSpriteSheet->animationClips[buf];

        ImGui::SameLine();
        ImGui::InputText("Animation name", buf, IM_ARRAYSIZE(buf));

        ImGui::Text("Animation data");
        ImGui::SameLine();
        if(ImGui::Button("CLEAR"))
        {
            modifiedSpriteSheet->animationClips.clear();
        }
        ImGui::SameLine();
        if(ImGui::Button("SAVE"))
        {
            engine->SaveProjectSetting();
        }
        ImGui::SameLine();

        if(ImGui::Button("LOAD"))
        {
            modifiedSpriteSheet->animationClips.clear();
            engine->LoadProjectSetting();
        }
    }

    ImVec2 _cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 _mousePos  = ImGui::GetMousePos();

    auto _tileSize = static_cast<float>(scaledTileSize);

    int _id = 0;
    std::string _toBeErasedKey;

    for(auto _it = modifiedSpriteSheet->animationClips.begin(); _it != modifiedSpriteSheet->animationClips.end(); ++_it)
    {
        std::string _name             = _it->first;
        std::vector<SDL_Rect>& _clips = _it->second.clips;

        ImGui::PushID(_id);

        if(ImGui::Button("CLEAR"))
            modifiedSpriteSheet->animationClips[_name].clips.clear();
        ImGui::SameLine();
        if(ImGui::Button("DELETE"))
            _toBeErasedKey = _name;
        ImGui::SameLine();

        if(ImGui::Selectable(_name.c_str(), selectedAnimationItem[_name]))
        {
            for(auto& _item : selectedAnimationItem)
                _item.second = false;

            selectedAnimationItem[_name] = !selectedAnimationItem[_name];
            currentEditAnimationName = _name;
        }

        for(int _i = 0; _i < _clips.size(); ++_i)
        {
            auto _clip = _clips[_i];

            ImGui::PushID(_i);
            if (_i > 0)
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(_i - 1.0f, _i - 1.0f));

            auto _imageSize = static_cast<float>(modifiedSpriteSheet->tileUnitPixelSize * mapScale);
            ImVec2 size = ImVec2(_imageSize, _imageSize);

            auto _texCoordX = static_cast<float>(_clip.x) / static_cast<float>(modifiedSpriteSheet->spriteSheetWidth);
            auto _texCoordY = static_cast<float>(_clip.y) / static_cast<float>(modifiedSpriteSheet->spriteSheetHeight);

            ImVec2 uv0 = ImVec2(_texCoordX, _texCoordY);
            ImVec2 uv1 = ImVec2(
                    _texCoordX + (static_cast<float>(modifiedSpriteSheet->tileUnitPixelSize) / modifiedSpriteSheet->spriteSheetWidth),
                    _texCoordY + (static_cast<float>(modifiedSpriteSheet->tileUnitPixelSize) / modifiedSpriteSheet->spriteSheetHeight));

//        ImVec2 uv0 = ImVec2(8.0f/modifiedSpriteSheet->spriteSheetWidth, 0.0f);                            // UV coordinates for lower-left
//        ImVec2 uv1 = ImVec2(16.0f/modifiedSpriteSheet->spriteSheetWidth, 8.0f /modifiedSpriteSheet->spriteSheetHeight);    // UV coordinates for (32,32) in our texture

            ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
            ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

            ImGui::Image((ImTextureID)(intptr_t)modifiedSpriteSheet->texture, size, uv0, uv1, tint_col);

            if (_i > 0)
                ImGui::PopStyleVar();
            ImGui::PopID();
            ImGui::SameLine();
        }

        ImGui::NewLine();
        ImGui::InputFloat("Animation speed", &modifiedSpriteSheet->animationClips[_name].animationSpeed);

        ImGui::PopID();
        _id++;

        ImGui::NewLine();
    }

    if(!_toBeErasedKey.empty())
        modifiedSpriteSheet->animationClips.erase(_toBeErasedKey);

    ImGui::NewLine();
    ImGui::SeparatorText("Sprite sheet display");

    std::string _text = "Texture coord : " +
                        std::to_string(currentTexCoord.x) + "," +
                        std::to_string(currentTexCoord.y);
    ImGui::Text(_text.c_str());

    _cursorPos = ImGui::GetCursorScreenPos();

    ImVec2 _imageSize {
            static_cast<float>(modifiedSpriteSheet->spriteSheetWidth  * mapScale),
            static_cast<float>(modifiedSpriteSheet->spriteSheetHeight * mapScale)
    };

    ImGui::Image((ImTextureID)(intptr_t)modifiedSpriteSheet->texture, _imageSize);

    ImVec2 p0 {};
    ImVec2 p1 {};

    /*Draw grid*/
    for(int _i = 0; _i < modifiedSpriteSheet->numberOfTileWidth; ++_i)
    {
        for(int _j = 0; _j < modifiedSpriteSheet->numberOfTileHeight; ++_j)
        {
            p0 = ImVec2(
                    _cursorPos.x + _i * scaledTileSize,
                    _cursorPos.y + _j * scaledTileSize
                    );

            p1 = ImVec2(
                    p0.x + scaledTileSize,
                    p0.y + scaledTileSize
            );

            ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(10, 10, 10, 255));
        }
    }

    /*Draw frame*/
    p0 = _cursorPos;
    p1 = ImVec2(p0.x + _imageSize.x,p0.y + _imageSize.y);
    ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(255, 255, 255, 255));

    /*Render selected tile*/
    currentTileSelectionCursorPos_P1 = ImVec2(
            currentTileSelectionCursorPos_P0.x + _tileSize,
            currentTileSelectionCursorPos_P0.y + _tileSize);

    ImGui::GetWindowDrawList()->AddRect(
            currentTileSelectionCursorPos_P0,
            currentTileSelectionCursorPos_P1,
            IM_COL32(0, 255, 0, 255));


    /*Render mouse tile selection cursor*/
    if(_mousePos.y > _cursorPos.y &&
       _mousePos.y < _cursorPos.y + _imageSize.y &&
       _mousePos.x > _cursorPos.x &&
       _mousePos.x < _cursorPos.x + _imageSize.x)
    {
        ImVec2 _mouseP0 = {
                (_tileSize * std::floor((_mousePos.x - _cursorPos.x) / _tileSize) + _cursorPos.x),
                (_tileSize * std::floor((_mousePos.y - _cursorPos.y) / _tileSize) + _cursorPos.y)
        };
        ImVec2 _mouseP1 = ImVec2(_mouseP0.x + _tileSize, _mouseP0.y + _tileSize);
        ImGui::GetWindowDrawList()->AddRect(_mouseP0, _mouseP1, IM_COL32(255, 0, 0, 255));

        if(engine->inputManager.leftMousePressed)
        {
            currentTexCoord.x = static_cast<int>(_mouseP0.x - _cursorPos.x) / mapScale;
            currentTexCoord.y = static_cast<int>(_mouseP0.y - _cursorPos.y) / mapScale;

            if(!EditTileAnimation)
            {
                currentTileSelectionCursorPos_P0 = _mouseP0;
            }
            else
            {
                //Add clip to list
                modifiedSpriteSheet->AddAnimationClip(currentEditAnimationName,{
                    currentTexCoord.x,
                    currentTexCoord.y,
                    modifiedSpriteSheet->tileUnitPixelSize,
                    modifiedSpriteSheet->tileUnitPixelSize
                });
            }
        }
    }
}

void TileEditor::UpdateSelectedEditMode()
{
    std::string _selectedMode = EditModeItems[SelectedEditModeIndex];

    if(_selectedMode == editTile)
        currentEditMode = EditMode::EDIT_TILE;
    else if(_selectedMode == editBoxCollider)
        currentEditMode = EditMode::EDIT_BOX_COLLIDER;
    else if(_selectedMode == editFlag)
        currentEditMode = EditMode::EDIT_FLAG;
    else if(_selectedMode == editObject)
        currentEditMode = EditMode::EDIT_OBJECT;
}

void TileEditor::UpdateLayers()
{
    std::string _selectedLayer = LayerItems[tileGridCanvas->currentLayer];

    if(_selectedLayer == "Layer 0")
        tileGridCanvas->currentLayer = 0;
    else if(_selectedLayer == "Layer 1")
        tileGridCanvas->currentLayer = 1;
    else if(_selectedLayer == "Layer 2")
        tileGridCanvas->currentLayer = 2;
    else if(_selectedLayer == "Layer 3")
        tileGridCanvas->currentLayer = 3;
    else if(_selectedLayer == "Layer 4")
        tileGridCanvas->currentLayer = 4;
}

void TileEditor::UpdateSelectedFlag()
{
    const char* _selectedFlag = FlagItems[SelectedFlagIndex];
}

void TileEditor::UpdateScale()
{
    scaledTileSize = mapScale * modifiedSpriteSheet->tileUnitPixelSize;

    selectionCursor.w = scaledTileSize;
    selectionCursor.h = scaledTileSize;

    currentTileSelection.w = scaledTileSize;
    currentTileSelection.h = scaledTileSize;
}