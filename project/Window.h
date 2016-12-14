#ifndef WINDOW_H___
#define WINDOW_H___

#include <SDL/SDL.h>
#include <cstdlib>  // exit()
#include <string>
#include <iostream>
#include <fstream>
#include "Converter.h"

class Window
{
public:
	using uint = unsigned int;

	explicit Window(std::string filename);
	explicit Window(SDL_Surface* image);
	~Window();
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;
private:
	uint m_width;
	uint m_height;
	std::string m_filename;

	SDL_Surface* m_screen;
	SDL_Surface* m_bmp;
	SDL_Rect* m_dstrect; // for centering bitmap on the screen
	void init();
	void create_window();
	void load_image();
	void set_dimensions(); // m_width and m_height init
public:
	void draw_image();

	inline uint get_width() { return m_width; };
	inline uint get_height() { return m_height; };
};

#endif // WINDOW_H___
