#ifndef SDL_FUNCTION___
#define SDL_FUNCTION___

#include <SDL/SDL.h>
#include <vector>

namespace SDL
{
    Uint32 getpixel(SDL_Surface *surface, int x, int y);
    void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
    uint8_t* getpixel2(SDL_Surface* surf, int x, int y);
}

#endif
