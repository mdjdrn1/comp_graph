#ifndef FILE_H___
#define FILE_H___

#include <fstream>
#include <iostream>
#include <string>

class File
{
	std::string m_name;
	std::fstream fs;
public:
	explicit File(std::string filename, int mode);
	~File();
	bool eof();
	bool is_open();
	void seek(off_t offset);
	void read(char* buffer, long long bytes);
	void write(char* buffer, long long bytes);
};

#endif