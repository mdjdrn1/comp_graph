#include "huffman.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>


huffman::huffman(std::string in_file_name)
{
	std::ifstream in_file(in_file_name, std::ios::binary); // file open
	if (in_file.fail())
	{
		std::cout << "Cannot open file! \n ";
		exit(0);
	}
	in_file.seekg(54); // TAK NA POCZATEK :) zeby sie nie jebac z SDL

	uint8_t color; // Iam not sure this :) maybe unsigned char ?

	int frequencyColor[256];
	memset(frequencyColor, 0, sizeof(int) * 256);
	while (!in_file.eof())
	{
		// lead colors
		in_file.read((char*)&color, sizeof(uint8_t));

		// counts frequency colors
		++frequencyColor[color];
	}

	makeTree(frequencyColor);
	in_file.close();
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
	// 
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
		std::cout << "Kolor: " << std::setw(3) << (unsigned)root->color << " Czestotliwosc: " << std::setw(6) << root->frequency << " Kod: ";
		for (auto i : codeRepresentation)
			root->code += std::to_string(i);
		std::cout << root->code << std::endl;
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

