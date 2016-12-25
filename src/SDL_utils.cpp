#include "sdl_utils.hpp"
#include <iostream>


/** \brief Get pixel RGB values from SDL_Surface
*
* \param surface SDL_Surface* image
* \param x const int& x pos (indexing from 0)
* \param y const int& y pos (indexing from 0)
* \return SDL_utils::PixArr 3-item array with RGB values
*
*/
SDL_utils::PixArr SDL_utils::get_pixel(SDL_Surface* surface, const int& x, const int& y)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* pixelPtr = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp; // addres to pixel to get
	Uint32 pixel = *(reinterpret_cast<Uint32 *>(pixelPtr));
	PixArr pixel_array; // pixel_array will contain RGB values
	SDL_GetRGB(pixel, surface->format, &pixel_array[0], &pixel_array[1], &pixel_array[2]);

	return std::move(pixel_array);
}

/** \brief Draw pixel into SDL_Surface
 *
 * \param surface SDL_Surface* image
 * \param x const int& x pos (indexing from 0)
 * \param y const int& y pos (indexing from 0)
 * \param R const uint8_t& R value
 * \param G const uint8_t& G value
 * \param B const uint8_t& B value
 *
 */
void SDL_utils::draw_pixel(SDL_Surface* surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B)
{
	Uint32 pixel = SDL_MapRGB(surface->format, R, G, B);

	int bpp = surface->format->BytesPerPixel;
	Uint8* p = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp; // ptr to pixel we want to set

	p[0] = pixel & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = (pixel >> 16) & 0xff;
}

/** \brief Create new SDL_Surface from existing BMP file
 *
 * \param filename const std::string& BMP file name
 * \return SDL_Surface* new surface
 *
 */
SDL_Surface* SDL_utils::new_bmp_surface(const std::string& filename)
{
	SDL_Surface* surface = SDL_LoadBMP(filename.c_str());
	if (!surface)
		throw Error("In SDL_utils::new_bmp_surface(): Unable to load bitmap from file: " + filename + ".");
	
	return surface;
}

/** \brief Create new empty SDL_Surface
 *
 * \param width const int& width of new surface
 * \param height const int& height of new surface
 * \return SDL_Surface* new surface
 *
 */
SDL_Surface* SDL_utils::new_empty_surface(const int& width, const int& height)
{
	int bit_depth = 24;

	Uint32 Rmask = 0x00ff0000; // red mask for the pixels
	Uint32 Gmask = 0x0000ff00; // green mask for the pixels
	Uint32 Bmask = 0x000000ff; // blue mask for the pixels
	Uint32 Amask = 0xff000000; // alpha mask for the pixels

	SDL_Surface* surf = SDL_CreateRGBSurface(0, width, height, bit_depth, std::move(Rmask), std::move(Gmask), std::move(Bmask), std::move(Amask));
	
	if (!surf)
		throw Error("In SDL_utils::new_empty_surface(): failed to create new surface.");
	
	return surf;
}

/** \brief Delete properly SDL_Surface
 *
 * \param surface SDL_Surface* surface
 *
 */
void SDL_utils::delete_surface(SDL_Surface* surface)
{
	SDL_FreeSurface(surface);
}
