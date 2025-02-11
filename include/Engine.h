#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include <SDL2/SDL.h>
#include <unordered_map>

#include "Timer.h"
#include "InputManager.h"
#include "TileSpriteSheetInfo.h"
#include "TileEditor.h"
#include "EntityInspector.h"
#include "EntityManager.h"
#include "SceneManager.h"

class Engine
{
    enum class EngineMode
    {
        PLAY_MODE,
        EDIT_MODE
    };

public:
    bool fullScreen;
    bool show_demo_window;
    bool showAboutInfo;
    bool showEntityInspector;
    bool showTileEditor;
    bool showSceneManager;

    Timer m_Time;
    InputManager inputManager;
    TileGrid* m_TileGrid;

    int selectedSpriteSheetIndex = 0;
    std::vector<std::string> spriteSheetItems;
    std::unordered_map<std::string, TileSpriteSheetInfo> spriteSheetData;

    Engine();
    ~Engine();

    void LoadAssets();
    void UnloadAssets();

    void SaveScene(const char* _path) const;
    void LoadScene(const char* _path);

    void SaveProjectSetting();
    void LoadProjectSetting();

    SDL_Texture* LoadTexture(const char* _name, const char* _filePath);

    void GenerateEntities();

    void ProcessInput(SDL_Event* _event);
    void Update();
    void UpdatePlayMode();
    void UpdateEditMode();

private:
    bool m_IsRunning;
    SDL_Window* m_Window;
    SDL_Renderer* m_Render;
    int m_ScreenWidth;
    int m_ScreenHeight;

    std::vector<SDL_Texture*> m_Textures;

    TileEditor*      m_TileEditor;
    EntityInspector* m_EntityInspector;
    EntityManager    m_EntityManager;
    SceneManager*    m_SceneManager;

    EngineMode m_EngineMode;

    SDL_Point _mousePos;
    SDL_Event _event;

    ImVec4 _clearCol = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 _gridCol  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    SDL_Color _clearColor { 0, 0, 0, SDL_ALPHA_OPAQUE };

    const char* projectSettingPath = "../savefiles/ProjectSettings.yaml";
};