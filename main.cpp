#include "huffman.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
int main(int argc, char ** argv)
{
	std::string file_to_encode = "8.bmp";
	std::string file_after_decode = "encode_" + file_to_encode;
	Huffman("img/" + file_to_encode, "img/TESTER.bard");
	Huffman("img/TESTER.bard", "img/" + file_after_decode);

	system("PAUSE");
	return 0;
}
