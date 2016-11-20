#include "BMP.h"

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
	for (auto i = 0; i < data_size;i += 3)
	{
		int bval = static_cast<int>(data[i]);
		int gval = static_cast<int>(data[i + 1]);
		int rval = static_cast<int>(data[i + 2]);

		os << "R: " << rval << " G: " << gval << " B: " << bval << std::endl;
	}
}
/*
 * Read RGB values from every pixel in image and display them in SDL window.
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
	for (auto i = 0, x = 0, y = 0; i < data_size; i += 3, ++x)
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

void BMP::read_header()
{
	// reading first 54 file bytes containing header
	fseek(bmp_file, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, bmp_file);
}

std::ostream & operator<<(std::ostream & os, const BMP& obj)
{
	os << "Width: " << obj.header.biWidth << std::endl;
	os << "Height: " << obj.header.biHeight << std::endl;
	os << "Size: " << static_cast<double>(obj.header.bfSize) / 1024 << "KB" << std::endl;
	os << "Signature: " << std::hex << obj.header.bfSignature << std::endl;
	os << "Bit: " << std::dec << obj.header.biBitCount << std::endl;
	os << "Compression: " << std::dec << obj.header.biCompression << std::endl;
	os << std::dec << std::endl;
	return os;
}
