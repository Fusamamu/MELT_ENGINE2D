#include "InputManager.h"

InputManager::InputManager():
previousKey(-1),
currentKey(-1),
mousePressed(false),
mouseReleased(true),
leftMousePressed(false),
leftMouseReleased(true),
rightMousePressed(false),
rightMouseReleased(true)
{

}

InputManager::~InputManager()
{

}

void InputManager::ProcessInput(SDL_Event* _event)
{
    switch(_event->type)
    {
        case SDL_KEYDOWN:

            if(previousKey == currentKey)
                break;
            previousKey = currentKey;

            switch (_event->key.keysym.sym)
            {
                case SDLK_w:
                case SDLK_UP:
                    currentKey = SDLK_w;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    currentKey = SDLK_a;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    currentKey = SDLK_s;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    currentKey = SDLK_d;
                    break;
            }
            break;

        case SDL_KEYUP:
            previousKey = -1;
            currentKey  = -1;
            break;

        case SDL_MOUSEBUTTONDOWN:

            if(mouseReleased)
            {
                mousePressed = true;
                mouseReleased = false;
            }

            if(_event->button.button == SDL_BUTTON_LEFT)
            {
                if(leftMouseReleased)
                {
                    leftMousePressed  = true;
                    leftMouseReleased = false;
                }
            }

            if(_event->button.button == SDL_BUTTON_RIGHT)
            {
                if(rightMouseReleased)
                {
                    rightMousePressed  = true;
                    rightMouseReleased = false;
                }
            }

            break;
        case SDL_MOUSEBUTTONUP:

            mouseReleased = true;

            if (_event->button.button == SDL_BUTTON_LEFT)
                leftMouseReleased  = true;
            if (_event->button.button == SDL_BUTTON_RIGHT)
                rightMouseReleased = true;
            break;
    }
}

bool InputManager::IsPressedThisFrame(int _key)
{
    return _key == currentKey;
}

void InputManager::ClearInputs()
{
    mousePressed       = false;
    leftMousePressed   = false;
    rightMousePressed  = false;
}