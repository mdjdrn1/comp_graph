#ifndef CODER_H___
#define CODER_H___

#include "../sdl_utils/sdl_utils.hpp"
#include "../error/error.hpp"
#include <vector>

class Coder
{
public:
	using ushort = unsigned short;
	using uint = unsigned int;
	using ull = unsigned long long;

	using DataVector = std::vector<uint8_t>; // bytes vector
	using PixArr = SDL_utils::PixArr; // PixArr = std::array<uint8_t, 3>

	enum mode;
	struct bardHeader;

	explicit Coder(mode choice = BITPACK);
	Coder(const Coder&) = delete;
	Coder(Coder&&) = delete;
	Coder& operator=(const Coder&) = delete;
	Coder& operator=(Coder&&) = delete;
	virtual ~Coder() = 0;

	virtual void change_mode(const mode& new_mode); // change coding algorithm
protected:
	mode m_current_mode; // current mode

public:
	enum mode
	{
		BITPACK,
		HUFFMAN,
		RLE
	};
#pragma pack(push, 1)	// set struct alignment to 0 bytes to 'turn off' paddings
	struct Coder::bardHeader
	{
		// TODO: Add var for offset to Huffman's codes array
		uint offset; // offset to data, should be 16 bytes
		int width;
		int height;
		ushort grayscale; // 0 or 1
		ushort compression; // 0=BITPACK, 1=HUFFMAN, 2=RLE
		void create_from_SDLSurface(SDL_Surface* image, mode compression_mode = BITPACK, int grayscale_choice = 0);
		void create_from_encoded_file(std::fstream& input);
	};
#pragma pack(pop)
};

#endif
