#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "TileGrid.h"

class Engine;

struct SceneManager
{
    Engine* engine;

    char buf[256] = "";

    SceneManager(Engine* _engine, TileGrid* _tileGrid);
    ~SceneManager();

    void RenderEditorGUI();
};