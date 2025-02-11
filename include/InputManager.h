#pragma once
#include <SDL2/SDL.h>

struct InputManager
{
    int currentKey;
    int previousKey;

    bool mousePressed;
    bool mouseReleased;

    bool leftMousePressed;
    bool leftMouseReleased;

    bool rightMousePressed;
    bool rightMouseReleased;

    InputManager();
    ~InputManager();

    void ProcessInput(SDL_Event* _event);
    bool IsPressedThisFrame(int _key);

    void ClearInputs();
};