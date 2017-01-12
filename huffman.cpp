#include "huffman.h"
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
huffman::huffman(std::string in_file_name, std::string out_file_name)
{
	// check input file extesion
	if (in_file_name.substr(in_file_name.find_last_of(".") + 1) == "bmp")
	{
		encode(in_file_name, out_file_name);
	}
	else if (in_file_name.substr(in_file_name.find_last_of(".") + 1) == "bard")
	{
		decode(in_file_name, out_file_name);
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
	//std::ifstream in_file(in_file_name, std::ios::binary);
	//if (in_file.fail())
	//{
	//	std::cout << "Cannot open file! \n ";
	//	exit(0);
	//}
	//in_file.seekg(54);

	//// COUNT FREQUENCY COLOR 
	//int frequencyColor[256];
	//memset(frequencyColor, 0, sizeof(int) * 256);

	//uint8_t color;

	//unsigned int sizeFile = 0;
	//while (!in_file.eof())
	//{
	//	// Lead colors
	//	in_file.read((char*)&color, sizeof(uint8_t));
	//	//std::cout << sizeFile << " " << (unsigned)color << ", " << std::endl;
	//	++sizeFile;
	//	// Counts frequency colors
	//	++frequencyColor[color];
	//}

	std::array<uint8_t, 3> arr;
	unsigned long long frequencyColor[256];
	memset(frequencyColor, 0, sizeof(int) * 256); // POMYSLY NA SZYBSZE ZEROWANIE ?!
	SDL_Surface *pics =  SDL_utils::new_bmp_surface(in_file_name);

	size_t countsPixels = 0; // how many pixels is in file
	for(int y = 0; y < pics->h; ++y)
		for (int x = 0; x < pics->w; ++x)
		{
			arr = SDL_utils::get_pixel(pics, x, y);
			++frequencyColor[arr[0]];
			++frequencyColor[arr[1]];
			++frequencyColor[arr[2]];
			countsPixels += 3;
			//std::cout << "Pixel  : " << (unsigned)arr[0] << " " << (unsigned)arr[1] << " " << (unsigned)arr[2] << " " << std::endl;
		}
	
	//// CREATE HUFFMAN CODE
	makeTree(frequencyColor);

	/////////////////////////////////////////////////////////////////////////
	// Save header to file
	/////////////////////////////////////////////////////////////////////////
	std::ofstream out_file(out_file_name, std::ios::binary);
	if (out_file.fail())
	{
		std::cout << "cannot open file! \n ";
		exit(0);
	}

	///////////////////////
	int h = pics->h, w = pics->w;
	out_file.write((char*)&countsPixels, sizeof(countsPixels));
	out_file.write((char*)&h, sizeof(h));
	out_file.write((char*)&w, sizeof(w));
	////////////////
	int max = 0;
	for (unsigned short i = 0, code; i < 256; ++i)
	{
		if (!huffmanCode[i].empty())
		{
			// binary huffman code to decimal value
			std::string::size_type buffer = 0;
			if (max < huffmanCode[i].size())
				max = huffmanCode[i].size();
			//std::cout << i << " " << huffmanCode[i] << std::endl;

			huffmanCode[i] = '1' + huffmanCode[i];
			code = std::stoi(huffmanCode[i], &buffer, 2);

			//std::cout << i << " " << huffmanCode[i] << " " << code << std::endl;
			out_file.write((char*)&code, sizeof(code));
		}
		else
			out_file.write((char*)&(code = 0), sizeof(code));
	}
	
	/////////////////////////////////////////////////////////////////////////
	// Read bytes from bmp file and write huffman code to compress file
	/////////////////////////////////////////////////////////////////////////
	// Main functions loop reading data frome file byte by byte and save huffman code bit by bit
	/////////////////////////////////////////////////////////////////////////
	bool bit;
	unsigned char byte = 0;
	short position = 7;
	for (int y = 0; y < pics->h; ++y) // ITS A PRANK BRO
		for (int x = 0; x < pics->w; ++x)
		{
			arr = SDL_utils::get_pixel(pics, x, y);
			for (int index = 0; index < 3; ++index)
			{
				for (int i = 1; i < huffmanCode[arr[index]].size(); ++i)
						{
							if (huffmanCode[arr[index]][i] == '1')
								bit = 1;
							else
								bit = 0;
							byte |= (bit << position);
							if (--position == -1)
							{
								out_file.write((char*)&byte, sizeof(byte));
								//std::cout << (unsigned)byte << std::endl;
								position = 7; 
								byte = 0;
							}
						}
			}
		}
	out_file.write((char*)&byte, sizeof(byte)); // last bajt with some garbage
			
	//while (!in_file.eof())
	//{
	//	uint8_t color;
	//	in_file.read((char*)&color, sizeof(color));
	//	for (int i = 1; i < huffmanCode[color].size(); ++i)
	//	{
	//		if (huffmanCode[color][i] == '1')
	//			bit = 1;
	//		else
	//			bit = 0;
	//		byte |= (bit << position);
	//		if (--position == -1)
	//		{
	//			out_file.write((char*)&byte, sizeof(byte));
	//			//std::cout << (unsigned)byte << std::endl;
	//			position = 7; 
	//			byte = 0;
	//		}
	//	}
	//}

	//out_file.write((char*)&byte, sizeof(byte)); // last bajt with some garbage

	//std::cout << "MAXYMALNY ROZMIAR: " << max << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void huffman::encode::makeTree(unsigned long long *frequencyColor)
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
huffman::decode::decode(std::string in_file_name, std::string out_file_name)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////     DEKOMPRESJA     /////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	
	std::ifstream in_file(in_file_name, std::ios::binary);
	//std::ofstream out_file(out_file_name, std::ios::binary);
	/// POZA KONTROLO :D
	std::map <std::string, uint8_t> mapCode;
	size_t countsPixels;
	int w, h;
	in_file.read((char*)&countsPixels, sizeof(countsPixels)); // read quantity pixels
	in_file.read((char*)&h, sizeof(int));
	in_file.read((char*)&w, sizeof(int));
	for (int i = 0; i < 256; ++i)
	{
		unsigned short _code;
		in_file.read((char*)&_code, sizeof(_code));
		std::string code = decToBin(_code);
		
		if (!code.empty())
		{
			code.erase(code.begin());
			//std::cout << i << ". " << code << std::endl;
			mapCode.insert({code, i });
		}
	}
	//std::cout << "Wysokosc: " << h << " Szerokosc: " << w << std::endl;

	//for (const auto &p : mapCode) 
	//	std::cout << "KOD[" << p.first << "] - kolor: " << (unsigned) p.second << '\n';
	//////////////////////////////////////////////////////////
	int x = 0, y = 0;
	SDL_Surface* decoded_image = SDL_utils::new_empty_surface(w, h);
	

	std::string temp_color;
	int index = 0;
	uint8_t RGB[3];
	while (!in_file.eof())
	{
		uint8_t _color;
		std::string color;
		in_file.read((char*)&_color, sizeof(_color));
		color = decToBin(_color);
		
		while (color.size() != 8)
			color = '0' + color;
		//std::cout << color << std::endl;

		for (int i = 0; i < color.size(); ++i)
				{
					temp_color += color[i];
					if (mapCode.count(temp_color) > 0 && countsPixels--) // Przydaly sie systemy xdddd
					{
						
						RGB[index++] = mapCode.find(temp_color)->second;
						
						if (index == 3)
						{
							SDL_utils::draw_pixel(decoded_image, x, y, RGB[0], RGB[1], RGB[2]);
							++x;
							if (x == w)
							{
								x = 0;
								++y;
							}
							//std::cout << (unsigned)RGB[0] << " " << (unsigned)RGB[1] << " " << (unsigned)RGB[2] << std::endl;
							index = 0;
						}
						//std::cout << (unsigned) mapCode.find(temp_color)->second << std::endl;
						//out_file.write((char*)&mapCode.find(temp_color)->second, sizeof(mapCode.find(temp_color)->second));
						temp_color.clear();
					}
				}
		
		

		
	}
	SDL_SaveBMP(decoded_image, out_file_name.c_str()); // finally, save file to BMP extension
	SDL_utils::delete_surface(decoded_image); // clean up surface


	///////////////////////////////////////////////
	//std::string temp_color;
	//while (!in_file.eof())
	//{
	//	uint8_t _color;
	//	std::string color;
	//	in_file.read((char*)&_color, sizeof(_color));
	//	color = decToBin(_color);
	//	while (color.size() != 8)
	//		color = '0' + color;
	//	//std::cout << color << std::endl;
	//	for (int i = 0; i < color.size(); ++i)
	//	{
	//		temp_color += color[i];
	//		if (mapCode.count(temp_color) > 0)
	//		{
	//			//std::cout << (unsigned) mapCode.find(temp_color)->second << std::endl;
	//			out_file.write((char*)&mapCode.find(temp_color)->second, sizeof(mapCode.find(temp_color)->second));
	//			temp_color.clear();
	//		}
	//	}
	//}
}
//////////////////////////////////////////////////////////////////////////////////////////
