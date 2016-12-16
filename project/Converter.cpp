#include "Converter.h"

Converter::Converter(mode selection, bool grayscale) : m_cur_mode(selection), m_grayscale(grayscale)
{
}

Converter::~Converter()
{
}

/** \brief Run proper converting method for current mode
 *
 * \param filename const std::string& converted file path
 * \return void
 *
 */
void Converter::convert(const std::string& filename)
{
	switch (m_cur_mode)
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
		std::cerr << "Invalid convert mode.\n" << std::endl;
		exit(EXIT_FAILURE);
		break;
	}
}

/** \brief Run proper deconverting method for current mode
 *
 * \param filename const std::string& converted file name
 * \return void
 *
 */
void Converter::deconvert(const std::string& filename)
{
	switch (m_cur_mode)
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
		std::cerr << "Invalid convert mode.\n" << std::endl;
		exit(EXIT_FAILURE);
		break;
	}
}


/** \brief Draw pixels into SDL_Surface image and cleans them up from 'pixels'
 *
 * \param image SDL_Surface* pixels-input surface
 * \param pixels const DataVector& vector with uint8_ts that represent pixels' RGB channels (in BRG order)
 * \param x int& width value for image
 * \param y int& y heigth value for image
 * \return void
 *
 */
void Converter::draw_pixels(SDL_Surface* image, DataVector& pixels, int& x, int& y)
{
    uint8_t* pixelptr = pixels.data();   // first pixels obj pointer
     // calculate how many pixels (from DataVector pixels) are available to draw in surface
    uint left_to_draw = (pixels.size() - pixels.size()%3) / 3;
    while( y < image->h && x < image->w && left_to_draw > 0 )
    {
    	SDL::draw_pixel(image, x, y, pixelptr[2], pixelptr[1], pixelptr[0]);
        pixelptr+=3;
    	++x;
        --left_to_draw;
        if(x == image->w)   // go to next line of image
        {
            x = 0;
            ++y;
        }
    }
    pixels.erase(pixels.begin(), pixels.end() - pixels.size()%3);   // remove drew pixels
}

/** \brief Create bard header
 *
 * \param image SDL_Surface* image to create header from
 * \param compression_mode mode BITPACK, RLE or HUFF
 * \param grayscale int 1 for grayscale, otherwise 0
 * \return Converter::bard_header new header
 *
 */
Converter::bard_header Converter::create_header(SDL_Surface* image, mode compression_mode, int grayscale)
{
	bard_header new_header;

	new_header.offset = sizeof(bard_header);
	new_header.width = image->w;
	new_header.height = image->h;
	new_header.grayscale = grayscale;
	new_header.compression = static_cast<ushort>(compression_mode);

	return new_header;
}


/** \brief Readheader from bard file
 *
 * \param input std::fstream& input file stream
 * \return Converter::bard_header read header
 *
 */
Converter::bard_header Converter::read_header(std::fstream& input)
{
	bard_header new_header;
	input.seekg(0, std::ios_base::beg); // set input file to begin
	input.read(reinterpret_cast<char*>(&new_header), sizeof(new_header));

	return new_header;
}


/** \brief Change mode of converter
 *
 * \param new_mode const mode& BITPACK, RLE or HUFF
 * \param grayscale const bool& 1 if converting image to grayscale, otherwise 0
 * \return void
 *
 */
void Converter::change_mode(const mode& new_mode, const bool& grayscale)
{
	m_cur_mode = new_mode;
	m_grayscale = grayscale;
}


/** \brief Alter pixel into grayscale
 *
 * \param pixel PixArr  array of 3 uint8_ts representing RGB pixel (in BGR order)
 * \return void
 *
 */
void Converter::to_gray(PixArr& pixel) // pixel in BGR order
{
	// using luma formula to calculate "relative luminescence"
	uint8_t luma = static_cast<uint8_t>(pixel[2] * 0.2126 + pixel[1] * 0.7152 + pixel[0] * 0.0722);
	pixel[0] = pixel[1] = pixel[2] = luma;
}
