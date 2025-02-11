#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "Engine.h"
#include "Utility.h"

#include "nfd.h"
#include <string>
#include <filesystem>
#include <fstream>

#include "yaml-cpp/yaml.h"

Engine::Engine() :
m_IsRunning(true),
m_Window(nullptr),
m_Render(nullptr),
m_ScreenWidth(1600),
m_ScreenHeight(900),
m_EngineMode(EngineMode::EDIT_MODE),
inputManager({}),
m_EntityManager(EntityManager(this)),
m_TileEditor(nullptr),
m_TileGrid(nullptr),
m_SceneManager(nullptr),
_mousePos({}),
_event({}),
fullScreen(true),
showSceneManager(false)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    m_Window = SDL_CreateWindow
            (
                    "MELT BY MUGCUP",
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    m_ScreenWidth,
                    m_ScreenHeight,
                    SDL_WINDOW_SHOWN
            );

    if(m_Window == nullptr)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    SDL_SetWindowFullscreen(m_Window, fullScreen ? SDL_WINDOW_FULLSCREEN : 0);

    m_Render = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(m_Window, m_Render);
    ImGui_ImplSDLRenderer2_Init(m_Render);

    NFD_Init();

    m_TileGrid   = new TileGrid();
    m_TileEditor = new TileEditor(this, m_TileGrid);
    m_TileEditor->Init(m_Render);

    m_SceneManager = new SceneManager(this, m_TileGrid);
}

Engine::~Engine()
{
    UnloadAssets();

    SDL_DestroyRenderer(m_Render);
    SDL_DestroyWindow  (m_Window);
    m_Render = nullptr;
    m_Window = nullptr;

    delete m_TileGrid;
    delete m_TileEditor;

    NFD_Quit();
    SDL_Quit();
}

void Engine::LoadAssets()
{
    ENTITY _e = m_EntityManager.CreateEntity();

    m_EntityManager.Registry.Transforms[_e] = TransformComponent({0, 0});

    m_EntityManager.Registry.Sprites[_e] = SpriteComponent {
        SDL_Rect { 0, 0, 16, 16},
        SDL_Rect { 0, 0, 256, 256},
        IMG_LoadTexture(m_Render, "../res/tiles.png")
    };

    const char* _tilesPath    = "../res/tiles.png";
    const char* _blueFirePath = "../res/fire/blue/loops/burning_loop_1.png";
    const char* _monochrome   = "../res/monochrome_tilemap_packed.png";
    const char* _waterPath    = "../res/Animated-Tiles/Animated-Water-Tiles-Sheet(10-frames).png";
    const char* _wallTorchPath = "../res/Animated-Tiles/Wall-Torch.png";

    LoadTexture("Monochrome", _monochrome   );
    LoadTexture("WallTorch" , _wallTorchPath);
    LoadTexture("Tiles"     , _tilesPath    );
    LoadTexture("BlueFire"  , _blueFirePath );
    LoadTexture("Water"     , _waterPath    );
}

void Engine::UnloadAssets()
{
    for(auto _data : spriteSheetData)
        SDL_DestroyTexture(_data.second.texture);

    spriteSheetData.clear();
}

void Engine::SaveScene(const char* _path) const
{
    YAML::Node _root;

    for(auto&  _spriteData : spriteSheetData)
    {
        YAML::Node _data;
        _data[_spriteData.first] = _spriteData.second.spriteSheetPath;
        _root["Sprites"] = _data;
    }

    YAML::Node _tileGridData;

    YAML::Node _gridSize;
    YAML::Node _tileNode;

    _tileGridData["TileGridData"] = _gridSize;
    _tileGridData["Tiles"]        = _tileNode;

    _gridSize["Tile width"]  = m_TileGrid->gridWidth;
    _gridSize["Tile height"] = m_TileGrid->gridHeight;

    _root["TileGrid"] = _tileGridData;

    for(std::vector<Tile>& _tiles : m_TileGrid->TileLayers)
    {
        YAML::Node _layerNode;

        for(const Tile& _tile :_tiles)
        {
            auto _node = _tile.Serialize();
            _layerNode.push_back(_node);
        }

        _tileNode.push_back(_layerNode);
    }

    std::ofstream _file;
    _file.open(_path);
    _file << _root;
    _file.close();
}

void Engine::LoadScene(const char* _path)
{
    YAML::Node _root = YAML::LoadFile("../savefiles/SceneA.yaml");

    YAML::Node _tileGrid = _root["TileGrid"];

    m_TileGrid->ClearGrid();
    m_TileGrid->gridWidth  = _tileGrid["TileGridData"]["Tile width"].as<int>();
    m_TileGrid->gridHeight = _tileGrid["TileGridData"]["Tile height"].as<int>();
    m_TileGrid->GenerateTiles();

    YAML::Node _layers = _tileGrid["Tiles"];

    LAYER _tileLayer = 0;

    for(YAML::const_iterator _layer = _layers.begin(); _layer != _layers.end(); ++_layer)
    {
        YAML::Node _tiles = (*_layer);

        for(YAML::const_iterator _it = _tiles.begin(); _it != _tiles.end(); ++_it)
        {
            int _row    = (*_it)["row idx"].as<int>();
            int _column = (*_it)["col idx"].as<int>();

            Tile& _tile = m_TileGrid->GetTileAt(_tileLayer, _row, _column);

            _tile.hasCollider = (*_it)["collider"].as<bool>();

            auto _spriteName = (*_it)["sprite"]["Name"].as<std::string>();

            _tile.sprite.srcRect.x = (*_it)["sprite"]["srcRect"][0].as<int>();
            _tile.sprite.srcRect.y = (*_it)["sprite"]["srcRect"][1].as<int>();
            _tile.sprite.srcRect.w = (*_it)["sprite"]["srcRect"][2].as<int>();
            _tile.sprite.srcRect.h = (*_it)["sprite"]["srcRect"][3].as<int>();

            if(spriteSheetData.find(_spriteName) != spriteSheetData.end())
            {
                _tile.sprite.spriteSheet = spriteSheetData[_spriteName].texture;
                _tile.sprite.name        = spriteSheetData[_spriteName].spriteSheetName;
                _tile.sprite.path        = spriteSheetData[_spriteName].spriteSheetPath;
            }
        }

        _tileLayer++;
    }
}

void Engine::SaveProjectSetting()
{
    YAML::Node _root;
    for(auto _data : spriteSheetData)
        _root["AnimationData"][_data.first] = _data.second.Serialize();

    std::ofstream _file;
    _file.open(projectSettingPath);
    _file << _root;
    _file.close();
}

void Engine::LoadProjectSetting()
{
    YAML::Node _root = YAML::LoadFile(projectSettingPath);

    if(!_root)
        return;

    for(auto _data : spriteSheetData)
    {
        auto _spriteKey = _data.first;

        YAML::Node _spriteDataNode = _root["AnimationData"][_spriteKey];

        if(_spriteDataNode)
        {
            for(YAML::const_iterator _it = _spriteDataNode.begin(); _it != _spriteDataNode.end(); ++_it)
            {
                auto _animationName = (*_it).first.as<std::string>();

                std::cout << _animationName << std::endl;//Idle, Run

                auto _animationData = _it->second;

                auto _animationSpeedNode = _animationData["AnimSpeed"];
                auto _clipNodes          = _animationData["Clips"];


                spriteSheetData[_spriteKey].SetAnimationSpeed(_animationName, _animationSpeedNode.as<float>());

                for(YAML::const_iterator _j = _clipNodes.begin(); _j != _clipNodes.end(); ++_j)
                {
                    auto _clipDataNode = (*_j);

                    SDL_Rect _clipRect
                            {
                                    _clipDataNode[0].as<int>(),
                                    _clipDataNode[1].as<int>(),
                                    _clipDataNode[2].as<int>(),
                                    _clipDataNode[3].as<int>(),
                            };

                    spriteSheetData[_spriteKey].AddAnimationClip(_animationName, _clipRect);
                }
            }
        }
    }
}

void Engine::GenerateEntities()
{
    m_EntityManager.Reset();

    for(const Tile& _tile : m_TileEditor->tileGridCanvas->TileLayers[0])
    {
        if(_tile.sprite.spriteSheet == nullptr)
            continue;

        ENTITY _e = m_EntityManager.CreateEntity();

        TransformComponent _transformComponent
                {
                    static_cast<float>(_tile.sprite.dstRect.x),
                    static_cast<float>(_tile.sprite.dstRect.y),
                    0.0f,
                    0.0f,
                    _tile.rowIndex,
                    _tile.columnIndex
                };

        SpriteComponent _spriteComponent
                {
                    _tile.sprite.srcRect,
                    _tile.sprite.dstRect,
                    _tile.sprite.spriteSheet,
                    _tile.sprite.isAnimated,
                    0,
                    0.0f,
                    10.f,
                    _tile.sprite.animationClips
                };

        m_EntityManager.Registry.Transforms[_e] = _transformComponent;
        m_EntityManager.Registry.Sprites   [_e] = _spriteComponent;
    }

    //Player Layer
    for(const Tile& _tile : m_TileEditor->tileGridCanvas->TileLayers[1])
    {
        if(_tile.sprite.spriteSheet == nullptr)
            continue;

        ENTITY _e = m_EntityManager.CreateEntity();

        TransformComponent _transformComponent
                {
                    static_cast<float>(_tile.sprite.dstRect.x),
                    static_cast<float>(_tile.sprite.dstRect.y),
                    0.0f,
                    0.0f,
                    _tile.rowIndex,
                    _tile.columnIndex
                };

        SpriteComponent _spriteComponent
                {
                        _tile.sprite.srcRect,
                        _tile.sprite.dstRect,
                        _tile.sprite.spriteSheet,
                        _tile.sprite.isAnimated,
                        0,
                        0.0f,
                        10.f,
                        _tile.sprite.animationClips
                };

        InputComponent _inputComponent { };

        m_EntityManager.Registry.Transforms[_e] = _transformComponent;
        m_EntityManager.Registry.Sprites   [_e] = _spriteComponent;
        m_EntityManager.Registry.Inputs    [_e] = _inputComponent;
    }
}

SDL_Texture* Engine::LoadTexture(const char* _name, const char* _filePath)
{
    SDL_Texture* _texture = IMG_LoadTexture(m_Render, _filePath);

    spriteSheetData[_name] = TileSpriteSheetInfo(_texture, m_Render, _name, _filePath);
    spriteSheetItems.emplace_back(_name);

    return _texture;
}

void Engine::ProcessInput(SDL_Event* _event)
{
    inputManager.ClearInputs();

    while(SDL_PollEvent(_event))
    {
        ImGui_ImplSDL2_ProcessEvent(_event);

        inputManager.ProcessInput(_event);

        switch(_event->type)
        {
            case SDL_QUIT:
                m_IsRunning = false;
                break;
            case SDL_KEYDOWN:
                if(_event->key.keysym.sym == SDLK_ESCAPE)
                {
                    if(m_EngineMode == EngineMode::PLAY_MODE)
                    {
                        m_EngineMode = EngineMode::EDIT_MODE;
                        return;
                    }
                    m_IsRunning = false;
                }
                break;
        }

        m_TileEditor->ProcessInput(_event);
    }
}

SDL_Color ImVec4ToSDLColor(const ImVec4& color) {
    SDL_Color sdlColor;
    sdlColor.r = static_cast<Uint8>(color.x * 255.0f);
    sdlColor.g = static_cast<Uint8>(color.y * 255.0f);
    sdlColor.b = static_cast<Uint8>(color.z * 255.0f);
    sdlColor.a = static_cast<Uint8>(color.w * 255.0f);
    return sdlColor;
}

void Engine::Update()
{
    while(m_IsRunning)
    {
        m_Time.Tick();

        SDL_GetMouseState(&_mousePos.x, &_mousePos.y);
        ProcessInput(&_event);

        switch (m_EngineMode)
        {
            case EngineMode::EDIT_MODE:
                UpdateEditMode();
                break;
            case EngineMode::PLAY_MODE:
                UpdatePlayMode();
                break;
        }
    }
}

void Engine::UpdatePlayMode()
{
    m_EntityManager.ProcessInput();
    m_EntityManager.Update();

    SDL_SetRenderDrawColor(m_Render, _clearColor.r, _clearColor.g, _clearColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_Render);

    m_EntityManager.Render(m_Render);

    SDL_RenderPresent(m_Render);
}

void Engine::UpdateEditMode()
{
    m_TileEditor->Update(_mousePos);

    SDL_SetRenderDrawColor(m_Render, _clearColor.r, _clearColor.g, _clearColor.b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_Render);

    m_TileEditor->Render(m_Render);

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Engine"))
        {
            if (ImGui::MenuItem("About", "_b", &showAboutInfo))
            {

            }
            if (ImGui::MenuItem("Exit"))
                m_IsRunning = false;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New scene", "Ctrl+n"))
            {
                m_TileEditor->tileGridCanvas->ClearGrid();
                m_TileEditor->tileGridCanvas->GenerateTiles();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
                SaveScene("../savefiles/SceneA.yaml");
            if (ImGui::MenuItem("Load"))
                LoadScene("../savefiles/SceneA.yaml");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            ImGui::MenuItem("Scene manager", "-"   , &showSceneManager);
            ImGui::MenuItem("Entity inspector", "-", &showEntityInspector);
            ImGui::MenuItem("Tile Editor", "Ctrl+O", &showTileEditor);

            if (ImGui::MenuItem("Full screen", "Ctrl+w", &fullScreen))
                SDL_SetWindowFullscreen(m_Window, fullScreen ? SDL_WINDOW_FULLSCREEN : 0);

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showAboutInfo) {
        ImGui::OpenPopup("ABOUT MELT ENGINE");
        if (ImGui::BeginPopupModal("ABOUT MELT ENGINE", &showAboutInfo, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("MELT ENGINE IS TILE BASE EDITOR WITH ECS ARCHITECTURE.\n FEEL FREE TO USE AS PLEASE.\n CONTACT : sukumduangpattra@gmail.com");
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

    if(showSceneManager)
        m_SceneManager->RenderEditorGUI();

    if(showEntityInspector)
    {
        ImGui::Begin("Entity inspector", &showEntityInspector);

        //Selected tile current layer
        //Name
        //Tile component
        //Sprite component


        ImGui::End();
    }

    if(showTileEditor)
    {
        ImGui::Begin("Tile Map Editor", &showTileEditor);

        if (ImGui::Button(m_EngineMode == EngineMode::EDIT_MODE ? "Play" : "Stop"))
        {
            if(m_EngineMode == EngineMode::EDIT_MODE)
            {
                m_EngineMode = EngineMode::PLAY_MODE;
                GenerateEntities();
            }

            else if (m_EngineMode == EngineMode::PLAY_MODE)
                m_EngineMode = EngineMode::EDIT_MODE;
        }

        if (ImGui::CollapsingHeader("TILE GRID"))
        {
            ImGui::Text("Gizmos setting");
            ImGui::Checkbox("Display gizmos"        , &m_TileEditor->DisplayGizmos);
            ImGui::Checkbox("Display grid guideline", &m_TileEditor->DisplayGridGuideline);
            ImGui::Checkbox("Display box collider"  , &m_TileEditor->DisplayBoxCollider);
            ImGui::Checkbox("Display flags"         , &m_TileEditor->DisplayFlags);

            ImGui::Text("Color setting");
            if(ImGui::ColorEdit4("Clear color", (float*)&_clearCol))
                _clearColor = ImVec4ToSDLColor(_clearCol);

            if(ImGui::ColorEdit4("Grid color", (float*)&_gridCol))
                m_TileEditor->gridColor = ImVec4ToSDLColor(_gridCol);

            ImGui::Text("Tile grid generation setting");
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Grid size");
            ImGui::InputInt("Grid width" , &m_TileEditor->tileWidthCount);
            ImGui::InputInt("Grid height", &m_TileEditor->tileHeightCount);

            if(ImGui::Button("CLEAR"))
            {
                m_TileEditor->tileGridCanvas->gridWidth  = m_TileEditor->tileWidthCount;
                m_TileEditor->tileGridCanvas->gridHeight = m_TileEditor->tileHeightCount;
                m_TileEditor->tileGridCanvas->ClearGrid();
                m_TileEditor->tileGridCanvas->GenerateTiles();
            }
            ImGui::SameLine();
            if(ImGui::Button("GENERATE"))
            {
                m_TileEditor->tileGridCanvas->gridWidth  = m_TileEditor->tileWidthCount;
                m_TileEditor->tileGridCanvas->gridHeight = m_TileEditor->tileHeightCount;
                m_TileEditor->tileGridCanvas->ClearGrid();
                m_TileEditor->tileGridCanvas->GenerateTiles();
            }

            ImGui::Text("Tile edit setting");
            ImGui::Checkbox("Use animated tile", &m_TileEditor->useAnimatedTile);

            if(ImGui::Combo("Edit Mode", &m_TileEditor->SelectedEditModeIndex, m_TileEditor->EditModeItems, IM_ARRAYSIZE(m_TileEditor->EditModeItems)))
                m_TileEditor->UpdateSelectedEditMode();

            if(ImGui::Combo("Layer", &m_TileEditor->tileGridCanvas->currentLayer, m_TileEditor->LayerItems, IM_ARRAYSIZE(m_TileEditor->LayerItems)))
                m_TileEditor->UpdateLayers();

            if(ImGui::Combo("Tile Flag", &m_TileEditor->SelectedFlagIndex, m_TileEditor->FlagItems, IM_ARRAYSIZE(m_TileEditor->FlagItems)))
            {

            }
        }

        if (ImGui::CollapsingHeader("SPRITE SHEET"))
        {
            ImGui::SeparatorText("Sprite sheet setting");

            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Current modified sprite sheet");

            if (ImGui::BeginCombo("Sprite sheets", spriteSheetItems[selectedSpriteSheetIndex].c_str()))
            {
                for (int i = 0; i < spriteSheetItems.size(); i++)
                {
                    bool isSelected = selectedSpriteSheetIndex == i;

                    if (ImGui::Selectable(spriteSheetItems[i].c_str(), isSelected))
                    {
                        selectedSpriteSheetIndex = i;

                        std::string _selectedKey = spriteSheetItems[selectedSpriteSheetIndex];
                        m_TileEditor->modifiedSpriteSheet = &spriteSheetData[_selectedKey];
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            if(ImGui::InputInt("Tile pixel unit", &m_TileEditor->modifiedSpriteSheet->tileUnitPixelSize))
                m_TileEditor->UpdateScale();

            if(ImGui::SliderInt("Scale", &m_TileEditor->mapScale, 1, 4))
                m_TileEditor->UpdateScale();

            m_TileEditor->RenderEditorGUI();
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(m_Render);
}