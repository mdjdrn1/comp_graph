#ifndef CONVERTER_H
#define CONVERTER_H

#include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "sdl_utils.hpp"
#include "error.hpp"

class Converter
{
public:
	enum mode
	{
		BITPACK,
		HUFF,
		RLE
	};

	using ushort = unsigned short;
	using uint = unsigned int;
	using ull = unsigned long long;
	using DataVector = std::vector<uint8_t>; // bytes vector
	using PixArr = SDL_utils::PixArr; // PixArr = std::array<uint8_t, 3>

	explicit Converter(mode choice = BITPACK, bool grayscale = 0);
	~Converter() = default;
	Converter(const Converter&) = delete;
	Converter(Converter&&) = delete;
	Converter& operator=(const Converter&) = delete;
	Converter& operator=(Converter&&) = delete;

#pragma pack(push, 1)	// set struct alignment to 0 bytes to 'turn off' paddings
	struct bardHeader
	{
		// TODO: Add var for offset to Huffman's codes array
		uint offset; // offset to data, should be 16 bytes
		int width;
		int height;
		ushort grayscale; // 0 or 1
		ushort compression; // 0=BITPACK, 1=HUFFMAN, 2=RLE
		void create_from_SDLSurface(SDL_Surface* image, mode compression_mode = BITPACK, int grayscale_choice = 0);
		void create_from_encoded_file(std::fstream& input);
	};
#pragma pack(pop)
private:
	mode m_current_mode; // current mode
	bool m_is_grayscale; // grayscale

	// coding methods
	// 8 to 7 bits
	void conv_7(const std::string& filename);
	DataVector packer(DataVector& raw_bytes) const; // auxiliary method
	// Huffman
	void conv_huffman(const std::string& filename);
	// RLE
	void conv_rle(const std::string& filename);
	// decoding methods
	// 8 to 7 bits
	void dconv_7(const std::string& filename);
	DataVector unpacker(DataVector& encoded_bytes) const; // auxiliary method
	void draw_pixels(SDL_Surface* image, DataVector& pixels, int& x, int& y); // auxiliary method
	// Huffman
	void dconv_huffman(const std::string& filename);
	// RLE
	void dconv_rle(const std::string& filename);
public:
	void convert(const std::string& filename);
	void deconvert(const std::string& filename);

	void change_mode(const mode& new_mode, const bool& grayscale); // change coding algorithm
	void to_gray(PixArr& pixel); // convert pixel into grayscale
};

#endif // CONVERTER_H
