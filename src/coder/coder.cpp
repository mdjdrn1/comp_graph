#include "coder.hpp"

#include <array>
#include "../error/error.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <SDL.h>
#elif defined(__unix__)
#include <SDL2/SDL.h>
#endif

/**
 * \brief custom deleter for SDL_Surface unique pointer
 * \param surface SDL_Surface
 */
void Coder::Surface_deleter::operator()(SDL_Surface* surface) const
{
	SDL_FreeSurface(surface);
}

/**
* \brief Get pixel RGB values from SDL_Surface
* \param surface image
* \param x pos (indexing from 0)
* \param y pos (indexing from 0)
* \return 3-item array with RGB values
*/
auto Coder::get_pixel(SDL_Surface* surface, const int& x, const int& y) const -> Pixel
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* pixelPtr = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp; // addres to pixel to get
	Uint32 pixel = *(reinterpret_cast<Uint32 *>(pixelPtr));
	Pixel pixel_array; // pixel_array will contain RGB values
	SDL_GetRGB(pixel, surface->format, &pixel_array[0], &pixel_array[1], &pixel_array[2]);

	return std::move(pixel_array);
}

/**
* \brief Draw pixel into SDL_Surface
* \param surface image
* \param x pos (indexing from 0)
* \param y pos (indexing from 0)
* \param R R value
* \param G G value
* \param B B value
*/
void Coder::draw_pixel(SDL_Surface* surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B) const
{
	Uint32 pixel = SDL_MapRGB(surface->format, R, G, B);

	int bpp = surface->format->BytesPerPixel;
	Uint8* p = static_cast<Uint8 *>(surface->pixels) + y * surface->pitch + x * bpp; // ptr to pixel we want to set

	p[0] = pixel & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = (pixel >> 16) & 0xff;
}

Coder::Coder()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
		throw Error ("In Coder::Coder: Unable to initialize SDL:");
}

Coder::~Coder()
{
	SDL_Quit();
}

/**
* \brief Create new SDL_Surface from existing BMP file
* \param filename BMP file name
* \return new surface
*/
SDL_Surface* Coder::new_bmp_surface(const std::string& filename) const
{
	SDL_Surface* surface = SDL_LoadBMP(filename.c_str());
	if (!surface)
		throw Error("In Coder::new_bmp_surface(): Unable to load bitmap from file: " + filename + ".");

	return surface;
}

/**
* \brief Create new empty SDL_Surface
* \param width width of new surface
* \param height height of new surface
* \return new surface
*/
SDL_Surface* Coder::new_empty_surface(const int& width, const int& height) const
{
	int bit_depth = 24;

	Uint32 Rmask = 0x00ff0000; // red mask for the pixels
	Uint32 Gmask = 0x0000ff00; // green mask for the pixels
	Uint32 Bmask = 0x000000ff; // blue mask for the pixels
	Uint32 Amask = 0xff000000; // alpha mask for the pixels

	SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, bit_depth, Rmask, Gmask, Bmask, Amask);

	if (!surface)
		throw Error("In Coder::new_empty_surface(): failed to create new surface.");

	return surface;
}


/*
* \brief Header ctor(for encoding)
* \param image image to create header from
* \param compression_mode mode BITPACK, RLE or HUFFMAN
* \param grayscale_choice int 1 for grayscale, otherwise 0
*/
Coder::Header::Header(const int& h, const int& w, const mode& compression_mode, const bool& grayscale_choice)
	: signature(0),
	  offset(sizeof(Header)),
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
	: signature(0),
	  offset(sizeof(Header)),
	  width(image->w),
	  height(image->h),
	  grayscale(grayscale_choice),
	  compression(static_cast<ushort>(compression_mode))
{
	// Create signature
	uint8_t* sig_ptr = reinterpret_cast<uint8_t*>(&signature);
	sig_ptr[0] = 'X';
	sig_ptr[1] = 'D';
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
void Coder::draw_pixels(const SDL_Surface& image, DataVector& pixels, int& x, int& y, const bool& grayscale) const
{
	uint8_t* pixelptr = pixels.data(); // first pixels obj pointer
	// then calculate how many pixels (from DataVector pixels) are available to draw in surface
	ull left_to_draw = static_cast<ull>((pixels.size() - pixels.size() % 3));// / 3);
	if (!grayscale)
		left_to_draw /= 3;
	while (y < image.h && x < image.w && left_to_draw > 0)
	{
		if (!grayscale)
		{
			draw_pixel(const_cast<SDL_Surface*>(&image), x, y, pixelptr[2], pixelptr[1], pixelptr[0]);
			pixelptr += 3;
		}
		else
		{
			draw_pixel(const_cast<SDL_Surface*>(&image), x, y, pixelptr[0], pixelptr[0], pixelptr[0]);
			++pixelptr;
		}
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

// TODO: temporary, just for current RLE without reading pixels from SDL_Surface
/**
* \brief Draw pixels into SDL_Surface image and cleans them up from 'pixels'
* \param pixel pixel that will be drawn
* \param pixel vector with uint8_ts that represent pixels' RGB channels (in BRG order)
* \param x width value for image
* \param y heigth value for image
*/
void Coder::draw_pixels(const SDL_Surface& image, const Pixel& pixel, const int& reps, int& x, int& y) const
{
	int left_to_draw = reps;
	while (y < image.h && x < image.w && left_to_draw > 0)
	{
		draw_pixel(const_cast<SDL_Surface*>(&image), x, y, pixel[2], pixel[1], pixel[0]);
		++x;
		--left_to_draw;
		if (x == image.w) // go to next line of image
		{
			x = 0;
			++y;
		}
	}
}

/**
 * \brief alters 8-bit pixel values into 7-bit values
 * \param pixel input pixel
 */
void Coder::to_7_bit(Pixel& pixel) const
{
	for (auto& value : pixel)
		value &= 254;
}

/**
* \brief Alter pixel into grayscale
* \param pixel array of 3 uint8_ts representing RGB pixel (in BGR order)
*/
void Coder::to_gray(Pixel& pixel) const // pixel in BGR order
{
	// using luma formula to calculate "relative luminescence"
	uint8_t luma = static_cast<uint8_t>(pixel[2] * 0.2126 + pixel[1] * 0.7152 + pixel[0] * 0.0722);
	pixel[0] = pixel[1] = pixel[2] = luma;
}

/**
 * \brief Validate if file was BMP
 * \param filename input encode file name
 */
void Coder::validate_bmp(const std::string& filename) const
{
	std::fstream bmp_file(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!bmp_file.is_open())
		throw Error("In Converter::validate_bmp(): couldn't open input convert file.");

	uint16_t buff;
	bmp_file.read(reinterpret_cast<char*>(&buff), sizeof(buff));
	uint8_t* buff_ptr = reinterpret_cast<uint8_t*>(&buff);
	if (!(buff_ptr[0] == 'B' && buff_ptr[1] == 'M'))
		throw Error("In Converter::validate_bmp(): invalid signature in BMP file.");
	bmp_file.close();
}

/**
 * \brief Validate if file was bard
 * \param filename input decode file name
 */
void Coder::validate_bard(const std::string& filename) const
{
	std::fstream bard_file(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!bard_file.is_open())
		throw Error("In Converter::validate_bard(): couldn't open input convert file.");

	Header bard_header(bard_file); // reading Header
	bard_file.close();

	uint8_t* sig_ptr = reinterpret_cast<uint8_t*>(&bard_header.signature);
	if (!(sig_ptr[0] == 'X' && sig_ptr[1] == 'D'))
		throw Error("In Converter::validate_bard(): invalid signature in BARD file.");
}

