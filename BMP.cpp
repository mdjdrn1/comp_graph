#include "BMP.h"
#include <fstream>
#include <string>
#include "SDL.h"

BMP::BMP(std::string name) : filename(name)
{
	fopen_s(&bmp_file, filename.c_str(), "rb");

	if (bmp_file == nullptr)
	{
		std::cerr << "Error. Opening file failed.\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}

	read_header();
}

BMP::~BMP()
{
	if (fclose(bmp_file) == EOF)
	{
		std::cerr << "Error. Closing file failed.\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}
}

/*
 * Read RGB values from every pixel in image and pass them to 'os' stream.
 */
void BMP::read_plxs(std::ostream& os) const
{
	// start reading from 54 byte, as first 54 contain header info
	fseek(bmp_file, 54, std::ios::beg);
	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight;	//calculate data size with padding
	unsigned char* data = new unsigned char[data_size];	// data buffer
	fread(data, data_size, 1, bmp_file);
	for (uint i = 0; i < data_size;i += 3)
	{
		int bval = static_cast<int>(data[i]);
		int gval = static_cast<int>(data[i + 1]);
		int rval = static_cast<int>(data[i + 2]);

		os << "R: " << rval << " G: " << gval << " B: " << bval << std::endl;
	}
}


/**
 * \brief Converting input 24-bit BMP file to reduced 24-bit file
 */
void BMP::convert_bmp_7()
{
	// temporary 'creating' file if it didn't exist
	std::fstream outcreate("output.bin", std::ios::trunc);
	outcreate.close();

	FILE* out;

	fopen_s(&out, "output.bin", "wb+");
	/*if(!fopen_s(&out, "output.bin", "wb+"))
	{
		std::cerr << "opening failed.\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}*/

	uint8_t RGB[8];

	// start reading from 54 byte, as first 54 contain header info
	fseek(bmp_file, 54, std::ios::beg);
	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight;	//calculate data size with padding
	unsigned char* data = new unsigned char[data_size];	// data buffer
	fread(data, data_size, 1, bmp_file);

	uint i;
	for (i = 0; i < data_size; ++i)
	{
		RGB[i%7] = static_cast<uint8_t>(data[i]);
		
		if (i!=0 && i % 7 == 0)
		{
			pack(RGB, out);
		}
	}
	/*
	// TURNED OFF ON TESTS
	// latest bits, that didn't fill RGB array fully
	if ((i-1) % 7 != 0)
	{
		i = i % 7;
		while(i<8)
		{
			// fill with zeros remaining values
			RGB[i] = 0;
			++i;
		}
		// pack same as before (in a loop)
		pack(RGB, out);
	}*/
	fclose(out);
}

/**
 * \brief Reducing every byte from 8-bits to 7-bits
 * \param vals 8-bytes array, which will be reduced to 7-bytes
 * \param fp output file pointer
 */
void BMP::pack(uint8_t vals[8], FILE* fp)
{
	uint8_t pack = 0x00;
	uint8_t x, p;
	bool bit = 1;

	for (int t = 0, n = 0; t < 7; ++t, ++n)
	{
		pack = vals[t];
		x = 7;
		p = n;
		for (int k = 0; k <= n; ++k, --p, --x)
		{
			//check bit 
			bit = (vals[t + 1] >> x) & 1;

			if (bit)	//set bit x to 1
				pack |= 1 << p;
			else		//set bit x to 0
				pack &= ~(1 << p);
		}
		vals[t + 1] <<= t + 1;

		fwrite(&pack, sizeof(uint8_t), 1, fp);
	}
}

/**
 * \brief Read RGB values from every pixel in image and display them in SDL window.
 */
void BMP::sdl_display() const
{
	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO);		// Initialize SDL2

	SDL_CreateWindowAndRenderer(	// create window
		header.biWidth+50,
		header.biHeight+50,
		NULL,//SDL_WINDOW_OPENGL | !SDL_WINDOW_RESIZABLE,		// flags
		&window,
		&renderer
		);

	SDL_RenderClear(renderer);

	// background
	for (auto x = 0; x< header.biWidth + 50; ++x)
	{
		for (auto y = 0; y < header.biHeight + 50; ++y)
		{
			SDL_SetRenderDrawColor(renderer, 242, 31, 237, 0);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}


	// start reading from 54 byte, as first 54 contain header info
	fseek(bmp_file, 54, std::ios::beg);

	uint data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight;	//calculate data size with padding
	unsigned char* data = new unsigned char[data_size];	// data buffer

	fread(data, data_size, 1, bmp_file);
	for (uint i = 0, x = 0, y = 0; i < data_size; i += 3, ++x)
	{
		if (x == header.biWidth)
		{
			x = 0;
			++y;
		}

		uint8_t bval = static_cast<uint8_t>(data[i]);
		uint8_t gval = static_cast<uint8_t>(data[i + 1]);
		uint8_t rval = static_cast<uint8_t>(data[i + 2]);
		if (i < 50)
		{
			std::cout << "R: " << rval << " G: " << gval << " B: " << bval << std::endl;
		}

		if(!SDL_SetRenderDrawColor(renderer, rval, gval, bval, 255))
			std::cout<<SDL_GetError();
		SDL_RenderDrawPoint(renderer, x, y);
	}
	
	SDL_RenderPresent(renderer);

	// quit event
	while (1) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	// Clean up
	SDL_Quit();
}

/**
 * \brief reading first 54 file bytes containing header
 */
void BMP::read_header()
{
	fseek(bmp_file, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, bmp_file);
}

std::ostream & operator<<(std::ostream & os, const BMP& obj)
{
	os << "sizeof(header): " << sizeof(obj.header) << std::endl;
	os << "Width: " << obj.header.biWidth << std::endl;
	os << "Height: " << obj.header.biHeight << std::endl;
	os << "Size: " << static_cast<double>(obj.header.bfSize) / 1024 << "KB" << std::endl;
	os << "Signature: " << std::hex << obj.header.bfSignature << std::endl;
	os << "Bit: " << std::dec << obj.header.biBitCount << std::endl;
	os << "Compression: " << std::dec << obj.header.biCompression << std::endl;
	os << std::dec << std::endl;
	return os;
}
