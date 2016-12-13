#ifndef SDL_FUNCTION___
#define SDL_FUNCTION___

#include <SDL/SDL.h>
#include <vector>
#include <string>
#include <array>

namespace SDL
{
    using PixArr = std::array<uint8_t, 3>;

    Uint32 get_pixel(SDL_Surface *surface, const int& x, const int& y);   // get pixel from SDL_surface
    PixArr get_pixel2(SDL_Surface* surf, const int& x, const int& y);
    void draw_pixel(SDL_Surface *surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B);
    SDL_Surface* new_bmp_surface(const std::string& filename);
    SDL_Surface* new_empty_surface(const int& width, const int& height);
    void delete_surface(SDL_Surface* surf);
}


#endif
