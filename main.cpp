#define _CRT_SECURE_NO_WARNINGS

#include "huffman.h"
#include <fstream>
#include <iostream>

int main()
{
	std::cout << "//////// KODOWANIE HUFFMANA \\\\\\\\\\\\\\\\\\ " << std::endl;
	huffman TEST("1.bmp");
	
	
	// TEST.TESTER();

	system("PAUSE");
	return 0;
}
