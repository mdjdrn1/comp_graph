#include <iostream>
#include <iomanip>
#include <string>
#include "converter/converter.hpp"
#include "error/error.hpp"

using ConverterTuple = std::tuple<bool, std::string, Converter::mode, bool>;

ConverterTuple get_data();

int main(int argc, char** argv)
{
	try
	{
		Converter converter;
		ConverterTuple convert_data = get_data();
		if (std::get<0>(convert_data))
			converter.encode(std::get<1>(convert_data), std::get<2>(convert_data), std::get<3>(convert_data));
		else
			converter.decode(std::get<1>(convert_data));
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

	std::cout << "Finished!" << std::endl;

	return 0;
}

/**
 * \brief Get compression data from user
 * \return ConverterTuple Tuple with necessary data for conversion
 * \ConverterTuple bool coding mode: 0=encode, 1=decode
 * \ConverterTuple std::string filename
 * \ConverterTuple Converter::mode coding algorithm
 * \ConverterTuple bool grayscale
 */
ConverterTuple get_data()
{
	std::string filename;
	std::cout << "SELECT FILE" << std::endl;
	std::cout << ">> ";
	std::cin.clear();
	if (!std::getline(std::cin, filename))
		throw Error("Input error.");

	int coding;
	bool encoding;
	std::cout << "SELECT OPERATION" << std::endl;
	std::cout << "1: " << std::setw(10) << "Encoding" << std::endl;
	std::cout << "2: " << std::setw(10) << "Decoding" << std::endl;
	std::cout << ">> ";
	std::cin.clear();
	if (!(std::cin >> coding))
		throw Error("Input error.");

	if (!(coding == 1 || coding == 2))
		throw Error("Invalid operation choice.");

	encoding = coding == 1 ? true : false;

	Converter::mode algorithm_mode;
	bool grayscale_mode;
	// choice of algorithm mode and grayscale only if encoding, decoding automatically detects grayscale
	if (encoding)
	{
		int algorithm;
		std::cout << "SELECT ALGORITHM" << std::endl;
		std::cout << "1: " << std::setw(10) << "Bitpack" << std::endl;
		std::cout << "2: " << std::setw(10) << "Huffman" << std::endl;
		std::cout << "3: " << std::setw(10) << "RLE" << std::endl;
		std::cout << ">> ";
		std::cin.clear();
		if (!(std::cin >> algorithm))
			throw Error("Input error.");

		if (algorithm < 1 || algorithm > 3 || !(static_cast<Converter::mode>(algorithm) == algorithm))
			throw Error("Invalid algorithm.");

		algorithm_mode = static_cast<Converter::mode>(algorithm - 1);

		int grayscale;
		std::cout << "GRAYSCALE CONVERSION?" << std::endl;
		std::cout << "1: " << std::setw(10) << "Yes" << std::endl;
		std::cout << "2: " << std::setw(10) << "No" << std::endl;
		std::cout << ">> ";
		std::cin.clear();
		if (!(std::cin >> grayscale))
			throw Error("Input error.");

		if (!(grayscale == 1 || grayscale == 2))
			throw Error("Invalid grayscale choice.");

		grayscale_mode = (grayscale == 1);
	}
	return std::make_tuple(encoding, std::move(filename), algorithm_mode, grayscale_mode);
}
