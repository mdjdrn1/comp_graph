#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <bitset>
#include "Huffman.hpp"

Huffman::Huffman()
{
}

void Huffman::encode(const std::string& filename, const bool& grayscale)
{
	// OPEN BMP FILE
	std::ifstream in_file(filename, std::ios::binary);
	if (in_file.fail())
	{
		std::cout << "Cannot open file! \n ";
		exit(0);
	}
	in_file.seekg(54);

	// COUNT FREQUENCY COLOR 
	int frequencyColor[256];
	memset(frequencyColor, 0, sizeof(int) * 256);

	uint8_t color;
	while (!in_file.eof())
	{
		// lead colors
		in_file.read(reinterpret_cast<char*>(&color), sizeof(uint8_t));
		// counts frequency colors
		++frequencyColor[color];
	}
	//	std::cout << std::endl;
	in_file.close();

	// CREATE HUFFMAN CODE
	makeTree(frequencyColor);

	// save header to file
	std::ofstream out_file(encoded_filename(filename), std::ios::binary);
	if (out_file.fail())
	{
		std::cout << "cannot open file! \n ";
		exit(0);
	}
	std::sort(vecHuffman.begin(), vecHuffman.end());

	for (unsigned short i = 0, j = 0, k = 0; i < 256; ++i)
	{
		if (i == vecHuffman[j].first)
		{
			// binary huffman code to decimal value
			std::string::size_type buffer = 0;
			unsigned short code = std::stoul(vecHuffman[j].second, &buffer, 2);
			uint8_t number_of_zeros = 0;
			for (size_t m = 0; m < vecHuffman[m].second.size(); ++m)
				if (vecHuffman[m].second[m] == '0')
					++number_of_zeros;
				else
					break;

			out_file.write(reinterpret_cast<char*>(&number_of_zeros), sizeof(number_of_zeros)); // send: how many zeros is in befor decimal code	(1 byte)
			out_file.write(reinterpret_cast<char*>(&code), sizeof(code)); // send: huffman code(decimal)						(2 bytes)
			++j;
		}
		else
		{
			out_file.write(reinterpret_cast<char*>(&k), sizeof(uint8_t));
			out_file.write(reinterpret_cast<char*>(&k), sizeof(k));
		}
	}
	out_file.close();

	// Read bytes from bmp file and write huffman code to compress file
	saveEncode(filename, encoded_filename(filename));
}

void Huffman::decode(const std::string& filename, const bool& grayscale)
{
	std::ifstream in_file(filename, std::ios::binary);
	std::map<std::string, uint8_t> mapCode;

	// read huffman code form file
	for (int i = 0; i < 256; ++i)
	{
		unsigned short code_short;
		uint8_t number_of_zeros;

		in_file.read(reinterpret_cast<char*>(&number_of_zeros), sizeof(number_of_zeros));
		in_file.read(reinterpret_cast<char*>(&code_short), sizeof(code_short));

		std::string code = decToBin(code_short);
		for (int k = 0; k < number_of_zeros; ++k)
			code = '0' + code;

		if (!code.empty())
			mapCode.insert({code, i});
	}

	std::string temp_color;
	while (!in_file.eof())
	{
		uint8_t _color;
		std::string color;
		in_file.read(reinterpret_cast<char*>(&_color), sizeof(_color));
		color = decToBin(_color);
		while (color.size() != 8)
			color = '0' + color;
		for (int i = 0; i < color.size(); ++i)
		{
			temp_color += color[i];
			if (mapCode.count(temp_color) > 0)
			{
				temp_color.clear();
			}
		}
	}
}

void Huffman::makeTree(int* frequencyColor)
{
	for (int i = 0; i < 256; ++i)
		if (frequencyColor[i])
		{
			// save data like color and frequency 
			auto root = std::make_shared<Huffman::node>(i, frequencyColor[i]);

			// create node 
			huffmanQueue.push(std::move(root));
		}

	// create huffman tree
	while (huffmanQueue.size() > 1)
	{
		auto leftNode = huffmanQueue.top();
		huffmanQueue.pop();

		auto rightNode = huffmanQueue.top();
		huffmanQueue.pop();

		auto root = std::make_shared<Huffman::node>(256, leftNode->frequency + rightNode->frequency);
		root->left = leftNode;
		root->right = rightNode;
		huffmanQueue.push(std::move(root));
	}

	returnCode(huffmanQueue.top());
}

void Huffman::returnCode(const shared_ptr_huff& root)
{
	// create code huffman 
	if ((root->left != nullptr) && (root->right != nullptr))
	{
		codeRepresentation.push_back(0);
		returnCode(root->left);
		codeRepresentation.pop_back();
		codeRepresentation.push_back(1);
		returnCode(root->right);
		codeRepresentation.pop_back();
	}
	else
	{
		for (const auto& i : codeRepresentation)
			root->code += std::to_string(i);
		vecHuffman.push_back(std::make_pair(root->color, root->code));
	}
}

void Huffman::saveEncode(std::string in_file_name, std::string out_file_name)
{
	std::ifstream in_file(in_file_name, std::ios::binary);
	if (in_file.fail())
	{
		std::cout << "Cannot open file! \n ";
		exit(0);
	}
	in_file.seekg(54);

	std::ofstream out_file(out_file_name, std::ios::binary | std::ios::app);
	if (out_file.fail())
	{
		std::cout << "cannot open file! \n ";
		exit(0);
	}

	// Main functions loop reading data from file byte by byte and save huffman code bit by bit
	bool bit;
	unsigned char byte = 0;
	short position = 7;

	while (!in_file.eof())
	{
		uint8_t color;
		in_file.read(reinterpret_cast<char*>(&color), sizeof(color));
		int counter = 0;
		while (color != vecHuffman[counter].first) // trzeba to zamienic na map bo w uj slabo to dziala O(n^2)
			++counter; // ew jakies pomysly na to prosze pisac :)
		for (int i = 0; i < vecHuffman[counter].second.size(); ++i)
		{
			if (vecHuffman[counter].second[i] == '1')
				bit = 1;
			else
				bit = 0;

			writeCode(bit, position, byte); // set bit
			if (--position == -1) // send to file pack bits (byte)
			{
				out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
				position = 7;
				byte = 0;
			}
		}
	}
	out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
}

void Huffman::writeCode(bool& bit, short& bitCount, unsigned char& temp)
{
	temp |= (bit << bitCount);
}

Huffman::node::node(uint8_t color, int frequency)
	: color(color), left(nullptr), right(nullptr), frequency(frequency)
{
	// ctor
}

std::string Huffman::decToBin(unsigned short n)
{
	std::string bin;
	while (n)
	{
		if (n % 2)
			bin = '1' + bin;
		else
			bin = '0' + bin;
		n /= 2;
	}
	return std::move(bin);
}
