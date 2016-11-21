#include <iostream>
#include <fstream>
#include "BMP.h"

int main(int argc, char* argv[])
{
	const int SIZE = 6;
	BMP* pics;
	pics = new BMP[SIZE]{ {"test_pics\\pic1.bmp"}, {"test_pics\\pic1_cut.bmp"}, {"test_pics\\pic2.bmp"}, { "test_pics\\pic3.bmp" } ,{ "test_pics\\pic4.bmp" } ,{ "test_pics\\pic5.bmp" } };
	for (int i = 0; i < SIZE; ++i)
		std::cout << pics[i];

	std::fstream out("output.txt", std::ios_base::out | std::ios_base::trunc);
	//pics[4].read_plxs(out);
//	pics[4].sdl_display();
	pics[5].convert_bmp_7();

	system("pause");
	return 0;
}