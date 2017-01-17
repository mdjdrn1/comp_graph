#ifndef CODER_HPP___
#define CODER_HPP___

#include <string>
#include <vector>
#include <SDL.h>
#include <memory>
#include <array>

class Coder
{
public:
	using ushort = unsigned short;
	using uint = unsigned int;
	using ull = unsigned long long;

	using DataVector = std::vector<uint8_t>; // bytes vector
	using Pixel = std::array<uint8_t, 3>;
protected:
	Pixel get_pixel(SDL_Surface* surface, const int& x, const int& y);
	void draw_pixel(SDL_Surface* surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B);
	SDL_Surface* new_bmp_surface(const std::string& filename);
	SDL_Surface* new_empty_surface(const int& width, const int& height);
	struct Surface_deleter
	{
		void operator()(SDL_Surface* surface) const;
	};
	using SDL_Surface_ptr = std::unique_ptr<SDL_Surface, Surface_deleter>;
public:
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
		Header(const int& h, const int& w, const mode& compression_mode, const bool& grayscale_choice);
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
	void draw_pixels(const SDL_Surface& image, const Pixel& pixel, const int& reps, int& x, int& y); // auxiliary method
	void to_7_bit(Pixel& pixel);
	void to_gray(Pixel& pixel);
};


#endif // Coder_HPP___
