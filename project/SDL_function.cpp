#include "SDL_function.h"
#include <iostream>

Uint32 SDL::get_pixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void SDL::draw_pixel(SDL_Surface *surface, int x, int y, uint8_t R, uint8_t G, uint8_t B)
{
    Uint32 pixel = SDL_MapRGB(surface->format, R, G, B);

    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

uint8_t* SDL::get_pixel2(SDL_Surface* surf, int x, int y)
{
    uint8_t* res = new uint8_t [3]; // res will contain RGB values
    Uint32 pixel = SDL::get_pixel(surf, x, y);
    SDL_GetRGB(pixel, surf->format, res, res+1, res+2);

    return res;
}

// create new surface from existing SDL file
SDL_Surface* SDL::new_bmp_surface(const std::string& filename)
{
    SDL_Surface* surf = SDL_LoadBMP(filename.c_str());
    if (!surf)
    {
        std::cerr << "Unable to load bitmap: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    return surf;
}

SDL_Surface* SDL::new_empty_surface(int width, int height)
{
    int bit_depth = 24;

    Uint32 Rmask = 0x00ff0000; // red mask for the pixels
    Uint32 Gmask = 0x0000ff00; // green mask for the pixels
    Uint32 Bmask = 0x000000ff; // blue mask for the pixels
    Uint32 Amask = 0xff000000; // alpha mask for the pixels

    SDL_Surface* surf = SDL_CreateRGBSurface(0, width, height, bit_depth, Rmask, Gmask, Bmask, Amask);
    return surf;
}

void SDL::delete_surface(SDL_Surface* surf)
{
    SDL_FreeSurface(surf);
}
