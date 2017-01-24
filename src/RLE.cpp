#include <fstream>
#include <iostream>
#include <cstdint>
#include "RLE.h"

using namespace std;

RLE::RLE() : Coder() {} ;
RLE::~RLE() : Coder() {} ;

void RLE::encode(string &input, string &output){

	fstream file;
	fstream compressed;
	vcopy.clear();

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
	
	while (!file.eof()) {
		
		if (compare(current, next)) {				// identical pixels
			if (!vcopy.empty()) {// save vector to file
				vcopy.pop_back();					// usun ostatni element - zostanie uwzglêdniony w tym powtorzeniu
				compressed.write((char*)&vstart, sizeof(vstart));	// 0 dla rozroznienia sekwencji od powtorzen
				vsize = vcopy.size();
				compressed.write((char*)&vsize, sizeof(vsize));		// ilosc elementow w sekwencji
				for (int i = 0; i < vsize; i++)						// wypisanie wektora
					compressed.write((char*)&vcopy[i], sizeof(PIXEL));
				vcopy.clear();						// uwuniecie zawartosci
			}
			if (repetition < 255)					
				++repetition;
			else {					// gdy liczba powtorzen przekracza 255
				compressed.write((char*)&repetition, sizeof(repetition));
				compressed.write((char*)&current, sizeof(PIXEL));
				repetition = 2;
			}
		}
		else {							// gdy pixele sa rozne
			if (repetition > 1) {		// jesli pixele sie powtarzaly, wypisz je
				compressed.write((char*)&repetition, sizeof(repetition));
				compressed.write((char*)&current, sizeof(PIXEL));
			}
			repetition = 1;
			vsize = vcopy.size();
			if (vsize < 255) 
				vcopy.push_back(next);
			else {						// jesli przekroczono limit nastapi zapis do pliku
				compressed.write((char*)&vstart, sizeof(vstart));
				vsize = vcopy.size();
				compressed.write((char*)&vsize, sizeof(vsize));
				for (int i = 0; i < vsize; i++)
					compressed.write((char*)&vcopy[i], sizeof(PIXEL));
				vcopy.clear();
				vcopy.push_back(next);	// dodaj nowy element
				
			}
		}

		current = next;
		file.read((char*)&next, sizeof(PIXEL));
	}

	if (file.eof()) {						// if end of file, write repetitions and pixel to file
		compressed.write((char*)&repetition, sizeof(repetition));
		compressed.write((char*)&current, sizeof(PIXEL));
	}

	file.close();
	compressed.close();
}

void RLE::decode(string &input, string &output) {
	fstream file;
	fstream ready;

	file.open(input, ios::in | ios::binary);
	if (!file.is_open()) {
		getchar();
		exit(1);
	}
	ready.open(output, ios::trunc | ios::out | ios::binary);
	if (!ready.is_open()) {
		getchar();
		exit(1);
	}

	while (!file.eof()){
		file.read((char*)&repetition, sizeof(repetition));	// read repetitions
		if (file.eof()) break;
		uint8_t count = 0;

		if (repetition == 0) 
		{
			file.read((char*)&count, sizeof(count));
			for (unsigned int i = 0; i < count; i++)
			{
				file.read((char*)&current, sizeof(PIXEL));
				ready.write((char*)&current, sizeof(PIXEL));
			}
		}
		else {
			file.read((char*)&current, sizeof(PIXEL));
			for (int j = 0; j < repetition; j++)				// write pixel 'repetition' times
			{
				ready.write((char*)&current, sizeof(PIXEL));
			}
		}
	}

	file.close();
	ready.close();
}


