#include "converter.hpp"


Converter::Converter(mode choice, bool grayscale) : m_current_mode(choice), m_is_grayscale(grayscale)
{
}

/**
 * \brief Run proper converting method for current mode
 * \param filename path to file that will be converted
 */
void Converter::convert(const std::string& filename)
{
	switch (m_current_mode)
	{
	case BITPACK:
		conv_7(filename);
		break;
	case HUFF:
		conv_huffman(filename);
		break;
	case RLE:
		conv_rle(filename);
		break;
	default:
		throw Error("In Converter::convert(): invalid convert mode.");	// should never happen, but just in case
	}
}

/**
 * \brief Run proper deconverting method for current mode
 * \param filename path to file that will be deconverted
 */
void Converter::deconvert(const std::string& filename)
{
	switch (m_current_mode)
	{
	case BITPACK:
		dconv_7(filename);
		break;
	case HUFF:
		dconv_huffman(filename);
		break;
	case RLE:
		dconv_rle(filename);
		break;
	default:
		throw Error("In Converter::deconvert(): invalid convert mode.");	// should never happen, but just in case
	}
}

/**
 * \brief Draw pixels into SDL_Surface image and cleans them up from 'pixels'
 * \param image pixels-input surface
 * \param pixels vector with uint8_ts that represent pixels' RGB channels (in BRG order)
 * \param x width value for image
 * \param y heigth value for image
 */
void Converter::draw_pixels(SDL_Surface* image, DataVector& pixels, int& x, int& y)
{
	uint8_t* pixelptr = pixels.data(); // first pixels obj pointer
	// calculate how many pixels (from DataVector pixels) are available to draw in surface
	ull left_to_draw = static_cast<ull>((pixels.size() - pixels.size() % 3) / 3);
	while (y < image->h && x < image->w && left_to_draw > 0)
	{
		SDL_utils::draw_pixel(image, x, y, pixelptr[2], pixelptr[1], pixelptr[0]);
		pixelptr += 3;
		++x;
		--left_to_draw;
		if (x == image->w) // go to next line of image
		{
			x = 0;
			++y;
		}
	}
	pixels.erase(pixels.begin(), pixels.end() - pixels.size() % 3); // remove drew pixels
}

/**
 * \brief Change mode of converter
 * \param new_mode BITPACK, RLE or HUFF
 * \param grayscale 1 if converting image to grayscale, otherwise 0
 */
void Converter::change_mode(const mode& new_mode, const bool& grayscale)
{
	m_current_mode = new_mode;
	m_is_grayscale = grayscale;
}

/**
 * \brief Alter pixel into grayscale
 * \param pixel array of 3 uint8_ts representing RGB pixel (in BGR order)
 */
void Converter::to_gray(PixArr& pixel) // pixel in BGR order
{
	// using luma formula to calculate "relative luminescence"
	uint8_t luma = static_cast<uint8_t>(pixel[2] * 0.2126 + pixel[1] * 0.7152 + pixel[0] * 0.0722);
	pixel[0] = pixel[1] = pixel[2] = luma;
}


// BARD HEADER METHODS' DEFINITIONS

/**
 * \brief Create bard header
 * \param image image to create header from
 * \param compression_mode mode BITPACK, RLE or HUFF
 * \param grayscale_choice int 1 for grayscale, otherwise 0
 * \return new header
 */
void Converter::bardHeader::create_from_SDLSurface(SDL_Surface* image, mode compression_mode, int grayscale_choice)
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
void Converter::bardHeader::create_from_encoded_file(std::fstream& input)
{
	input.seekg(0, std::ios_base::beg); // set input file to begin
	input.read(reinterpret_cast<char*>(this), sizeof(*this));
}
