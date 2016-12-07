#ifndef CONVERTER_H
#define CONVERTER_H

#include <SDL/SDL.h>
#include <fstream>
#include "SDL_function.h"
#include "Window.h"
#include <vector>
#include <algorithm>    // std::copy
#include <iterator> // std::make_move_iterator

class Window;   // declaration of Window, as its friend class is Converter

class Converter
{
    public:
        using uint = unsigned int;
        template <class T>
        using vect = std::vector<T>;

        enum mode { BITPACK, HUFF, RLE };
        mode m_cur_mode;    // current mode
        explicit Converter( mode selection );
        ~Converter();
        Converter(const Converter&) = delete;
        Converter(Converter&&) = delete;
        Converter& operator=(const Converter&) = delete;
        Converter& operator=(Converter&&) = delete;
    private:
       	void conv_7(Window* win);  // conversion from 8-bit to 7-bit
        void conv_huffman(Window* win);
        void conv_rle(Window* win);
        void packer(vect<uint8_t>& vals, std::fstream& out_file);   // conv_7 auxiliary method
    public:
        void change_mode(mode new_mode);
        void convert(Window* win); // surface == m_bmp from MainWindow
};

#endif // CONVERTER_H
