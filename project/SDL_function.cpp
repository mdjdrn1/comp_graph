#include "SDL_function.h"
#include <iostream>

/** \brief Gets pixel RGB values from SDL_Surface
 *
 * \param surface SDL_Surface* image
 * \param x const int& x pos (indexing from 0)
 * \param y const int& y pos (indexing from 0)
 * \return Uint32 var with pixel values (RGBA)
 *
 */
Uint32 SDL::get_pixel(SDL_Surface *surface, const int& x, const int& y)
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

/** \brief Draws pixel into SDL_Surface
 *
 * \param surface SDL_Surface* image
 * \param x const int& x pos (indexing from 0)
 * \param y const int& y pos (indexing from 0)
 * \param R const uint8_t& R value
 * \param G const uint8_t& G value
 * \param B const uint8_t& B value
 * \return void
 *
 */
void SDL::draw_pixel(SDL_Surface *surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B)
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

/** \brief Gets pixel RGB values from SDL_Surface
 *
 * \param surface SDL_Surface* image
 * \param x const int& x pos (indexing from 0)
 * \param y const int& y pos (indexing from 0)
 * \return uint8_t* 3-item array with RGB values
 *
 */
SDL::PixArr SDL::get_pixel2(SDL_Surface* surface, const int& x, const int& y)
{
    PixArr pixel_array; // res will contain RGB values
    Uint32 pixel = SDL::get_pixel(surface, x, y);
    SDL_GetRGB(pixel, surface->format, &pixel_array[0], &pixel_array[1], &pixel_array[2]);

    return std::move(pixel_array);
}

/** \brief creating new SDL_Surface from existing file
 *
 * \param filename const std::string& file name
 * \return SDL_Surface* new surface
 *
 */
SDL_Surface* SDL::new_bmp_surface(const std::string& filename)
{
    SDL_Surface* surface = SDL_LoadBMP(filename.c_str());
    if (!surface)
    {
        std::cerr << "Unable to load bitmap: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
    return surface;
}

/** \brief creating new empty SDL_Surface
 *
 * \param width const int& width of new surface
 * \param height const int& height of new surface
 * \return SDL_Surface* new surface
 *
 */
SDL_Surface* SDL::new_empty_surface(const int& width, const int& height)
{
    int bit_depth = 24;

    Uint32 Rmask = 0x00ff0000; // red mask for the pixels
    Uint32 Gmask = 0x0000ff00; // green mask for the pixels
    Uint32 Bmask = 0x000000ff; // blue mask for the pixels
    Uint32 Amask = 0xff000000; // alpha mask for the pixels

    SDL_Surface* surf = SDL_CreateRGBSurface(0, width, height, bit_depth, Rmask, Gmask, Bmask, Amask);
    return surf;
}

/** \brief Delete properly SDL_Surface
 *
 * \param surf SDL_Surface* surface
 * \return void
 *
 */
void SDL::delete_surface(SDL_Surface* surf)
{
    SDL_FreeSurface(surf);
}
