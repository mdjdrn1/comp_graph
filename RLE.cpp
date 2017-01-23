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
	vcopy.push_back(current);
	file.read((char*)&next, sizeof(PIXEL));
	
	while (!file.eof()) {
		if (compare(current, next)) {				// identical pixels
			if (!vcopy.empty()) {// save vector to file
				vcopy.pop_back();					// usun ostatni element - zostanie uwzględniony w tym powtorzeniu
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
			if (vsize < 255) {
				vcopy.push_back(next);
			}
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


	if (file.eof() && !vcopy.empty()) {						// if end of file, write repetitions and pixel to file
		compressed.write((char*)&vstart, sizeof(vstart));	// 0 dla rozroznienia sekwencji od powtorzen
		vsize = vcopy.size();
		compressed.write((char*)&vsize, sizeof(vsize));		// ilosc elementow w sekwencji
		for (int i = 0; i < vsize; i++)						// wypisanie wektora
			compressed.write((char*)&vcopy[i], sizeof(PIXEL));
		vcopy.clear();
	}
	else {
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
		file.read((char*)&repetition, sizeof(repetition));	// read repetitions
		//std::cout << " repetition :"<< (int)repetition << std::endl;
		//file.read((char*)&current, sizeof(PIXEL));			// read pixel
		if (file.eof()) break;
		uint8_t count = 0;

		if (repetition == 0) 
		{
			file.read((char*)&count, sizeof(count));
			//std::cout << " count : " << (int)count << std::endl;
			//file.read((char*)&current, sizeof(PIXEL));
			//std::cout << " current R : " << (int)current.red << std::endl;
			//std::cout << " current G : " << (int)current.green << std::endl;
			//std::cout << " current B : " << (int)current.blue << std::endl;
			for (unsigned int i = 0; i < count; i++)
			{
				file.read((char*)&current, sizeof(PIXEL));
				ready.write((char*)&current, sizeof(PIXEL));
			}
		}
		else {
			file.read((char*)&current, sizeof(PIXEL));
			//std::cout << " current R : " << (int)current.red << std::endl;
			//std::cout << " current G : " << (int)current.green << std::endl;
			//std::cout << " current B : " << (int)current.blue << std::endl;

			for (int j = 0; j < repetition; j++)				// write pixel 'repetition' times
			{
				//std::cout << "w ";
				ready.write((char*)&current, sizeof(PIXEL));
			}
			//std::cout << std::endl;
		}
	}

	file.close();
	ready.close();
}


