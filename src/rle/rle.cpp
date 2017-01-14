#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include "RLE.hpp"


void RLE::encode(const std::string& filename, const bool& grayscale)
{
	Pixel next;
	Pixel current;
	unsigned int repetition = 1;

	std::fstream file;
	std::fstream compressed;

	file.open(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "cannot open file to encode." << std::endl;
		getchar();
		exit(1);
	}

	compressed.open(decoded_filename(filename), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!compressed.is_open())
	{
		std::cout << "cannot open file to save encoded file." << std::endl;
		getchar();
		exit(1);
	}

	file.read(reinterpret_cast<char*>(&current), sizeof(Pixel)); // read first two pixeles
	file.read(reinterpret_cast<char*>(&next), sizeof(Pixel));

	while (!file.eof())
	{
		if (!compare(current, next)) // if different, write repetitions and pixel to file
		{
			compressed.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));
			compressed.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
			repetition = 0;
		}
		++repetition;
		current = next;
		file.read(reinterpret_cast<char*>(&next), sizeof(Pixel)); // read next pixel

		if (file.eof()) // if end of file, write repetitions and pixel to file
		{
			compressed.write(reinterpret_cast<char*>(&repetition), sizeof(repetition));
			compressed.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
		}
	}

	file.close();
	compressed.close();
}

void RLE::decode(const std::string& filename)
{
	unsigned int repetition = 1;
	Pixel current;
	std::fstream file;
	std::fstream ready;

	file.open(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "cannot open file to decode." << std::endl;
		getchar();
		exit(1);
	}
	ready.open(decoded_filename(filename).c_str(), std::ios::trunc | std::ios::out | std::ios::binary);
	if (!ready.is_open())
	{
		std::cout << "cannot open file to save decoded file." << std::endl;
		getchar();
		exit(1);
	}

	while (!file.eof())
	{
		file.read(reinterpret_cast<char*>(&repetition), sizeof(repetition)); // read repetitions
		file.read(reinterpret_cast<char*>(&current), sizeof(Pixel)); // read pixel
		if (file.eof()) break;
		for (unsigned int j = 0; j < repetition; j++) // write pixel 'repetition' times
			ready.write(reinterpret_cast<char*>(&current), sizeof(Pixel));
	}

	file.close();
	ready.close();
}


