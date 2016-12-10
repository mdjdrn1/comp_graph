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
        using DataVector = std::vector<uint8_t>;

        enum mode { BITPACK, HUFF, RLE };
        mode m_cur_mode;    // current mode
        explicit Converter( mode selection );
        ~Converter();
        Converter(const Converter&) = delete;
        Converter(Converter&&) = delete;
        Converter& operator=(const Converter&) = delete;
        Converter& operator=(Converter&&) = delete;
        #pragma pack(push, 0)	// set struct alignment to 0 bytes to force 16 bytes stuct size
        struct bard_header
        {
            uint offset;            // offset to data, should be 16 bytes
            int width;
            int height;
            ushort gray;            // grayscale
            ushort compression;     // 0=BITPACK, 1=HUFFMAN, 2=RLE
        };
        #pragma pack(pop)
    private:
        // creating and reading header
        bard_header create_header(SDL_Surface* image, mode compression_mode=BITPACK, int grayscale=0);
        bard_header read_header(std::fstream& input);
        // coding methods
       	void conv_7(const std::string& filename);  // conversion from 8-bit to 7-bit
        void packer(DataVector& vals, std::fstream& out_file);   // conv_7 auxiliary method
        void conv_huffman(const std::string& filename);
        void conv_rle(const std::string& filename);
        // decoding methods
        void dconv_7(const std::string& filename);
        DataVector unpacker(DataVector& vals);   // dconv_7 auxiliary method
        void draw_pixels(SDL_Surface* image, DataVector& pixels);

        void dconv_huffman(const std::string& filename);
        void dconv_rle(const std::string& filename);
    public:
        void change_mode(mode new_mode);
        void convert(const std::string& filename);
        void deconvert(const std::string& filename);
};

#endif // CONVERTER_H
