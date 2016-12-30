#include "huffman.h"
#include <fstream>
#include <iostream>

int main()
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////// KODOWANIE HUFFMANA ////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	
	huffman("2.bmp", "TESTER");
	system("PAUSE");
	return 0;
}