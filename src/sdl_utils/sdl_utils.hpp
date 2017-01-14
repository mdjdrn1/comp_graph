#ifndef SDL_UTILS___
#define SDL_UTILS___

#include <SDL.h>
#include <array>
#include "../error/error.hpp"

namespace SDL_utils
{
	using Pixel = std::array<uint8_t, 3>;

	Pixel get_pixel(SDL_Surface* surface, const int& x, const int& y);
	void draw_pixel(SDL_Surface* surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B);
	SDL_Surface* new_bmp_surface(const std::string& filename);
	SDL_Surface* new_empty_surface(const int& width, const int& height);
	void delete_surface(SDL_Surface* surface);
	void to_gray_pixel(Pixel& pixel);
}

#endif
