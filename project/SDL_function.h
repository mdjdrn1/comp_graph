#ifndef SDL_FUNCTION___
#define SDL_FUNCTION___

#include <SDL/SDL.h>
#include <vector>

namespace SDL
{
    Uint32 get_pixel(SDL_Surface *surface, int x, int y);   // get pixel from SDL_surface
    uint8_t* get_pixel2(SDL_Surface* surf, int x, int y);
    void draw_pixel(SDL_Surface *surface, int x, int y, uint8_t R, uint8_t G, uint8_t B);
    SDL_Surface* new_bmp_surface(const char* filename);
    SDL_Surface* new_empty_surface(int width, int height);
    void delete_surface(SDL_Surface* surf);
}


#endif
