#include "BMP.h"

BMP::BMP(std::string name) : filename(name)
{
	bmpin.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);

	// TODO: Exception handling for file opening
	if (!bmpin.is_open())
	{
		std::cout << "can't open file\n";
		system("Pause");
		exit(EXIT_FAILURE);
	}

	read_header();
}

BMP::~BMP()
{
	bmpin.close();
}

/*
 * Read RGB values from every pixel in image and pass them to 'os' stream.
 */
void BMP::read_plxs(std::ostream& os)
{
	// start reading from 54 byte, as first 54 contain header info
	bmpin.seekg(54, std::ios::beg);

	int data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight;	//calculate data size with padding
	unsigned char* data = new unsigned char[data_size];	// data buffer
	unsigned char temp;
	bmpin.read(reinterpret_cast<char*>(data), data_size);
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
void BMP::sdl_display()
{
	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;

	SDL_Init(SDL_INIT_VIDEO);		// Initialize SDL2
	SDL_CreateWindowAndRenderer(	// create window
		header.biWidth,
		header.biHeight,
		SDL_WINDOW_OPENGL | !SDL_WINDOW_RESIZABLE,		// flags
		&window,
		&renderer
		);

	SDL_RenderClear(renderer);

	// start reading from 54 byte, as first 54 contain header info
	bmpin.seekg(54, std::ios::beg);

	int data_size = ((header.biWidth * 3 + 3) & (~3)) * header.biHeight;	//calculate data size with padding
	unsigned char* data = new unsigned char[data_size];
	unsigned char temp;
	bmpin.read((char*)data, data_size);
	for (auto i = data_size - 4, x = 0, y = 0; i >= 0; i -= 3, ++x)
	{
		temp = data[i];				/*	a bit different			*/
		data[i] = data[i + 2];		/*	than in read_plxs		*/
		data[i + 2] = temp;			/*	these migth be useless!	*/

		int rval = static_cast<int>(data[i] & 0xff);
		int gval = static_cast<int>(data[i + 1] & 0xff);
		int bval = static_cast<int>(data[i + 2] & 0xff);

		SDL_SetRenderDrawColor(renderer, rval, gval, bval, 255);
		SDL_RenderDrawPoint(renderer, x, y);
		if (x == header.biWidth - 1)
		{
			x = -1;
			++y;
		}
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
	unsigned char* header_data;
	header_data = new unsigned char[54];

	bmpin.seekg(0, std::ios::beg);
	bmpin.read(reinterpret_cast<char*>(header_data), 54);

	header.bfSignature = *(reinterpret_cast<ushort*>(&header_data[0]));
	header.bfSize = *(reinterpret_cast<uint*>(&header_data[2]));
	header.bfRes1 = *(reinterpret_cast<ushort*>(&header_data[6]));
	header.bfRes2 = *(reinterpret_cast<ushort*>(&header_data[8]));
	header.bfOffset = *(reinterpret_cast<uint*>(&header_data[10]));

	header.biSize = *(reinterpret_cast<uint*>(&header_data[14]));
	header.biWidth = *(reinterpret_cast<int*>(&header_data[18]));
	header.biHeight = *(reinterpret_cast<int*>(&header_data[22]));
	header.biPlanes = *(reinterpret_cast<ushort*>(&header_data[26]));
	header.biBitCount = *(reinterpret_cast<ushort*>(&header_data[28]));
	header.biCompression = *(reinterpret_cast<uint*>(&header_data[30]));
	header.biSizeImage = *(reinterpret_cast<uint*>(&header_data[34]));
	header.biXPelsPerMeter = *(reinterpret_cast<int*>(&header_data[38]));
	header.biYPelsPerMeter = *(reinterpret_cast<int*>(&header_data[42]));
	header.biClrUsed = *(reinterpret_cast<uint*>(&header_data[46]));
	header.biClrImportant = *(reinterpret_cast<uint*>(&header_data[50]));

	delete[] header_data;
}

std::ostream & operator<<(std::ostream & os, const BMP& obj)
{
	os << "Width: " << obj.header.biWidth << std::endl;
	os << "Height: " << obj.header.biHeight << std::endl;
	os << "Size: " << static_cast<double>(obj.header.bfSize) / 1024 << "KB" << std::endl;
	os << "Signature: " << std::hex << obj.header.bfSignature << std::endl;
	os << std::dec << std::endl;
	return os;
}
