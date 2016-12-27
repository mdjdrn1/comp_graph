#define _CRT_SECURE_NO_WARNINGS

#include "huffman.h"
#include <fstream>
#include <iostream>

int main()
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////// KODOWANIE HUFFMANA ////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	huffman TEST;
	TEST.encode("2.bmp", "decode.drab");
	TEST.decode("decode.drab", "");
	
	
	// TEST.TESTER();

	system("PAUSE");
	return 0;
}
