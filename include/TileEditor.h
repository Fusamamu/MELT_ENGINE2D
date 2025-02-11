#pragma once
#include <SDL2_image/SDL_image.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "TileGrid.h"
#include "TileSpriteSheetInfo.h"

class Engine;

struct TileEditor
{
    enum class EditMode
    {
        EDIT_TILE,
        EDIT_BOX_COLLIDER,
        EDIT_FLAG,
        EDIT_OBJECT,
    };

    EditMode currentEditMode = EditMode::EDIT_TILE;

    TileGrid* tileGridCanvas;

    TileSpriteSheetInfo* modifiedSpriteSheet;

    int mapScale = 3;
    int scaledTileSize;

    int tileWidthCount;
    int tileHeightCount;

    SDL_Rect editCursor;
    SDL_Rect selectionCursor;
    SDL_Rect currentTileSelection;
    SDL_Rect FlagRect;
    SDL_Rect BoxColliderRect;

    SDL_Point currentTexCoord;

    ImVec2 currentTileSelectionCursorPos_P0 { };
    ImVec2 currentTileSelectionCursorPos_P1 { };

    bool TileMapPanelActive;
    bool TileGridCanvasActive;

    bool Active;
    bool DisplayGizmos;
    bool DisplayGridGuideline;
    bool DisplayBoxCollider;
    bool DisplayFlags;

    char buf[256] = "";
    bool useAnimatedTile;
    bool EditTileAnimation;
    std::string currentEditAnimationName;
    std::unordered_map<std::string, bool> selectedAnimationItem;

    const char* noneFlag    = "None";
    const char* groundFlag  = "Ground";
    const char* wallFlag    = "Wall";
    const char* ladderFlag  = "Ladder";

    int SelectedFlagIndex = 0;
    const char* FlagItems[4] = { noneFlag, groundFlag, wallFlag, ladderFlag };

    const char* editTile        = "Edit tile";
    const char* editBoxCollider = "Edit box collider";
    const char* editFlag        = "Edit flag";
    const char* editObject      = "Edit object";

    int SelectedEditModeIndex = 0;
    const char* EditModeItems[4] = { editTile, editBoxCollider, editFlag, editObject };

    const char* LayerItems[5] = { "Layer 0", "Layer 1", "Layer 2", "Layer 3", "Layer 4",};

    SDL_Color gridColor { 21 , 17, 33, 255 };

    Engine* engine;

    TileEditor(Engine* _engine, TileGrid* _tileGrid);
    ~TileEditor();

    void Init(SDL_Renderer* _render);

    void UpdateSelectedEditMode();
    void UpdateLayers();
    void UpdateSelectedFlag();
    void UpdateScale();

    void ProcessInput(SDL_Event* _event);
    void Update(const SDL_Point& _mousePos);
    void Render(SDL_Renderer* _render);
    void RenderEditorGUI();
};