#include "RLE.h"
#include <stdio.h>
#include <fstream>

using namespace std;

void Encode(const char *input, const char *output) {
	fstream file;					// original file
	fstream compressed;				// compresed file
	char current;
	char next;
	int fsize = 0;					// file size
	int repetitions = 1;
	
	file.open(input, ios::in | ios::ate | ios::binary);
	compressed.open(output, ios::out | ios::trunc | ios::binary);

	fsize = file.tellg();
		
	for (int i = 0; i < fsize; i++){
		file.seekg(i, ios::beg);
		file.read((char*)&current, sizeof(char)); // get current character
		next = file.peek();
		
		if (next != current){
			compressed.write((char*)&repetitions, sizeof(char));
			compressed.write((char*)&current, sizeof(char));
			repetitions = 0;
		}
		repetitions++;
	}
	file.close();
	compressed.close();
}


void Decode(const char *input, const char *output) {
	fstream file;
	fstream ufile;
	char current;
	char next;
	int repetitions = 0;
	int fsize = 0;
	
	file.open(input, ios::ate | ios::in | ios::binary);
	ufile.open(output, ios::trunc | ios::out | ios::binary);
	fsize = file.tellg();	


	for (int i = 0; i < fsize; i += 2){
		file.seekg(i, ios::beg);
		file.read((char*)&repetitions, sizeof(char));
		file.seekg(i + 1, ios::beg);
		file.read((char*)&current, sizeof(char));
		for (int j = 0; j < repetitions; j++)
			ufile.write((char*)&current, sizeof(char));
	}

	file.close();
	ufile.close();
}