#ifndef RLE_HPP___
#define RLE_HPP___

#include "../coder/coder.hpp"

class RLE : public Coder
{
	bool compare(const Pixel& pixel1, const Pixel& pixel2) const;
public:
	RLE() = default;
	virtual ~RLE() = default;
	void encode(const std::string& filename, const bool& grayscale) override;
	void decode(const std::string& filename) override;
};

#endif
