#ifndef CONVERTER_H
#define CONVERTER_H

#include <SDL/SDL.h>
#include <fstream>
#include "SDL_function.h"
#include "Window.h"
#include <vector>
#include <algorithm>
#include <iterator>

class Converter
{
public:
	enum mode
	{
		BITPACK,
		HUFF,
		RLE
	};
    using uint = unsigned int;
	using ushort = unsigned short;
	using DataVector = std::vector<uint8_t>;
	using PixArr = SDL::PixArr; // PixArr = std::array<uint8_t, 3>

	explicit Converter(mode selection = BITPACK, bool grayscale = 0);
	~Converter();
	Converter(const Converter&) = delete;
	Converter(Converter&&) = delete;
	Converter& operator=(const Converter&) = delete;
	Converter& operator=(Converter&&) = delete;

#pragma pack(push, 0)	// set struct alignment to 0 bytes to 'turn off' paddings
	struct bard_header
	{
		// TODO: Add var for offset to Huffman's codes array
		uint offset; // offset to data, should be 16 bytes
		int width;
		int height;
		ushort grayscale; // 0 or 1
		ushort compression; // 0=BITPACK, 1=HUFFMAN, 2=RLE
	};
#pragma pack(pop)
private:
	mode m_cur_mode; // current mode
	bool m_grayscale; // grayscale

	// coded file header
	bard_header create_header(SDL_Surface* image, mode compression_mode = BITPACK, int grayscale = 0);
	bard_header read_header(std::fstream& input);

	// coding methods
	// 8 to 7 bits
	void conv_7(const std::string& filename);
	DataVector packer(DataVector& vals); // auxiliary method
    // Huffman
	void conv_huffman(const std::string& filename);
	// RLE
	void conv_rle(const std::string& filename);
	// decoding methods
	// 8 to 7 bits
	void dconv_7(const std::string& filename);
	DataVector unpacker(DataVector& vals); // auxiliary method
	void draw_pixels(SDL_Surface* image, DataVector& pixels, int& x, int& y); // auxiliary method
    // Huffman
	void dconv_huffman(const std::string& filename);
	// RLE
	void dconv_rle(const std::string& filename);
public:
	void convert(const std::string& filename);
	void deconvert(const std::string& filename);

	void change_mode(const mode& new_mode, const bool& grayscale); // change coding algorithm
	void to_gray(PixArr& pixel);    // convert pixel into grayscale
};

#endif // CONVERTER_H
