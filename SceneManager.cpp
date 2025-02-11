#include "SceneManager.h"
#include "Engine.h"

SceneManager::SceneManager(Engine* _engine, TileGrid* _tileGrid):
engine(_engine)
{

}

SceneManager::~SceneManager()
{

}

void SceneManager::RenderEditorGUI()
{
    ImGui::Begin("Scene manager", &engine->showSceneManager);

    if(ImGui::Button("SAVE"))
    {

    }
    ImGui::SameLine();
    if(ImGui::Button("LOAD"))
    {

    }
    ImGui::SameLine();
    ImGui::InputText("Scene name", buf, IM_ARRAYSIZE(buf));




    ImGui::End();
}