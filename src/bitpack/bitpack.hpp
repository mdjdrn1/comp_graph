#ifndef BITPACK_HPP___
#define BITPACK_HPP___

#include "../coder/coder.hpp"
#include <vector>

class Bitpack : public Coder
{
public:
	using ushort = unsigned short;
	using uint = unsigned int;
	using ull = unsigned long long;

	using DataVector = std::vector<uint8_t>; // bytes vector
	using Pixel = Coder::Pixel;

	Bitpack();// = default;
	~Bitpack() = default;
	Bitpack(const Bitpack&) = default;
	Bitpack(Bitpack&&) = default;
	Bitpack& operator=(const Bitpack&) = default;
	Bitpack& operator=(Bitpack&&) = default;

public:
	void encode(const std::string& filename, const bool& grayscale) override;
private:
	DataVector packer(DataVector& raw_bytes) const; // auxiliary method for encode
public:
	void decode(const std::string& filename, const bool& grayscale) override;
private:
	DataVector unpacker(DataVector& encoded_bytes) const; // auxiliary method for decode
};


#endif
