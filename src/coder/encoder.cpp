#include "encoder.hpp"


Encoder::Encoder(mode choice, bool grayscale) : Coder(choice), m_is_grayscale(grayscale)
{
}

/**
* \brief Change mode of converter
* \param new_mode coding algorithm BITPACK, RLE or HUFFMAN
* \param grayscale 1 if converting image to grayscale, otherwise 0
*/
void Encoder::change_mode(const mode& new_mode, const bool& grayscale)
{
	m_current_mode = new_mode;
	m_is_grayscale = grayscale;
}

/**
 * \brief Run proper converting method for current mode
 * \param filename path to file that will be converted
 */
void Encoder::encode(const std::string& filename)
{
	switch (m_current_mode)
	{
	case BITPACK:
		bitpack(filename);
		break;
	case HUFFMAN:
		huffman(filename);
		break;
	case RLE:
		rle(filename);
		break;
	default:
		throw Error("In Encoder::encode(): invalid encode mode.");	// should never happen, but just in case
	}
}
