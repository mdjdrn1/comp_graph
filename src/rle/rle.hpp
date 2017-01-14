#ifndef RLE_HPP___
#define RLE_HPP___

#include "../coder/coder.hpp"
#include <string>
#include "../sdl_utils/sdl_utils.hpp"

class RLE : public Coder
{
	bool compare(Pixel pixel1, Pixel pixel2)
	{
		if (pixel1[0] == pixel2[0] && pixel1[1] == pixel2[1] && pixel1[2] == pixel2[2])
			return true;
		else return false;
	};
public:
	RLE()
	{
	}

	virtual ~RLE() = default;
	void encode(const std::string& filename, const bool& grayscale) override;
	void decode(const std::string& filename) override;
};

#endif
