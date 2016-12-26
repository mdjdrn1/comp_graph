#include "decoder.hpp"

Decoder::Decoder(mode choice) : Coder(choice)
{
}

/**
* \brief Run proper deconverting method for current mode
* \param filename path to file that will be deconverted
*/
void Decoder::decode(const std::string& filename)
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
		throw Error("In Decoder::decode(): invalid convert mode.");	// should never happen, but just in case
	}
}
