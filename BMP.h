#ifndef BMP_H
#define BMP_H

#include<string>
#include<iostream>
#include<fstream>
#include "SDL.h"

class BMP
{
private:
	typedef unsigned short ushort;
	typedef unsigned int uint;
	std::fstream bmpin;		// fstream object for reading from image file
	std::string filename;
	#pragma pack(2)	// set struct alignment to 2 bytes
	struct
	{
	// BMP File header part (14 bytes)
		ushort bfSignature;		// signature (must be (hex)4D42)
		uint bfSize;			// file size in bytes (unreliable)
		ushort bfRes1;			// reserved 1
		ushort bfRes2;			// reserved 2
		uint bfOffset;			// offset to data
	// BMP info header part (40 bytes) (BITMAPINFOHEADER)
		uint biSize;			// BMP info header size in bytes (must be 40)
		int  biWidth;			// image width (in pixels)
		int  biHeight;			// image height (in pixels)
		ushort  biPlanes;		// number of planes (must be 1)
		ushort  biBitCount;		// bits per pixel (1, 4, 8, or 24)
		uint biCompression;		// compression type (0=none, 1=RLE-8, 2=RLE-4)
		uint biSizeImage;		// size of image data in bytes(including padding)
		int  biXPelsPerMeter;	// horizontal resolution in pixels per meter (unreliable)
		int  biYPelsPerMeter;	// vertical resolution in pixels per meter (unreliable)
		uint biClrUsed;			// number of colors in image, or zero
		uint biClrImportant;	// number of important colors, or zero
	} header, *pheader;			// header and header pointer

	void read_header();
public:
	BMP(std::string name);
	~BMP();
	inline int get_width() { return header.biWidth; }
	inline int get_height() { return header.biHeight; }
	void read_plxs(std::ostream& os);
	void sdl_display();
	friend std::ostream& operator<< (std::ostream& os, const BMP& obj);
};

#endif