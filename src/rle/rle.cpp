#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include "RLE.hpp"
#include "../error/error.hpp"


bool RLE::compare(const Pixel& pixel1, const Pixel& pixel2) const
{
	if (pixel1[0] == pixel2[0] && pixel1[1] == pixel2[1] && pixel1[2] == pixel2[2])
		return true;
	return false;
}

void RLE::encode(const std::string& filename, const bool& grayscale)
{
	std::fstream infile(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!infile.is_open())
		throw Error("In RLE::encode(): couldn't open input convert file.");

	// assuming filename is in correct format e.g. "path\\foo bar xyz.bmp"
	std::fstream outfile(encoded_filename(filename).c_str(),
		std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (!outfile.is_open())
		throw Error("In RLE::encode(): couldn't open output convert file.");

	// load surface to create bard_header
	SDL_Surface_ptr image(new_bmp_surface(filename));

	// create new header for coded file
	Header bard_header(image.get(), RLE_EN, grayscale);
	
	outfile.write(reinterpret_cast<char*>(&bard_header), sizeof(bard_header));
	outfile.seekg(bard_header.offset, std::ios_base::beg);

	infile.seekg(54, std::ios_base::beg); // set file to read after header

	Pixel next {};
	Pixel current {};
	unsigned int repetition = 1;

	infile.read(reinterpret_cast<char*>(&current), sizeof(Pixel)); // read first two pixeles
	infile.read(reinterpret_cast<char*>(&next), sizeof(Pixel));

	to_7_bit(current);
	if (grayscale)
		to_gray(current);

	// TODO: Read pixels from SDL_Surface
	while (!infile.eof())
	{
		if (!compare(current, next)) // if different, write repetitions and pixel to file
		{
			outfile.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));
			outfile.write(reinterpret_cast<char*>(&current), sizeof(Pixel));

			repetition = 0;
		}
		++repetition;
		current = next;
		infile.read(reinterpret_cast<char*>(&next), sizeof(Pixel)); // read next pixel
		to_7_bit(next);
		if (grayscale)
			to_gray(next);

		if (infile.eof()) // if end of file, write repetitions and pixel to file
		{
			outfile.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));
			outfile.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
		}
	}

	infile.close();
	outfile.close();
}

void RLE::decode(const std::string& filename, const bool& grayscale)
{
	// reading values from file
	std::fstream infile(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!infile.is_open())
		throw Error("In RLE::decode(): couldn't open input convert file.");

	Header bard_header(infile); // reading Header
	infile.seekg(bard_header.offset, std::ios_base::beg); // set file to read after header

	SDL_Surface_ptr decoded_image(new_empty_surface(bard_header.width, bard_header.height)); // creates surface for drawuing pixels (size header.width x header.height)
	int x = 0, y = decoded_image->h - 1; // init current pixel (in surface) position

	unsigned int repetition = 1;
	Pixel current_pixel;

	while (!infile.eof())
	{
		infile.read(reinterpret_cast<char*>(&repetition), sizeof(repetition)); // read repetitions
		infile.read(reinterpret_cast<char*>(&current_pixel), sizeof(Pixel)); // read pixel
		if (infile.eof()) 
			break;
		draw_pixels(*decoded_image, current_pixel, repetition, x, y);
	}

	infile.close();

	SDL_SaveBMP(decoded_image.get(), decoded_filename(filename).c_str()); // finally, save file to BMP extension
}
