#ifndef CONVERTER_HPP___
#define CONVERTER_HPP___

#include "../error/error.hpp"
#include "../rle/rle.hpp"
#include "../huffman/Huffman.hpp"
#include "../bitpack/bitpack.hpp"
#include "../coder/coder.hpp"

class Converter
{
public:
	using mode = Coder::mode;

	Converter() = default;
	~Converter() = default;
	Converter(const Converter&) = default;
	Converter(Converter&&) = default;
	Converter& operator=(const Converter&) = default;
	Converter& operator=(Converter&&) = default;

	void encode(const std::string& filename, const mode& algoritm_mode, const bool& grayscale = false) const;
	void decode(const std::string& filename, const mode& algoritm_mode) const;
};

#endif
