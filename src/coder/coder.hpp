#ifndef CODER_HPP___
#define CODER_HPP___

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <array>

#if defined(_WIN32) || defined(_WIN64)
#include <SDL.h>
#elif defined(__unix__)
#include <SDL2/SDL.h>
#endif

class Coder
{
public:
	virtual ~Coder() = default;

	// pure virtual methods
	virtual void encode(const std::string& filename, const bool& grayscale) = 0;
	virtual void decode(const std::string& filename, const bool& grayscale) = 0;

	// aliases
	using ushort = unsigned short;
	using uint = unsigned int;
	using ull = unsigned long long;
	using DataVector = std::vector<uint8_t>; // bytes vector
	using Pixel = std::array<uint8_t, 3>;
protected:
	// Create/remove SDL_Surface
	SDL_Surface* new_bmp_surface(const std::string& filename) const;
	SDL_Surface* new_empty_surface(const int& width, const int& height) const;

	struct Surface_deleter
	{
		void operator()(SDL_Surface* surface) const;
	};

	using SDL_Surface_ptr = std::unique_ptr<SDL_Surface, Surface_deleter>;

	// manipulate SDL_Surface
	Pixel get_pixel(SDL_Surface* surface, const int& x, const int& y);
	void draw_pixel(SDL_Surface* surface, const int& x, const int& y, const uint8_t& R, const uint8_t& G, const uint8_t& B) const;
	void draw_pixels(const SDL_Surface& image, DataVector& pixels, int& x, int& y, const bool& grayscale = false) const; // auxiliary method
	void draw_pixels(const SDL_Surface& image, const Pixel& pixel, const int& reps, int& x, int& y) const; // auxiliary method

	// manipulate Pixel
	void to_7_bit(Pixel& pixel) const;
	void to_gray(Pixel& pixel) const;

	// manipulate filename
	std::string encoded_filename(const std::string& input_filename) const;
	std::string decoded_filename(const std::string& input_filename) const;
public:
	// coding algorithm
	enum mode
	{
		BITPACK,
		HUFF,
		RLE_EN
	};

	// basic header
#pragma pack(push, 1)	// set struct alignment to 0 bytes to 'turn off' paddings
	struct Header
	{
		uint16_t signature;
		uint32_t offset; // offset to data, should be 15(?) bytes
		int32_t width;
		int32_t height;
		bool grayscale; // 0 or 1
		uint16_t compression; // 0=BITPACK, 1=HUFFMAN, 2=RLE
		Header(const int32_t& h, const int32_t& w, const mode& compression_mode, const bool& grayscale_choice);
		Header(SDL_Surface* image, const mode& compression_mode, const bool& grayscale_choice);
		explicit Header(std::fstream& input);
	};
#pragma pack(pop)

	void validate_bmp(const std::string& filename) const;
	void validate_bard(const std::string& filename) const;
};


#endif // Coder_HPP___
