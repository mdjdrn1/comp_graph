#ifndef CONVERTER_H
#define CONVERTER_H

#include <SDL/SDL.h>
#include <fstream>
#include "SDL_function.h"
#include "Window.h"
#include <vector>
#include <algorithm>    // std::copy
#include <iterator> // std::next

class Converter
{
    public:
        enum mode { BITPACK, HUFF, RLE };

        explicit Converter(mode selection = BITPACK, bool grayscale = 0);
        ~Converter();
        Converter(const Converter&) = delete;
        Converter(Converter&&) = delete;
        Converter& operator=(const Converter&) = delete;
        Converter& operator=(Converter&&) = delete;
    public:
        using uint = unsigned int;
        using ushort = unsigned short;
        using DataVector = std::vector<uint8_t>;

        #pragma pack(push, 0)	// set struct alignment to 0 bytes to force 14 bytes stuct size
        struct bard_header
        {
            uint offset;            // offset to data, should be 14 bytes
            int width;
            int height;
            ushort grayscale;    // 0 or 1
            ushort compression;     // 0=BITPACK, 1=HUFFMAN, 2=RLE
        };
        #pragma pack(pop)
    private:
        mode m_cur_mode;    // current mode
        bool m_grayscale;   // grayscale

        // creating and reading header
        bard_header create_header(SDL_Surface* image, mode compression_mode=BITPACK, int grayscale=0);
        bard_header read_header(std::fstream& input);
        // coding methods
       	void conv_7(const std::string& filename);  // conversion from 8-bit to 7-bit
        DataVector packer(DataVector& vals);   // conv_7 auxiliary method
        void conv_huffman(const std::string& filename);
        void conv_rle(const std::string& filename);
        // decoding methods
        void dconv_7(const std::string& filename);
        DataVector unpacker(DataVector& vals);   // dconv_7 auxiliary method
        void draw_pixels(SDL_Surface* image, DataVector& pixels);

        void dconv_huffman(const std::string& filename);
        void dconv_rle(const std::string& filename);
    public:
        void convert(const std::string& filename);
        void deconvert(const std::string& filename);

        void change_mode(mode new_mode, bool grayscale);    // change coding algorithm
        void to_gray(uint8_t* pixel);
};

#endif // CONVERTER_H
