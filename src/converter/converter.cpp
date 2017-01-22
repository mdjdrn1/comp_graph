#include "converter.hpp"
#include "../error/error.hpp"
#include "../rle/rle.hpp"
#include "../huffman/Huffman.hpp"
#include "../bitpack/bitpack.hpp"

void Converter::encode(const std::string& filename, const mode& algoritm_mode, const bool& grayscale) const
{
	std::unique_ptr<Coder> encoder;
	encoder->validate_bmp(filename);
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

void Converter::decode(const std::string& filename) const
{
	std::unique_ptr<Coder> decoder;
	decoder->validate_bard(filename);
	auto mode_and_grayscale = get_algorithm_mode_and_grayscale(filename);

	switch (mode_and_grayscale.first)
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
	decoder->decode(filename, mode_and_grayscale.second);
}

/**
* \brief Read from encoded file mode and grayscale
* \param filename path to the file
* \return pair of mode with algorithm mode and bool with grayscale
*/
auto Converter::get_algorithm_mode_and_grayscale(const std::string& filename) const -> std::pair<mode, bool>
{
	std::fstream encoded_file(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!encoded_file.is_open())
		throw Error("In Converter::get_mode_from_header(): couldn't open input convert file.");

	Coder::Header bard_header(encoded_file); // reading Header
	encoded_file.close();
	mode file_mode;
	if (static_cast<mode>(bard_header.compression) == bard_header.compression)
		file_mode = static_cast<mode>(bard_header.compression);
	else
		throw Error("In Converter::get_mode_from_header: invalid header in encoded file.");

	return std::make_pair(file_mode, bard_header.grayscale);
}
