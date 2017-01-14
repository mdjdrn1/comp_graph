#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <numeric>
#include <string>
#include "converter/converter.hpp"

void testConverter(const std::vector<std::string>& names, const Converter::mode& mode, unsigned number_of_tests);

int main(int argc, char** argv)
{
	std::fstream logfile("log.txt", std::ios_base::out | std::ios_base::trunc);
	std::cout.rdbuf(logfile.rdbuf()); // redirect stdout to log file

	using StringVector = std::vector<std::string>;

	StringVector filenames = {"test\\pics\\1.bmp", "test\\pics\\2.bmp",
		"test\\pics\\3.bmp", "test\\pics\\4.bmp",
		"test\\pics\\5.bmp", "test\\pics\\6.bmp",
		"test\\pics\\7.bmp"};

	try
	{
		testConverter(filenames, Converter::mode::BITPACK, 1);
	}
	catch (const Error&)
	{
		std::cout << "Exception catched." << std::endl
			<< "Check errors log in error_log.txt file." << std::endl;
		exit(EXIT_FAILURE);
	}
	catch (...)
	{
		std::cout << "Unknown exception catched." << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout.rdbuf(nullptr);
	return 0;
}

/**
 * \brief Conversion/deconversion test for bitpack.
 * \param names container with file names
 * \param number_of_tests number of tests that will be executed for each file
 */
void testConverter(const std::vector<std::string>& names, const Converter::mode& mode, unsigned number_of_tests)
{
	using uint = unsigned int;
	using ull = unsigned long long;
	std::cout << std::setw(10) << "Cases: " << number_of_tests << std::endl << std::endl;
	Converter converter;
	std::chrono::system_clock::time_point start, end;
	using usVect = std::vector<std::chrono::microseconds::rep>;
	usVect time;
	std::chrono::microseconds::rep av;
	usVect::iterator best, worst;

	for (const auto& name : names)
	{
		time.clear();
		time.reserve(number_of_tests);

		for (uint k = 0; k < number_of_tests; ++k)
		{
			start = std::chrono::system_clock::now();
			converter.encode(name, mode);
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

		std::string new_name = name.substr(0, name.size() - 3) + "bard";

		for (uint k = 0; k < number_of_tests; ++k)
		{
			start = std::chrono::system_clock::now();
			converter.decode(new_name, mode);
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
