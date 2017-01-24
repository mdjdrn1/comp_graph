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

RLE::RLE() : Coder()
{
}

void RLE::encode(const std::string& filename, const bool& grayscale)
{
	// assuming filename is in correct format e.g. "path\\foo bar xyz.bmp"
	std::fstream compressed(encoded_filename(filename).c_str(),
	                        std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (!compressed.is_open())
		throw Error("In RLE::encode(): couldn't open output convert file.");

	// load surface to create bard_header
	SDL_Surface_ptr image(new_bmp_surface(filename));
	// create new header for coded file
	Header bard_header(image.get(), RLE_EN, grayscale);

	compressed.write(reinterpret_cast<char*>(&bard_header), sizeof(bard_header));
	compressed.seekg(bard_header.offset, std::ios_base::beg);


	std::vector<Pixel> vcopy;
	uint8_t vstart = 0;
	uint8_t vsize = vcopy.size();

	Pixel next, current;
	uint8_t repetition = 1;

	int x = 0, y = 0;
	current = get_pixel(image.get(), x++, y);
	if (grayscale)
		to_gray(current);
	if(x == image->w)
	{
		x = 0;
		++y;
	}
	vcopy.push_back(current);

	for (y; y < image->h; ++y)
	{
		for (x=0; x < image->w; ++x)
		{
			next = get_pixel(image.get(), x, y);
			if (grayscale)
				to_gray(next);
			if (compare(current, next))
			{ // identical pixels
				if (!vcopy.empty())
				{// save vector to file
					vcopy.pop_back(); // usun ostatni element - zostanie uwzglêdniony w tym powtorzeniu
					compressed.write(reinterpret_cast<char*>(&vstart), sizeof(vstart)); // 0 dla rozroznienia sekwencji od powtorzen
					vsize = vcopy.size();
					compressed.write(reinterpret_cast<char*>(&vsize), sizeof(vsize)); // ilosc elementow w sekwencji
					for (int i = 0; i < vsize; i++) // wypisanie wektora
					{
						if(!grayscale)
							compressed.write(reinterpret_cast<char*>(&vcopy[i]), sizeof(Pixel));
						else
							compressed.write(reinterpret_cast<char*>(&vcopy[i][0]), sizeof(uint8_t));
					}
					vcopy.clear(); // uwuniecie zawartosci
				}
				if (repetition < 255)
					++repetition;
				else
				{ // gdy liczba powtorzen przekracza 255
					compressed.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));

					if (!grayscale)
						compressed.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
					else
						compressed.write(reinterpret_cast<char*>(&current[0]), sizeof(uint8_t));
					repetition = 2;
				}
			}
			else
			{ // gdy pixele sa rozne
				if (repetition > 1)
				{ // jesli pixele sie powtarzaly, wypisz je
					compressed.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));
					if (!grayscale)
						compressed.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
					else
						compressed.write(reinterpret_cast<char*>(&current[0]), sizeof(uint8_t));
				}
				repetition = 1;
				vsize = vcopy.size();
				if (vsize < 255)
				{
					vcopy.push_back(next);
				}
				else
				{ // jesli przekroczono limit nastapi zapis do pliku
					compressed.write(reinterpret_cast<char*>(&vstart), sizeof(vstart));
					vsize = vcopy.size();
					compressed.write(reinterpret_cast<char*>(&vsize), sizeof(vsize));
					for (int i = 0; i < vsize; i++)
					{
						if (!grayscale)
							compressed.write(reinterpret_cast<char*>(&vcopy[i]), sizeof(Pixel));
						else
							compressed.write(reinterpret_cast<char*>(&vcopy[i][0]), sizeof(uint8_t));
					}
					vcopy.clear();
					vcopy.push_back(next); // dodaj nowy element
				}
			}

			current = next;
		}
	}


	if (!vcopy.empty())
	{ // if end of file, write repetitions and Pixel to file
		compressed.write(reinterpret_cast<char*>(&vstart), sizeof(vstart)); // 0 dla rozroznienia sekwencji od powtorzen
		vsize = vcopy.size();
		compressed.write(reinterpret_cast<char*>(&vsize), sizeof(vsize)); // ilosc elementow w sekwencji
		for (int i = 0; i < vsize; i++) // wypisanie wektora
		{
			if(!grayscale)
				compressed.write(reinterpret_cast<char*>(&vcopy[i]), sizeof(Pixel));
			else
				compressed.write(reinterpret_cast<char*>(&vcopy[i][0]), sizeof(uint8_t));
		}
		vcopy.clear();
	}
	else
	{
		compressed.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));
		if (!grayscale)
			compressed.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
		else
			compressed.write(reinterpret_cast<char*>(&current[0]), sizeof(uint8_t));
	}

	compressed.close();
}

void RLE::decode(const std::string& filename, const bool& grayscale)
{
	// reading values from file
	std::fstream file(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!file.is_open())
		throw Error("In RLE::decode(): couldn't open input convert file.");

	Header bard_header(file); // reading Header
	file.seekg(bard_header.offset, std::ios_base::beg); // set file to read after header

	SDL_Surface_ptr decoded_image(new_empty_surface(bard_header.width, bard_header.height)); // creates surface for drawuing pixels (size header.width x header.height)
	int x = 0, y = 0; // init current pixel (in surface) position

	uint8_t repetition;// = 1;
	Pixel current;

	while (!file.eof()) {
		file.read(reinterpret_cast<char*>(&repetition), sizeof(repetition));	// read repetitions
		
		if (file.eof()) break;
		uint8_t count = 0;

		if (repetition == 0)
		{
			file.read(reinterpret_cast<char*>(&count), sizeof(count));
			for (unsigned int i = 0; i < count; i++)
			{
				if(!grayscale)
					file.read(reinterpret_cast<char*>(&current), sizeof(Pixel));
				else
				{
					file.read(reinterpret_cast<char*>(&current[0]), sizeof(uint8_t));
				}
				current[1] = current[0];
				current[2] = current[0];
				draw_pixels(*decoded_image, current, 1, x, y);
			}
		}
		else {
			if (!grayscale)
				file.read(reinterpret_cast<char*>(&current), sizeof(Pixel));
			else
			{
				file.read(reinterpret_cast<char*>(&current[0]), sizeof(uint8_t));
			}
			current[1] = current[0];
			current[2] = current[0];

			draw_pixels(*decoded_image, current, repetition, x, y);
		}
	}

	file.close();


	if (SDL_SaveBMP(decoded_image.get(), decoded_filename(filename).c_str()) < 0) // finally, save file to BMP extension
		throw Error("In RLE::decode(): failed saving decoded file.");
}
