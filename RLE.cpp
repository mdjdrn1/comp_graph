#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>
#include "RLE.h"

using namespace std;

RLE::RLE() {} ;
RLE::~RLE() {} ;

void RLE::encode(string &input, string &output){

	fstream file;
	fstream compressed;

	file.open(input, ios::in | ios::binary);
	if (!file.is_open()) {
		cout << "cannot open file to encode." << endl;
		getchar();
		exit(1);
	}

	compressed.open(output, ios::out | ios::trunc | ios::binary);
	if (!compressed.is_open()) {
		cout << "cannot open file to save encoded file." << endl;
		getchar();
		exit(1);
	}

	file.read((char*)&current, sizeof(PIXEL));	// read first two pixeles
	file.read((char*)&next, sizeof(PIXEL));
	
	while (!file.eof()){

		if (!compare(current, next)) {		// if different, write repetitions and pixel to file
			compressed.write((char*)&repetition, sizeof(repetition));
			compressed.write((char*)&current, sizeof(PIXEL));
			repetition = 0;
		}
		repetition++;
		current = next;							
		file.read((char*)&next, sizeof(PIXEL));	// read next pixel

		if (file.eof()) {			// if end of file, write repetitions and pixel to file
			compressed.write((char*)&repetition, sizeof(repetition));
			compressed.write((char*)&current, sizeof(PIXEL));
		}
	}

	file.close();
	compressed.close();
}

void RLE::decode(string &input, string &output) {
	fstream file;
	fstream ready;

	file.open(input, ios::in | ios::binary);
	if (!file.is_open()) {
		cout << "cannot open file to decode." << endl;
		getchar();
		exit(1);
	}
	ready.open(output, ios::trunc | ios::out | ios::binary);
	if (!ready.is_open()) {
		cout << "cannot open file to save decoded file." << endl;
		getchar();
		exit(1);
	}

	while (!file.eof()){
		file.read((char*)&repetition, sizeof(repetition));		// read repetitions
		file.read((char*)&current, sizeof(PIXEL));			// read pixel
		if (file.eof()) break;
		for (int j = 0; j < repetition; j++)				// write pixel 'repetition' times
			ready.write((char*)&current, sizeof(PIXEL));
	}

	file.close();
	ready.close();
}


