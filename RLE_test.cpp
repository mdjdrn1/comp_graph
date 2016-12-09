#include <iostream>
#include "RLE.h"

using namespace std;

int main() {
	const char *input = "Lenna.bmp";
	const char *output = "bin";
	const char *ready = "ready.bmp";

	Encode(input, output);
	cout << "compressed." << endl;
	Decode(output, ready);
	cout << "ready." << endl;

	getchar();
	return 0;
}