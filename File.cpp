#include "File.h"

/**
 * @brief: file class with exceptions handling
 * @param filename: just filename
 * @param mode: standard file opening mode for fstream
 * e.g. File foo("spam", std::ios_base::in);
 *		instead of std::fstream bar("spam", std::ios_base::in);
 */
File::File(std::string filename, int mode) 
	: m_name(filename)
{
	fs.open((filename.c_str()), mode);

	if (!fs.is_open())
	{
		std::cerr << "File opening failed." << std::endl;
		system("Pause");
		exit(EXIT_FAILURE);
	}
}

File::~File()
{
	fs.close();
}

/**
 * @brief: check if it's not end of file
 * @return: true if eof reached, otherwise false 
 */
bool File::eof()
{
	return fs.eof();
}

/**
 * @brief: check if file was opened
 * @return: true if opened, otherwise false
 */
bool File::is_open()
{
	return fs.is_open();
}

/**
 * @brief: set position in file
 * @param offset: new position in file
 */
void File::seek(off_t offset)
{
	try
	{
		fs.seekg(offset, std::ios_base::beg);
	}
	catch (std::exception& e)
	{
		std::cerr << "File seek failed. " << std::endl;
		std::cerr << "what(): " << e.what() << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief: reading from file
 * @param buffer: pointer to the start of the buffer
 * @param bytes: number of bytes you want to read into buffer
 */
void File::read(char* buffer, long long bytes)
{
	try
	{
		fs.read(buffer, bytes);
	}
	catch (std::exception& e)
	{
		std::cerr << "Reading from file failed." << std::endl;
		std::cerr << "what(): " << e.what() << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief: writing to file
 * @param buffer: pointer to the start of the buffer
 * @param bytes: number of bytes you want to write from buffer
 */
void File::write(char* buffer, long long bytes)
{
	try
	{
		fs.write(buffer, bytes);
	}
	catch (std::exception& e)
	{
		std::cerr << "Writing to file failed." << std::endl;
		std::cerr << "what(): " << e.what() << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}
}
