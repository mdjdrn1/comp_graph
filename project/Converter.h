#ifndef CONVERTER_H
#define CONVERTER_H

#include <SDL/SDL.h>
#include <fstream>
#include "SDL_function.h"
#include "Window.h"
#include <vector>
#include <algorithm>    // std::copy
#include <iterator> // std::next

class Window;   // declaration of Window, as its friend class is Converter

class Converter
{
    public:
        using uint = unsigned int;
        using ushort = unsigned short;
        template <class T>
        using vect = std::vector<T>;

        enum mode { BITPACK, HUFF, RLE };   // TODO: read from header in bard
        mode m_cur_mode;    // current mode
        explicit Converter( mode selection );
        ~Converter();
        Converter(const Converter&) = delete;
        Converter(Converter&&) = delete;
        Converter& operator=(const Converter&) = delete;
        Converter& operator=(Converter&&) = delete;
        #pragma pack(push, 2)	// set struct alignment to 0 bytes to force 54 bytes stuct size
        struct bard_header  // 16 bytes
        {
            uint offset;
            int width;
            int height;
            ushort gray;
            ushort compression;     // 0 1 2
        };
        #pragma pack(pop)
    private:
        bard_header create_header(SDL_Surface* surface, mode_t compression_mode=BITPACK, int grayscale=0);
        bard_header read_header(std::fstream& input);

       	void conv_7(Window* win);  // conversion from 8-bit to 7-bit
        void conv_huffman(Window* win);
        void conv_rle(Window* win);
        void packer(vect<uint8_t>& vals, std::fstream& out_file);   // conv_7 auxiliary method
        void dconv_7(std::string filename);
        void dconv_huffman(std::string filename);
        void dconv_rle(std::string filename);
        vect<uint8_t> unpacker(vect<uint8_t>& vals);   // conv_7 auxiliary method
    public:
        void change_mode(mode new_mode);
        void convert(Window* win); // surface == m_bmp from MainWindow
        void deconvert(std::string filename);
};

#endif // CONVERTER_H
