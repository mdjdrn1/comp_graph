#include "BMP.h"
#include <fstream>
#include <string>
#include "SDL.h"

BMP::BMP(std::string name) : filename(name)
{
	fopen_s(&bmp_file, filename.c_str(), "rb");

	if (bmp_file == nullptr)
	{
		std::cerr << "Error. Opening file failed.\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}

	read_header();
}

BMP::~BMP()
{
	if (fclose(bmp_file) == EOF)
	{
		std::cerr << "Error. Closing file failed.\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}
}

/*
 * Read RGB values from every pixel in image and pass them to 'os' stream.
 */
void BMP::read_plxs(std::ostream& os) const
{
	// start reading from 54 byte, as first 54 contain header info
	fseek(bmp_file, 54, std::ios::beg);
	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight; //calculate data size with padding
	unsigned char* data = new unsigned char[data_size]; // data buffer
	fread(data, data_size, 1, bmp_file);
	for (uint i = 0; i < data_size; i += 3)
	{
		int bval = static_cast<int>(data[i]);
		int gval = static_cast<int>(data[i + 1]);
		int rval = static_cast<int>(data[i + 2]);

		os << "R: " << rval << " G: " << gval << " B: " << bval << std::endl;
	}
}

/**
 * \brief Converting input 24-bit BMP file to reduced 24-bit file
 */
void BMP::convert_bmp_to_7()
{
	// TODO: Change creating file
	// (just temporary) creating file if it didn't exist
	std::fstream outcreate("output.bard", std::ios::trunc);
	outcreate.close();

	FILE* out;

	if (fopen_s(&out, "output.bard", "wb+") == -1)
	{
		std::cerr << "opening failed.\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}

	// start reading from 54 byte, as first 54 contain header info
	fseek(bmp_file, 54, std::ios::beg);
	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight; //calculate data size with padding
	unsigned char* data = new unsigned char[data_size]; // data buffer
	fread(data, data_size, 1, bmp_file);

	uint i;
	uint8_t RGB[8];
	for (i = 0; i < data_size; ++i)
	{
		if (i != 0 && i % 8 == 0)
			pack(RGB, out);
		RGB[i % 8] = static_cast<uint8_t>(data[i]);
		// TEST 
		// std::cout << std::dec << i << ": " << std::hex << static_cast<int>(RGB[i % 8]) << std::endl;
	}

	// latest bits, that didn't fill RGB array fully
	if ((i - 1) % 8 != 0)
	{
		i = i % 8;
		while (i < 8)
		{
			// fill with zeros remaining values
			RGB[i] = 0;
			++i;
		}
		// pack same as before (in a loop)
		// TODO: Omit surplus bytes
		pack(RGB, out);
	}
	fclose(out);
}

/**
 * \brief Reducing every byte from 8-bits to 7-bits
 * \param vals array of 8 uint8_t(8-bytes) elements, where every element will be reduced to 7-bytes
 * \param fp output file pointer
 */
void BMP::pack(uint8_t vals[8], FILE* fp)
{
	// TESTS 
	/*std::cout << "values in vals:\n";
	for (int y = 0;y < 8;++y)
	{
		std::cout << std::dec << y << ": " << std::hex << static_cast<int>(vals[y]) << std::endl;
	}
	std::cout << std::endl;*/
	uint8_t pack = 0x00;
	uint8_t x, p;
	bool bit = 1;

	for (int t = 0, n = 0; t < 7; ++t , ++n)
	{
		pack = vals[t];
		x = 7;
		p = n;
		for (int k = 0; k <= n; ++k , --p , --x)
		{
			//check bit 
			bit = (vals[t + 1] >> x) & 1;

			if (bit) //set bit x to 1
				pack |= 1 << p;
			else //set bit x to 0
				pack &= ~(1 << p);
		}
		vals[t + 1] <<= t + 1;

		fwrite(&pack, sizeof(uint8_t), 1, fp);
	}
}


/**
 * \brief reading first 54 file bytes containing header
 */
void BMP::read_header()
{
	fseek(bmp_file, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, bmp_file);
}

std::ostream& operator<<(std::ostream& os, const BMP& obj)
{
	os << "Width: " << obj.header.biWidth << std::endl;
	os << "Height: " << obj.header.biHeight << std::endl;
	os << "Size: " << static_cast<double>(obj.header.bfSize) / 1024 << "KB" << std::endl;
	os << "Signature: " << std::hex << obj.header.bfSignature << std::endl;
	os << "Bit: " << std::dec << obj.header.biBitCount << std::endl;
	os << "Compression: " << std::dec << obj.header.biCompression << std::endl;
	os << std::dec << std::endl;
	return os;
}
