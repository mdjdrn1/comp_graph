#include "huffman.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>

//////////////////////////////////////////////////////////////////////////////////////////
huffman::huffman(std::string in_file_name, std::string out_file_name)
{
	// check input file extesion
	if (in_file_name.substr(in_file_name.find_last_of(".") + 1) == "bmp")
	{
		encode(in_file_name, out_file_name);
	}
	else if (in_file_name.substr(in_file_name.find_last_of(".") + 1) == "bard")
	{
		std::cout << "drab" << std::endl;
	}
	else
	{
		std::cout << "Wrong file extension! Try again!" << std::endl;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
huffman::encode::node::node(uint8_t color, int frequency)
	: color(color), left(nullptr), right(nullptr), frequency(frequency)
{
	// ctor
}
//////////////////////////////////////////////////////////////////////////////////////////
huffman::encode::encode(std::string in_file_name, std::string out_file_name)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////      KOMPRESJA      /////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	// OPEN BMP FILE
	std::ifstream in_file(in_file_name, std::ios::binary);
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
		// Lead colors
		in_file.read((char*)&color, sizeof(uint8_t));
		//std::cout << (unsigned)color << ", ";
		// Counts frequency colors
		++frequencyColor[color];
	}

	// CREATE HUFFMAN CODE
	makeTree(frequencyColor);

	/////////////////////////////////////////////////////////////////////////
	// Save header to file
	std::ofstream out_file(out_file_name, std::ios::binary);
	if (out_file.fail())
	{
		std::cout << "cannot open file! \n ";
		exit(0);
	}
	for (unsigned short i = 0, code; i < 256; ++i)
	{
		if (!huffmanCode[i].empty())
		{
			// binary huffman code to decimal value
			std::string::size_type buffer = 0;
			code = ~std::stoul(huffmanCode[i], &buffer, 2);
			out_file.write((char*)&code, sizeof(code));
		}
		else
			out_file.write((char*)&(code = 0), sizeof(code));
	}
	///////////////////////////////////////////////////////////////////////
	// Read bytes from bmp file and write huffman code to compress file
	in_file.clear();
	in_file.seekg(54);
	// Main functions loop reading data frome file byte by byte and save huffman code bit by bit
	bool bit;
	unsigned char byte = 0;
	short position = 7;

	while (!in_file.eof())
	{
		uint8_t color;
		in_file.read((char*)&color, sizeof(color));
		for (int i = 0; i < huffmanCode[color].size(); ++i)
		{
			if (huffmanCode[color][i] == '1')
				bit = 1;
			else
				bit = 0;
			byte |= (bit << position);
			if (--position == -1)
			{
				std::cout << byte << std::endl;
				out_file.write((char*)&byte, sizeof(byte));
				position = 7; 
				byte = 0;
			}
		}
	}
	std::cout << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void huffman::encode::makeTree(int *frequencyColor)
{ 
	for (int i = 0; i < 256; ++i)
		if (frequencyColor[i])
		{
			// save data like color and frequency 
			auto root = std::make_shared<encode::node>(i, frequencyColor[i]);

			// create node 
			huffmanQueue.push(root);
		}

	// create huffman tree
	while (huffmanQueue.size() > 1)
	{
		auto leftNode = huffmanQueue.top();
		huffmanQueue.pop();

		auto rightNode = huffmanQueue.top();
		huffmanQueue.pop();

		auto root = std::make_shared<encode::node>(256, leftNode->frequency + rightNode->frequency);
		root->left = leftNode;
		root->right = rightNode;
		huffmanQueue.push(root);
	}

	returnCode(huffmanQueue.top());
}
//////////////////////////////////////////////////////////////////////////////////////////
void huffman::encode::returnCode(const shared_ptr_huff & root)
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
		for (auto i : codeRepresentation)
			root->code += std::to_string(i);
		huffmanCode[root->color] = root->code;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
