#include "huffman.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <bitset>

std::string decToBin(unsigned short n)
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
	return bin;
}

huffman::huffman()
{
	max = 0;
}

void huffman::encode(std::string in_file_name, std::string out_file_name)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////      KOMPRESJA      /////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	// OPEN BMP FILE
	std::ifstream in_file(in_file_name, std::ios::binary); 
	if(in_file.fail())
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
		in_file.read((char*)&color, sizeof(uint8_t));

		// counts frequency colors
		++frequencyColor[color];
	}

	// CREATE HUFFMAN CODE
	makeTree(frequencyColor);
	//std::cout << "ROZMIAR MAX: " << max << std::endl;

	// save header to file
	std::ofstream out_file(out_file_name, std::ios::binary);
	if (out_file.fail())
	{
		std::cout << "cannot open file! \n ";
		exit(0);
	}
	std::sort(vecHuffman.begin(), vecHuffman.end());
	//for (int i = 0; i < vecHuffman.size(); ++i)
	//	std::cout << (unsigned) vecHuffman[i].first << " " << vecHuffman[i].second << std::endl;

	for (unsigned short i = 0, j = 0, k = 0; i < 256; ++i)
	{
		if (i == vecHuffman[j].first)
		{
			// binary huffman code to decimal value
			std::string::size_type buffer = 0;
			unsigned short code = std::stoul(vecHuffman[j].second, &buffer, 2);
			uint8_t number_of_zeros = 0;
			for (size_t i = 0; i < vecHuffman[j].second.size(); ++i)
				if (vecHuffman[j].second[i] == '0')
					++number_of_zeros;
				else
					break;

			out_file.write((char*)&number_of_zeros, sizeof(number_of_zeros));	// send: how many zeros is in befor decimal code	(1 byte)
			out_file.write((char*)&code, sizeof(code));							// send: huffman code(decimal)						(2 bytes)
			std::cout << i << " " << vecHuffman[j].second << " " << code << std::endl;
			j++;
		}
		else
		{
			out_file.write((char*)&k, sizeof(uint8_t));
			out_file.write((char*)&k, sizeof(k));
		}
	}

	out_file.close();
	in_file.close();
}

void huffman::decode(std::string in_file_name, std::string out_file_name)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////     DEKOMPRESJA     /////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	std::ifstream in_file(in_file_name, std::ios::binary);
	std::map <uint8_t, std::string> mapCode;

	// read huffman code form file
	for (int i = 0; i < 256; ++i)
	{
		unsigned short code_short; 
		uint8_t number_of_zeros;

		in_file.read((char*)&number_of_zeros, sizeof(number_of_zeros)); 
		in_file.read((char*)&code_short, sizeof(code_short));
		
		std::string code = decToBin(code_short);
		for (int i = 0; i < number_of_zeros; ++i)
			code = '0' + code;
		//if(code_short)
			std::cout << i << " " << code << " " << code_short << std::endl;
	}
}


void huffman::writeCode(bool &bit, short &bitCount, unsigned char &temp)
{
	temp |= (bit << bitCount);
		if (++bitCount == 8)
		{
			temp = 0;
			bitCount = 0;
		}
}

void huffman::makeTree(int *frequencyColor)
{
	for (int i = 0; i < 256; ++i)
		if (frequencyColor[i])
		{
			// save data like color and frequency 
			auto root = std::make_shared<huffman::node>(i, frequencyColor[i]);

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

		auto root = std::make_shared<huffman::node>(256, leftNode->frequency + rightNode->frequency);
		root->left = leftNode;
		root->right = rightNode;
		huffmanQueue.push(root);
	}

	returnCode(huffmanQueue.top());
}

void huffman::returnCode(const shared_ptr_huff &root)
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
		// std::cout << "Kolor: " << std::setw(3) << (unsigned)root->color << " Czestotliwosc: " << std::setw(6) << root->frequency << " Kod: ";
		for (auto i : codeRepresentation)
			root->code += std::to_string(i);
		if (root->code.size() > max)
			max = root->code.size();
		vecHuffman.push_back(std::make_pair(root->color, root->code));
		// std::cout << root->code << std::endl;
	}
}

void huffman::TESTER()
{

}

huffman::~huffman()
{
		// next time :)
}

huffman::node::node(uint8_t color, int frequency)
	: color(color), left(nullptr), right(nullptr), frequency(frequency)
{
	// ctor
}

