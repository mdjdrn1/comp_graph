#include "huffman.h"
#include "sdl_utils.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
int main(int argc, char ** argv)
{
	std::cout << "////////////////////////////////// " << std::endl;
	std::cout << "//////// KODOWANIE HUFFMANA ////// " << std::endl;
	std::cout << "////////////////////////////////// " << std::endl;
	std::string file_to_decode = "0.bmp"; // zle nazwy xd
	std::string file_to_encode = "encode_" + file_to_decode;

	using usVect = std::vector<std::chrono::microseconds::rep>;
	usVect::iterator best, worst;
	usVect time;
	std::chrono::system_clock::time_point start, end;
	start = std::chrono::system_clock::now();
	///
	huffman("img/" + file_to_decode, "img/t.bard");
	///
	end = std::chrono::system_clock::now();
	time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
	best = std::min_element(time.begin(), time.end());
	worst = std::max_element(time.begin(), time.end());
	std::cout << std::setw(10) << "Worst\t" << *worst / 1000000. << "s\n";
	std::cout << std::setw(10) << "Best\t" << *best / 1000000. << "s\n";

	start = std::chrono::system_clock::now();
	///
	huffman("img/t.bard", "img/" + file_to_encode);
	///
	end = std::chrono::system_clock::now();
	time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
	best = std::min_element(time.begin(), time.end());
	worst = std::max_element(time.begin(), time.end());
	std::cout << std::setw(10) << "Worst\t" << *worst / 1000000. << "s\n";
	std::cout << std::setw(10) << "Best\t" << *best / 1000000. << "s\n";


	system("PAUSE");
	return 0;
}