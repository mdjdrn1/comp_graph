#ifndef ENCODDER_H___
#define ENCODDER_H___

#include <SDL.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "../sdl_utils/sdl_utils.hpp"
#include "../error/error.hpp"
#include "coder.hpp"

class Encoder : public Coder
{
public:
	explicit Encoder(mode choice = BITPACK, bool grayscale = 0);
	~Encoder() = default;
	Encoder(const Encoder&) = delete;
	Encoder(Encoder&&) = delete;
	Encoder& operator=(const Encoder&) = delete;
	Encoder& operator=(Encoder&&) = delete;

	void change_mode(const mode& new_mode = BITPACK, const bool& grayscale = 0);
private:
	bool m_is_grayscale; // grayscale

// coding methods
// Bitpack - 8 to 7 bits
	void bitpack(const std::string& filename);
	DataVector packer(DataVector& raw_bytes) const; // auxiliary method
// Huffman
	void huffman(const std::string& filename);
// RLE
	void rle(const std::string& filename);
public:
	void encode(const std::string& filename);
};

#endif // ENCODDER_H___
