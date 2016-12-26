#ifndef DECODDER_H___
#define DECODDER_H___

#include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "../sdl_utils/sdl_utils.hpp"
#include "../error/error.hpp"
#include "coder.hpp"

class Decoder : public Coder
{
public:
	explicit Decoder(mode choice = BITPACK);
	Decoder(const Decoder&) = delete;
	Decoder(Decoder&&) = delete;
	Decoder& operator=(const Decoder&) = delete;
	Decoder& operator=(Decoder&&) = delete;
private:
// decoding methods
// Bitpack - 8 to 7 bits
	void bitpack(const std::string& filename);
	DataVector unpacker(DataVector& encoded_bytes) const; // auxiliary method
	void draw_pixels(SDL_Surface* image, DataVector& pixels, int& x, int& y); // auxiliary method
// Huffman
	void huffman(const std::string& filename);
// RLE
	void rle(const std::string& filename);
public:
	void decode(const std::string& filename);
};

#endif // DECODER_H___
