#include "BMP.h"
#include <fstream>
#include <string>
#include <cstdio>
#include <exception>


/**
* @brief: BMP file class
* @param filename: filename
*/
BMP::BMP(std::string filename) 
	: m_filename(filename), 
	m_fs(nullptr)
{
	m_fs = new File(m_filename, std::ios_base::in | std::ios_base::binary);
	read_header();
}

BMP::~BMP()
{
	delete m_fs;
}

/**
* @brief: reading first 54 file bytes containing header into header struct
*/
void BMP::read_header()
{
	m_fs->seek(0);
	m_fs->read(reinterpret_cast<char*>(&header), sizeof(header));
}

/*in
 * @brief: Read RGB values from every pixel in image
 * @param os: stream for output
 */
void BMP::read_file(std::ostream& os)
{
	// start reading from 54 byte, as first 54 contain header info
	m_fs->seek(54);
	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight; //calculate data size with padding
	unsigned char* data = new unsigned char[data_size]; // data buffer
	m_fs->read(reinterpret_cast<char*>(data), data_size);
	for (uint i = 0; i < data_size; i += 3)

	{
		int bval = static_cast<int>(data[i]);
		int gval = static_cast<int>(data[i + 1]);
		int rval = static_cast<int>(data[i + 2]);

		os << "R: " << rval << " G: " << gval << " B: " << bval << std::endl;
	}
}

/**
 * @brief: Converting input 24-bit BMP file to reduced 21-bit file
 */
void BMP::convert_bmp_to_7()
{
	// init input file
	// start reading from 54 byte, as first 54 contain header info
	m_fs->seek(54);
	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight; //calculate data size with padding
	unsigned char* data = new unsigned char[data_size]; // data buffer
	m_fs->read(reinterpret_cast<char*>(data), data_size);

	// assuming filename is in correct format "path\\foo bar xyz.bmp"
	File out_fs((m_filename.substr(0, m_filename.size() - 3) + "bard").c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	uint i;
	uint8_t pre_pack[8]{};
	for (i = 0; i < data_size; ++i)
	{
		if (i != 0 && i % 8 == 0)
			packer(pre_pack, out_fs);
		pre_pack[i % 8] = static_cast<uint8_t>(data[i]);
		// TEST 
		// std::cout << std::dec << i << ": " << std::hex << static_cast<int>(pre_pack[i % 8]) << std::endl;
	}

	// latest bits, that didn't fill pre_pack array fully
	if ((i - 1) % 8 != 0)
	{
		i = i % 8;
		while (i < 8)
		{
			// fill with zeros remaining values
			pre_pack[i] = 0;
			++i;
		}
		// pack same as before (in a loop)
		// TODO: Omit surplus bytes
		packer(pre_pack, out_fs);
	}
}

/**
 * @brief: Reducing every byte from 8-bits to 7-bits
 * @param vals: array of 8 uint8_t elements, where every element will be reduced to 7-bytes
 * @param file: File object for output
 */
void BMP::packer(uint8_t vals[8], File& file)
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

		file.write(reinterpret_cast<char*>(&pack), sizeof(uint8_t));
	}
}

std::ostream& operator<<(std::ostream& os, const BMP& obj)
{
	os << "Width: " << obj.header.biWidth << std::endl;
	os << "Height: " << obj.header.biHeight << std::endl;
	os << "Size: " << static_cast<double>(obj.header.bfSize) / 1024 << "KB" << std::endl;
	os << "Signature: " << std::hex << obj.header.bfSignature << std::endl;
	os << "Bit: " << std::dec << obj.header.biBitCount << std::endl;
	os << "Offset: " << std::dec << obj.header.bfOffset << std::endl;
	os << "Clrs used: " << std::dec << obj.header.biClrUsed << std::endl;
	os << "Compression: " << std::dec << obj.header.biCompression << std::endl;
	os << std::dec << std::endl;
	return os;
}