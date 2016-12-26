#include "coder.hpp"

Coder::Coder(mode choice) : m_current_mode(choice)
{

}

Coder::~Coder()
{
	
}

/**
* \brief Change mode of converter
* \param new_mode coding algorithm BITPACK, RLE or HUFFMAN
*/
void Coder::change_mode(const mode& new_mode)
{
	m_current_mode = new_mode;
}

/**
* \brief Create bard header
* \param image image to create header from
* \param compression_mode mode BITPACK, RLE or HUFFMAN
* \param grayscale_choice int 1 for grayscale, otherwise 0
* \return new header
*/
void Coder::bardHeader::create_from_SDLSurface(SDL_Surface* image, mode compression_mode, int grayscale_choice)
{
	offset = sizeof(bardHeader);
	width = image->w;
	height = image->h;
	grayscale = grayscale_choice;
	compression = static_cast<ushort>(compression_mode);
}

/**
* \brief Readheader from bard file
* \param input input file stream
* \return read header
*/
void Coder::bardHeader::create_from_encoded_file(std::fstream& input)
{
	// TODO: add checking if file is correct
	input.seekg(0, std::ios_base::beg); // set input file to begin
	input.read(reinterpret_cast<char*>(this), sizeof(*this));
}
