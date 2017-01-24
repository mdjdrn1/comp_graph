#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>

#include <fstream>

#include "Huffman.hpp"
#include "../coder/coder.hpp"
#include "../error/error.hpp"

//#if defined(_WIN32) || defined(_WIN64)
//#include <SDL.h>
//#elif defined(__unix__)
//#include <SDL2/SDL.h>
//#endif

//////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
struct Huffman::Huff_header
{
	ullong countColor; // quantity color channels
	ushort size_map_code; // the largest size codes
	ushort min_map_code = 32000; // the smalltest size codes
	bool headerType;
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////////////////////
struct Huffman::node
{
	uint8_t color; // RGB value
	std::vector<bool> code; // binary representation 
	uint frequency; // color
	shared_ptr_huff left; // child 
	shared_ptr_huff right; // child

	node(uint8_t color, uint frequency);
};

//////////////////////////////////////////////////////////////////////////////////////////
Huffman::node::node(uint8_t color, uint frequency)
	: color(color), left(nullptr), right(nullptr), frequency(frequency)
{
	// ctor
}

//////////////////////////////////////////////////////////////////////////////////////////
Huffman::Huffman()
	: compare([](const shared_ptr_huff& l, const shared_ptr_huff& r)
		  {
			  return (l->frequency > r->frequency);
		  }),
	  huffmanQueue(compare),
	  size_map_code(0), size_codeRepresentation(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void Huffman::encode(const std::string& filename, const bool& grayscale)
{
	std::array<uint8_t, 3> arr;
	uint frequencyColor[256]{};

	// Read bmp file and count frequency of Color
	uint countsColor = 0;
	SDL_Surface_ptr pics(new_bmp_surface(filename));
	for (int y = 0; y < pics->h; ++y)
		for (int x = 0; x < pics->w; ++x)
		{
			arr = Coder::get_pixel(pics.get(), x, y);
			if (grayscale)
			{
				Coder::to_gray(arr);
				++frequencyColor[arr[0]];
			}
			else
			{
				++frequencyColor[arr[0]];
				++frequencyColor[arr[1]];
				++frequencyColor[arr[2]];
				countsColor++;
			}
		}

	// CREATE HUFFMAN CODE
	makeTree(frequencyColor);
	///////////////////////////////////////////////////////////////////////////
	//// Save header to file
	///////////////////////////////////////////////////////////////////////////
	std::fstream out_file(encoded_filename(filename).c_str(),
	                       std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (!out_file.is_open())
		throw Error("In Huffman::encode(): couldn't open output convert file.");

	Header header(pics.get(), HUFF, grayscale);
	out_file.write(reinterpret_cast<char*>(&header), sizeof(header));
	out_file.seekg(header.offset, std::ios_base::beg);

	/////////////////////////////
	Huff_header huffman_header;
	huffman_header.countColor = countsColor;
	huffman_header.size_map_code = size_map_code / 7 + 1; // as byte

	//////////////////////////////
	// Set header type
	// Limit the size of header to a minimum 
	//////////////////////////////
	if (size_codeRepresentation < 120 && size_map_code < 8)
		huffman_header.headerType = true;
	else
		huffman_header.headerType = false;

	out_file.write(reinterpret_cast<char*>(&huffman_header), sizeof(Huff_header));

	// Save if exist color channels
	if (huffman_header.headerType)
	{
		out_file.write(reinterpret_cast<char*>(&size_codeRepresentation), sizeof(uint8_t));
		for (int it = 0; it < 256; ++it)
			if (codeRepresentation[it].size())
			{
				uint8_t color = 0;
				for (int position = 0, i = codeRepresentation[it].size() - 1; i >= 0; --i , ++position)
					color |= (codeRepresentation[it][i] << position);
				out_file.write(reinterpret_cast<char*>(&color), sizeof(uint8_t));
				out_file.write(reinterpret_cast<char*>(&it), sizeof(uint8_t));
			}
	}
	// Save all color channels
	else
	{
		for (int it = 0; it < 256; ++it)
		{
			uint32_t color{};
			if (codeRepresentation[it].size())
			{
				for (int position = 0, i = codeRepresentation[it].size() - 1; i >= 0; --i , ++position)
					color |= (codeRepresentation[it][i] << position);

				out_file.write(reinterpret_cast<char*>(&color), sizeof(uint8_t) * huffman_header.size_map_code);
			}
			else
				out_file.write(reinterpret_cast<char*>(&(color = 0)), sizeof(uint8_t) * huffman_header.size_map_code);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Main loop reading data frome BMP file 
	// byte by byte and save huffman code bit by bit
	/////////////////////////////////////////////////////////////////////////
	uchar byte = 0;
	short position = 7;
	for (int y = 0; y < pics->h; ++y)
		for (int x = 0; x < pics->w; ++x)
		{
			if (grayscale)
			{
				arr = get_pixel(pics.get(), x, y);
				to_gray(arr);
				for (int i = 1; i < codeRepresentation[arr[0]].size(); ++i)
				{
					byte |= (codeRepresentation[arr[0]][i] << position);
					if (--position == -1)
					{
						out_file.write(reinterpret_cast<char*>(&byte), sizeof(byte));
						position = 7;
						byte = 0;
					}
				}
			}
			else
			{
				arr = get_pixel(pics.get(), x, y);
				for (int index = 0; index < 3; ++index)
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

//////////////////////////////////////////////////////////////////////////////////////////
void Huffman::decode(const std::string& filename, const bool& grayscale)
{
	std::fstream in_file(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!in_file.is_open())
		throw Error("In Huffman::decode(): couldn't open input convert file.");

	Header header(in_file);
	in_file.seekg(header.offset, std::ios_base::beg); // set file to read after header

	/////////////////////////////////////////////////////////////////////////
	// Read huffman header from .bard file
	/////////////////////////////////////////////////////////////////////////
	Huff_header huff_header;
	in_file.read(reinterpret_cast<char*>(&huff_header), sizeof(Huff_header));

	// Map hold huffman code each color channels
	std::map<uint32_t, uint8_t> mapCode;

	// Read if exist color channels
	if (huff_header.headerType)
	{
		ushort it;
		in_file.read(reinterpret_cast<char*>(&it), sizeof(uint8_t));
		while (it--)
		{
			uint8_t position, color;
			in_file.read(reinterpret_cast<char*>(&color), sizeof(uint8_t));
			in_file.read(reinterpret_cast<char*>(&position), sizeof(uint8_t));
			mapCode.insert({color, position});
		}
	}
	// Read all color channels
	else
		for (ushort it = 0; it < 256; ++it)
		{
			uint32_t color;
			in_file.read(reinterpret_cast<char*>(&color), sizeof(uint8_t) * huff_header.size_map_code);
			if (color)
				mapCode.insert({color, it});
		}

	/////////////////////////////////////////////////////////////////////////
	// Read encoded data from .bard file
	/////////////////////////////////////////////////////////////////////////
	SDL_Surface_ptr decoded_image(new_empty_surface(header.width, header.height));
//	SDL_Surface* decoded_image(new_empty_surface(header.width, header.height));
	int x = 0, y = 0;

	short position = 0;
	uint32_t map_key = 1;
	ushort index_color = 0;
	uint8_t RGB[3];
	while (!in_file.eof())
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
				if (grayscale)
				{
					RGB[0] = it->second;
					draw_pixel(decoded_image.get(), x++, y, RGB[0], RGB[0], RGB[0]);
//					draw_pixel(decoded_image, x++, y, RGB[0], RGB[0], RGB[0]);
					if (x == header.width)
					{
						x = 0;
						++y;
					}
					index_color = 0;

					map_key = 1;
				}
				else
				{
					RGB[index_color++] = it->second;
					if (index_color >= 3)
					{
						draw_pixel(decoded_image.get(), x++, y, RGB[0], RGB[1], RGB[2]);
//						draw_pixel(decoded_image, x++, y, RGB[0], RGB[1], RGB[2]);
						if (x == header.width)
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
	}
	in_file.close();
	if (SDL_SaveBMP(decoded_image.get(), decoded_filename(filename).c_str()) < 0) // finally, save file to BMP extension
//	if (SDL_SaveBMP(decoded_image, decoded_filename(filename).c_str()) < 0) // finally, save file to BMP extension
		throw Error("In Huffman::decode(): failed saving decoded file.");
//	SDL_FreeSurface(decoded_image);
	// TODO: fix freesurface issue!
}

//////////////////////////////////////////////////////////////////////////////////////////
void Huffman::makeTree(unsigned int* frequencyColor)
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

//////////////////////////////////////////////////////////////////////////////////////////
void Huffman::returnCode(const shared_ptr_huff& root)
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
		++size_codeRepresentation;
		codeRepresentation[root->color] = code;
		// add control bit
		codeRepresentation[root->color].insert(codeRepresentation[root->color].begin(), true);
		if (code.size() > size_map_code) // check max map size
			size_map_code = code.size();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
