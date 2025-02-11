#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "Engine.h"

int main()
{
    Engine _engine;
    _engine.LoadAssets();
    _engine.Update();
    return 0;
}
