#ifndef BMP_H___
#define BMP_H___

#include <iostream>
#include <fstream>
#include "File.h"


class BMP
{
private:
	typedef unsigned short ushort;
	typedef unsigned int uint;
	File* m_fs;	// bmp file stream
	std::string m_filename;
	#pragma pack(push, 2)	// set struct alignment to 0 bytes to force 54 bytes stuct size 
	struct
	{
	// BMP File header part (14 bytes)
		ushort bfSignature;		// signature (must be (hex)4D42 or (dec)19778)
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
	} header, *header_ptr;		// header and header pointer
	#pragma pack(pop)
	void read_header();
	void packer(uint8_t vals[8], File& file);	// convert_to_bmp auxiliary method
public:
	BMP(std::string filename);
	~BMP();
	int get_width() const { return header.biWidth; }
	int get_height() const { return header.biHeight; }
	void read_file(std::ostream& os = std::cout);
	void convert_bmp_to_7();
	friend std::ostream& operator<< (std::ostream& os, const BMP& obj);
};

#endif