#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>
#include <string>
#include "src\Converter.h"


/**
 * \brief Conversion/deconversion test for bitpack.
 * \param names container with file names
 * \param number_of_tests number of tests that will be executed for each file
 */
void testConverter(const std::vector<std::string>& names, unsigned number_of_tests);


int main(int argc, char** argv)
{
	std::fstream logFileStream("log.txt", std::ios_base::out | std::ios_base::trunc);
	std::cerr.rdbuf(logFileStream.rdbuf()); // redirect stderr to log file
//	std::cout.rdbuf(sbuf.rdbuf());   // redirect stderr to log file

	using StringVector = std::vector<std::string>;

	StringVector sv = {"test\\pics\\1.bmp", "test\\pics\\2.bmp",
		"test\\pics\\3.bmp", "test\\pics\\4.bmp",
		"test\\pics\\5.bmp", "test\\pics\\6.bmp",
		"test\\pics\\7.bmp"};

	testConverter(sv, 30);

	std::cerr.rdbuf(nullptr);
//    std::cout.rdbuf(nullptr);
	system("PAUSE");
	return 0;
}


void testConverter(const std::vector<std::string>& names, unsigned number_of_tests)
{
	using uint = unsigned int;
	std::cout << std::setw(10) << "Cases: " << number_of_tests << std::endl << std::endl;
	Converter conv(Converter::BITPACK, 0);
	std::string output_name;
	std::chrono::system_clock::time_point start, end;
	using usVect = std::vector<std::chrono::microseconds::rep>;
	usVect time;
	std::chrono::microseconds::rep av;
	usVect::iterator best, worst;

	uint name_size = names.size();
	for (const auto& name : names)
	{
		time.clear();
		time.reserve(number_of_tests);

		for (uint k = 0; k < number_of_tests; ++k)
		{
			start = std::chrono::system_clock::now();
			conv.convert(name);
			end = std::chrono::system_clock::now();
			time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
		}
		av = std::accumulate(time.begin(), time.end(), 0) / time.size();
		best = std::min_element(time.begin(), time.end());
		worst = std::max_element(time.begin(), time.end());
		std::cout << std::setw(10) << "File: " << name << std::endl;
		std::cout << std::setw(10) << "ENCODING" << std::endl;
		std::cout << std::setw(10) << "Worst\t" << *worst / 1000000. << "s\n";
		std::cout << std::setw(10) << "Average\t" << av / 1000000. << "s\n";
		std::cout << std::setw(10) << "Best\t" << *best / 1000000. << "s\n";

		time.clear();
		time.reserve(number_of_tests);

		output_name = name.substr(0, name.size() - 3) + "bard";

		for (uint k = 0; k < number_of_tests; ++k)
		{
			start = std::chrono::system_clock::now();
			conv.deconvert(output_name);
			end = std::chrono::system_clock::now();
			time.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
		}
		av = std::accumulate(time.begin(), time.end(), 0) / time.size();
		best = std::min_element(time.begin(), time.end());
		worst = std::max_element(time.begin(), time.end());
		std::cout << std::setw(10) << "DECODING" << std::endl;
		std::cout << std::setw(10) << "Worst\t" << *worst / 1000000. << "s\n";
		std::cout << std::setw(10) << "Average\t" << av / 1000000. << "s\n";
		std::cout << std::setw(10) << "Best\t" << *best / 1000000. << "s\n";
	}
}
