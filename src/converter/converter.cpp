#include "converter.hpp"

void Converter::encode(const std::string& filename, const mode& algoritm_mode, const bool& grayscale) const
{
	std::unique_ptr<Coder> encoder = nullptr;
	switch (algoritm_mode)
	{
	case mode::BITPACK:
		encoder = std::make_unique<Bitpack>();
		break;
	case mode::HUFF:
		encoder = std::make_unique<Huffman>();
		break;
	case mode::RLE_EN:
		encoder = std::make_unique<RLE>();
		break;
	default:
		throw Error("In Converter::encode(): invalid algorithm mode."); // should never happen, but just in case!
	}
	encoder->encode(filename, grayscale);
}

void Converter::decode(const std::string& filename, const mode& algoritm_mode) const
{
	std::unique_ptr<Coder> decoder = nullptr;
	// TODO: find mode in header > run proper algorithm
	switch (algoritm_mode)
	{
	case mode::BITPACK:
		decoder = std::make_unique<Bitpack>();
		break;
	case mode::HUFF:
		decoder = std::make_unique<Huffman>();
		break;
	case mode::RLE_EN:
		decoder = std::make_unique<RLE>();
		break;
	default:
		throw Error("In Converter::decode(): invalid algorithm mode."); // should never happen, but just in case!
	}
	decoder->decode(filename);
}
