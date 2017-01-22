#include "Huffman.h"
#include "sdl_utils.h"

#include <SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <math.h>
#include <bitset>
//////////////////////////////////////////////////////////////////////////////////////////
struct Huffman::Header
{
	ullong countColor;		// quantity color channels
	ushort size_map_code;	// the largest size codes
	ushort min_map_code = 32000;	// the smalltest size codes

	ushort height;			// image
	ushort width;			// image
	bool headerType;		// ...	
	bool greyType;			// ...
	
};
struct Huffman::node
{
	uint8_t color;					// RGB value
	std::vector <bool> code;		// binary representation 
	uint frequency;					// color
	shared_ptr_huff left;			// child 
	shared_ptr_huff right;			// child

	node(uint8_t color, uint frequency);
};
Huffman::node::node(uint8_t color, uint frequency)
	: color(color), left(nullptr), right(nullptr), frequency(frequency)
{
	// ctor
}
Huffman::Huffman(std::string in_file_name, std::string out_file_name)
	: compare([](const shared_ptr_huff &l, const shared_ptr_huff &r)
	{return (l->frequency > r->frequency); }), huffmanQueue(compare),
	size_map_code(0), min_map_code(0)
{
	// Check input file extesion
	// If you want compress bmp file
	if ((in_file_name.substr(in_file_name.find_last_of(".") + 1) == "bmp") && 
		(out_file_name.substr(out_file_name.find_last_of(".") + 1) == "bard"))
			encode(in_file_name, out_file_name);
	// If you want decompress bard file
	else if ((in_file_name.substr(in_file_name.find_last_of(".") + 1) == "bard") &&
			(out_file_name.substr(out_file_name.find_last_of(".") + 1) == "bmp"))
				decode(in_file_name, out_file_name);
	// Extesion not found
	else
	{
		std::cout << "Wrong file extension! Try again!" << std::endl;
	}
}

void Huffman::encode(std::string in_file_name, std::string out_file_name)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////      KOMPRESJA      /////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;

	std::array<uint8_t, 3> arr;
	uint frequencyColor[256];
	memset(frequencyColor, 0, sizeof(uint) * 256);

	// Read bmp file 
	uint countsColor = 0;
	SDL_Surface *pics = SDL_utils::new_bmp_surface(in_file_name);
	for (int y = 0; y < pics->h; ++y)
		for (int x = 0; x < pics->w; ++x)
		{
			arr = SDL_utils::get_pixel(pics, x, y);
			++frequencyColor[arr[0]];
			++frequencyColor[arr[1]];
			++frequencyColor[arr[2]];
			countsColor++;;
		}
	// CREATE HUFFMAN CODE
	makeTree(frequencyColor);
	///////////////////////////////////////////////////////////////////////////
	//// Save header to file
	///////////////////////////////////////////////////////////////////////////
	std::ofstream out_file(out_file_name, std::ios::binary);
	if (out_file.fail())
	{
		std::cout << "Cannot open file! \n ";
		exit(0);
	}
	/////////////////////////////
	Header huffman_header;
	huffman_header.height = pics->h;
	huffman_header.width = pics->w;
	huffman_header.greyType = false;
	huffman_header.headerType = false;
	huffman_header.countColor = countsColor;
	huffman_header.size_map_code = size_map_code / 7 + 1; // as byte
	huffman_header.min_map_code = min_map_code;
	//////////////////////////////
	out_file.write(reinterpret_cast<char*>(&huffman_header), sizeof(Header));
	
	// Save if exist color channels
	if (huffman_header.headerType)
	{
		// later
	}
	// Save all color channels
	else
	{
		for (ushort it = 0; it < 256; ++it)
		{
			uint32_t color{};
			if (codeRepresentation[it].size())
			{
				for (int position = 0, i = codeRepresentation[it].size() - 1; i >= 0; --i, ++position)
					color |= (codeRepresentation[it][i] << position);
				//std::cout << it << ". " <<  decToBin(color) << std::endl;
				out_file.write(reinterpret_cast<char*>(&color), sizeof(uint8) * huffman_header.size_map_code);
			}
			else
				out_file.write(reinterpret_cast<char*>(&(color = 0)), sizeof(uint8) * huffman_header.size_map_code);
		}
	}


	/////////////////////////////////////////////////////////////////////////
	// Main loop reading data frome BMP file 
	// byte by byte and save huffman code bit by bit
	/////////////////////////////////////////////////////////////////////////
	bool bit;
	uchar byte = 0;
	short position = 7;
	for (int y = 0; y < pics->h; ++y) // ITS A PRANK BRO
		for (int x = 0; x < pics->w; ++x)
		{
			arr = SDL_utils::get_pixel(pics, x, y);
			for (int index = 0; index < 3; ++index)
			{
				for (int i = 1; i < codeRepresentation[arr[index]].size(); ++i)
				{
					byte |= (codeRepresentation[arr[index]][i] << position);
					if (--position == -1)
					{
						out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
						position = 7;
						byte = 0;
					}
				}
			}
		}
	out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte)); // last bajt with some garbage
	out_file.close();
}

void Huffman::decode(std::string in_file_name, std::string out_file_name)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////     DEKOMPRESJA     /////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	Header huff_header;
	std::ifstream in_file(in_file_name, std::ios::binary);
	if(in_file.fail())
	{
		std::cout << "Cannot open file! \n ";
		exit(0);
	}
	/////////////////////////////////////////////////////////////////////////
	// Read huffman header from .bard file
	/////////////////////////////////////////////////////////////////////////
	in_file.read(reinterpret_cast<char*>(&huff_header), sizeof(Header));

	// Map hold huffman code each color channels
	std::map <uint32_t, uchar> mapCode;

	// Read if exist color channels
	if (huff_header.headerType)
	{
		// later
	}
	// Read all color channels
	else
	{
		for (ushort it = 0; it < 256; ++it)
		{
			uint32_t color;
			in_file.read(reinterpret_cast<char*>(&color), sizeof(uint8) * huff_header.size_map_code);
			if (color)
			{
				mapCode.insert({color, it});
				//std::cout << it << ". " << decToBin(color) << std::endl;
			}

		}

		//for (auto it = mapCode.begin(); it != mapCode.end(); ++it)
		//	std::cout << (unsigned) it->second << " " <<  decToBin(it->first) << std::endl;
	}
	/////////////////////////////////////////////////////////////////////////
	// Read encoded data from .bard file
	/////////////////////////////////////////////////////////////////////////
	SDL_Surface* decoded_image = SDL_utils::new_empty_surface(huff_header.width, huff_header.height);
	//std::string temp_color;
	//int index = 0;
	//uint8_t RGB[3];
	ullong countColor = huff_header.countColor * 3;
	int x = 0, y = 0;

	//in_file.read(reinterpret_cast<char*>(&buffer[0]), countColor);

	short position = 0;
	short index = 7;
	uint32_t map_key = 1;
	ushort index_color = 0;
	uint8_t RGB[3];
	while(!in_file.eof())
	{
		uint8_t color = 0;
		in_file.read(reinterpret_cast<char*>(&color), sizeof(color));
		for (int i = 0, index = 7; i < CHAR_BIT; ++i)
		{
			bool bit;
			bit = ((color >> index--) & 1);
			map_key <<= 1;
			map_key |= (bit << position);
			auto it = mapCode.find(map_key);
			if (it != mapCode.end()) 
			{
				RGB[index_color++] = it->second;
				if (index_color >= 3)
				{
					SDL_utils::draw_pixel(decoded_image, x++, y, RGB[0], RGB[1], RGB[2]);
					if(x == huff_header.width)
					{
						x = 0;
						++y;
					}
					index_color = 0;
				}
				map_key = 1;
			}
		}
	}

	SDL_SaveBMP(decoded_image, out_file_name.c_str()); // finally, save file to BMP extension
	SDL_utils::delete_surface(decoded_image); // clean up surface
}

void Huffman::makeTree(unsigned int * frequencyColor)
{
	for (int i = 0; i < 256; ++i)
		if (frequencyColor[i])
		{
			// save data like color and frequency 
			auto root = std::make_shared<node>(i, frequencyColor[i]);

			// create node and push to queue
			huffmanQueue.push(root);
		}
	// create huffman tree
	while (huffmanQueue.size() > 1)
	{
		auto leftNode = huffmanQueue.top();
		huffmanQueue.pop();

		auto rightNode = huffmanQueue.top();
		huffmanQueue.pop();

		auto root = std::make_shared<node>(256, leftNode->frequency + rightNode->frequency);
		root->left = leftNode;
		root->right = rightNode;
		huffmanQueue.push(root);
	}

	returnCode(huffmanQueue.top());
}

void Huffman::returnCode(const shared_ptr_huff & root)
{
	// Create huffman code and send to vector codeRepresentation
	if ((root->left != nullptr) && (root->right != nullptr))
	{
		code.push_back(0);
		returnCode(root->left);
		code.pop_back();
		code.push_back(1);
		returnCode(root->right);
		code.pop_back();
	}
	else
	{
		codeRepresentation[root->color] = code;
		// add first and last control bit
		codeRepresentation[root->color].insert(codeRepresentation[root->color].begin(), true);
		//codeRepresentation[root->color].push_back(true);
		//std::cout << (unsigned) root->color << " KOD: " << decToBin(root->color) << std::endl;
		//std::cout << (unsigned)root->color << " KOD: ";

		//for (int j = 0; j < codeRepresentation[root->color].size(); ++j)
		//	std::cout << codeRepresentation[root->color][j];
		//std::cout << std::endl;
		
		if (code.size() > size_map_code) // check max map size
		{
			size_map_code = code.size();
			
		}
		else if ((code.size() < min_map_code))
			min_map_code = code.size();
			
	}
}
