#include "Huffman.h"
#include "sdl_utils.h"

#include <SDL.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>

//////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////
struct Huffman::Header
{
	ullong countColor;		// quantity color channels
	ushort size_map_code;	// the largest code size 
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
	size_map_code(0)
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
	huffman_header.size_map_code = size_map_code / 8 + 1; // as byte
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
		for (ushort i = 0; i < 256; ++i)
		{
			// If code isnt empty
			unsigned char byte = 0;			// added byte
			if (codeRepresentation[i].size())
			{
				uint countsByte = 0;		// quantity of added bytes
				short position = 7;		// position in byte
				for (int it = 0; it < codeRepresentation[i].size(); ++it)
				{
					byte |= (codeRepresentation[i][it] << position);
					if (--position == -1)
					{
						out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
						//if(i == 0)
						//	std::cout << i << ". " << decToBin(byte) << std::endl;
						byte = 0; 
						position = 7;
						++countsByte;
						
					}
					
				} 
				// Add bits to align the map huffman codes
				for (; countsByte < huffman_header.size_map_code; ++countsByte)
				{
					//if (i == 0)
					//	std::cout << i << ". " << decToBin(byte) << std::endl;
					out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
					byte = 0;
				}

			}
			// If code is empty
			else
			{
				for(int it = 0; it < huffman_header.size_map_code; ++it)
					out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
			}
			
		}
	}

	//for (uint i = 0; i < 256; ++i)
	//{
	//		std::cout << (unsigned)i << ". ";
	//		for (int it = 0; it < codeRepresentation[i].size(); ++it)
	//			std::cout << codeRepresentation[i][it];
	//		std::cout << std::endl;
	//}
		
	/////////////////////////////////////////////////////////////////////////
	// Main loop reading data frome BMP file 
	// byte by byte and save huffman code bit by bit
	/////////////////////////////////////////////////////////////////////////
	bool bit;
	byte byte = 0;
	short position = 7;
	for (int y = 0; y < pics->h; ++y) // ITS A PRANK BRO
		for (int x = 0; x < pics->w; ++x)
		{
			arr = SDL_utils::get_pixel(pics, x, y);
			for (int index = 0; index < 3; ++index)
			{
				for (int i = 1; i < codeRepresentation[arr[index]].size() - 1; ++i)
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
	std::map <std::string, uint8_t> mapCode;

	// Read if exist color channels
	if (huff_header.headerType)
	{
		// later
	}
	// Read all color channels
	else
	{
		for (ushort i = 0; i < 256; ++i)
		{
			byte _byte;
			std::string code;
			std::string temp_byte;
			for (int it = 0; it < huff_header.size_map_code; ++it)
			{
				/// TUTAJ JAKOS TO TRZEBA PRZYSPIESZYC
				in_file.read(reinterpret_cast<char*>(&_byte), sizeof(_byte));
				temp_byte = decToBin(_byte); 
				// If its empty add zero at the beginning byte
				if(!temp_byte.empty()) 
					while (temp_byte.size() != 8)
						temp_byte = '0' + temp_byte;
				code += temp_byte;
			}
			if (!code.empty())
			{
				// Clear first and last control bit
				code = code.substr(code.find_first_of('1') + 1, code.find_last_of('1') - 1);
				mapCode.insert({code, i});
			}
			
			code.clear();
		}
	}
	/////////////////////////////////////////////////////////////////////////
	// Read encoded data from .bard file
	/////////////////////////////////////////////////////////////////////////
	SDL_Surface* decoded_image = SDL_utils::new_empty_surface(huff_header.width, huff_header.height);
	std::string temp_color;
	int index = 0;
	uint8_t RGB[3];
	ullong countColor = huff_header.countColor * 3;
	int x = 0, y = 0;
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
			if (mapCode.count(temp_color) > 0 && countColor--)
			{
				RGB[index++] = mapCode.find(temp_color)->second;
				if (index == 3)
				{
					SDL_utils::draw_pixel(decoded_image, x++, y, RGB[0], RGB[1], RGB[2]);
					if (x == huff_header.width)
					{
						x = 0;
						++y;
					}
					index = 0;
				}
				temp_color.clear();
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
		codeRepresentation[root->color].push_back(true);
		if (code.size() > size_map_code) // check max map size
		{
			size_map_code = code.size();
			//std::cout << "KOD: " << (unsigned) root->color << std::endl;
		}
			
	}
}
