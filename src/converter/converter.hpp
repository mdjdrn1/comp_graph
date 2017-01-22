#ifndef CONVERTER_HPP___
#define CONVERTER_HPP___

#include "../coder/coder.hpp"

class Converter
{
public:
	using mode = Coder::mode;

	Converter() = default;
	~Converter() = default;
	Converter(const Converter&) = default;
	Converter(Converter&&) = default;
	Converter& operator=(const Converter&) = default;
	Converter& operator=(Converter&&) = default;

	void encode(const std::string& filename, const mode& algoritm_mode, const bool& grayscale) const;
	void decode(const std::string& filename) const;
private:
	auto get_algorithm_mode_and_grayscale(const std::string& filename) const -> std::pair<mode, bool>;
};

#endif
