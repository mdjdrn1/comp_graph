#include "coder.hpp"


/*
* \brief Header ctor(for encoding)
* \param image image to create header from
* \param compression_mode mode BITPACK, RLE or HUFFMAN
* \param grayscale_choice int 1 for grayscale, otherwise 0
*/
Coder::Header::Header(const int& h, const int& w, const mode& compression_mode, const bool& grayscale_choice)
	: offset(sizeof(Header)),
	  width(w),
	  height(h),
	  grayscale(grayscale_choice),
	  compression(static_cast<ushort>(compression_mode))
{
}

/*
* \brief Header ctor (for encoding)
* \param image image to create header from
* \param compression_mode mode BITPACK, RLE or HUFFMAN
* \param grayscale_choice int 1 for grayscale, otherwise 0
*/
Coder::Header::Header(SDL_Surface* image, const mode& compression_mode, const bool& grayscale_choice)
	: offset(sizeof(Header)),
	  width(image->w),
	  height(image->h),
	  grayscale(grayscale_choice),
	  compression(static_cast<ushort>(compression_mode))
{
}

/*
* \brief Header ctor (for decoding)
* \param input input file stream
*/
Coder::Header::Header(std::fstream& input)
{
	input.seekg(0, std::ios_base::beg); // set input file to begin
	input.read(reinterpret_cast<char*>(this), sizeof(*this));
}

/**
 * \brief Generates name of file after encoding
 * \param input_filename input file name
 * \return encoded file name
 */
std::string Coder::encoded_filename(const std::string& input_filename) const
{
	return std::move(input_filename.substr(0, input_filename.size() - 3) + "bard"); // output encoded file name
}

/**
* \brief Generates name of file after decoding
* \param input_filename input file name
* \return decoded file name
*/
std::string Coder::decoded_filename(const std::string& input_filename) const
{
	return std::move(input_filename.substr(0, input_filename.size() - 5) + "_decoded.bmp"); // output decoded file name
}

/**
* \brief Draw pixels into SDL_Surface image and cleans them up from 'pixels'
* \param image pixels-input surface
* \param pixels vector with uint8_ts that represent pixels' RGB channels (in BRG order)
* \param x width value for image
* \param y heigth value for image
*/
void Coder::draw_pixels(const SDL_Surface& image, DataVector& pixels, int& x, int& y)
{
	uint8_t* pixelptr = pixels.data(); // first pixels obj pointer
	// then calculate how many pixels (from DataVector pixels) are available to draw in surface
	ull left_to_draw = static_cast<ull>((pixels.size() - pixels.size() % 3) / 3);
	while (y < image.h && x < image.w && left_to_draw > 0)
	{
		SDL_utils::draw_pixel(const_cast<SDL_Surface*>(&image), x, y, pixelptr[2], pixelptr[1], pixelptr[0]);
		pixelptr += 3;
		++x;
		--left_to_draw;
		if (x == image.w) // go to next line of image
		{
			x = 0;
			++y;
		}
	}
	pixels.erase(pixels.begin(), pixels.end() - pixels.size() % 3); // remove drew pixels
}

/**
* \brief Draw pixels into SDL_Surface image and cleans them up from 'pixels'
* \param pixel pixel that will be drawn
* \param pixels vector with uint8_ts that represent pixels' RGB channels (in BRG order)
* \param x width value for image
* \param y heigth value for image
*/
void Coder::draw_pixels(const SDL_Surface& image, const Pixel& pixel, const int& reps, int& x, int& y)
{
	int left_to_draw = reps;
	while (y >= 0 && x < image.w && left_to_draw > 0)
	{
		SDL_utils::draw_pixel(const_cast<SDL_Surface*>(&image), x, y, pixel[2], pixel[1], pixel[0]);
		++x;
		--left_to_draw;
		if (x == image.w) // go to next line of image
		{
			x = 0;
			--y;
		}
	}
}

/**
 * \brief alters 8-bit pixel values into 7-bit values
 * \param pixel input pixel
 */
void Coder::to_7_bit(Pixel& pixel)
{
	for (auto& value : pixel)
		value &= 0;
}

/**
* \brief Alter pixel into grayscale
* \param pixel array of 3 uint8_ts representing RGB pixel (in BGR order)
*/
void Coder::to_gray(Pixel& pixel) // pixel in BGR order
{
	// using luma formula to calculate "relative luminescence"
	uint8_t luma = static_cast<uint8_t>(pixel[2] * 0.2126 + pixel[1] * 0.7152 + pixel[0] * 0.0722);
	pixel[0] = pixel[1] = pixel[2] = luma;
}
