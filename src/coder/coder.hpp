#ifndef CODER_HPP___
#define CODER_HPP___

#include <string>
#include <vector>
#include "../sdl_utils/sdl_utils.hpp"
#include "../error/error.hpp"

class Coder
{
public:
	using ushort = unsigned short;
	using uint = unsigned int;
	using ull = unsigned long long;

	using DataVector = std::vector<uint8_t>; // bytes vector
	using Pixel = SDL_utils::Pixel; // Pixel = std::array<uint8_t, 3>

	enum mode
	{
		BITPACK,
		HUFF,
		RLE_EN
	};
#pragma pack(push, 1)	// set struct alignment to 0 bytes to 'turn off' paddings
	struct Header
	{
		// TODO: Signature XDDD
		// TODO: Add Huffman's codes
		uint offset; // offset to data, should be 15(?) bytes
		int width;
		int height;
		bool grayscale; // 0 or 1
		ushort compression; // 0=BITPACK, 1=HUFFMAN, 2=RLE
		Header(SDL_Surface* image, const mode& compression_mode, const bool& grayscale_choice);
		explicit Header(std::fstream& input);
	};
#pragma pack(pop)

	virtual ~Coder() = default;

	virtual void encode(const std::string& filename, const bool& grayscale) = 0;
	virtual void decode(const std::string& filename) = 0;

	protected:
	std::string encoded_filename(const std::string& input_filename) const;
	std::string decoded_filename(const std::string& input_filename) const;
	void draw_pixels(const SDL_Surface& image, DataVector& pixels, int& x, int& y); // auxiliary method
};


#endif // Coder_HPP___
