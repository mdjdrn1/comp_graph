/**
 * Bitpack encoder implementation
 */

#include "../encoder.hpp"

/**
* \brief Converting method for 8-to-7 bits mode
* \param filename converted file path
*/
void Encoder::bitpack(const std::string& filename)
{
	// load surface
	SDL_Surface* image = SDL_utils::new_bmp_surface(filename);

	// assuming filename is in correct format e.g. "path\\foo bar xyz.bmp"
	std::string outfile_name = filename.substr(0, filename.size() - 4) + ".bard";
	std::fstream outfile(outfile_name.c_str(),
		std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (!outfile.is_open())
		throw Error("In Encoder::bitpack(): couldn't open output convert file.");

	// create new header for coded file
	bardHeader bard_header;
	bard_header.create_from_SDLSurface(image, BITPACK, 0);

	outfile.write(reinterpret_cast<char*>(&bard_header), sizeof(bard_header));
	outfile.seekg(bard_header.offset, std::ios_base::beg);

	DataVector raw_bytes; // vector with to-be-converted RGB values (in BGR order)
	DataVector encoded_bytes;

	raw_bytes.reserve(10);
	PixArr pixel; // temporary array of rgbs (3 uint8_ts)

	for (int y = 0; y < image->h; ++y)
	{
		for (int x = 0; x < image->w; ++x)
		{
			pixel = SDL_utils::get_pixel(image, x, y); // read RGB values
			if (m_is_grayscale == 1)
				SDL_utils::to_gray_pixel(pixel);

			for (uint8_t& byte : pixel)
				raw_bytes.push_back(std::move(byte)); // append read values

			if (raw_bytes.size() >= 8)
			{
				encoded_bytes = packer(raw_bytes);
				for (uint8_t& byte : encoded_bytes)
					outfile.write(reinterpret_cast<char*>(&byte), sizeof(byte));
				encoded_bytes.clear(); // clean up already saved values
			}
		}
	}

	// latest bits, that didn't fill v1 array fully
	// also works as a guard when input image was smaller than 3 pixels in total
	if (raw_bytes.size() > 0)
	{
		raw_bytes.resize(8, 0x00);
		encoded_bytes = packer(raw_bytes);
		for (uint8_t& byte : encoded_bytes)
			outfile.write(reinterpret_cast<char*>(&byte), sizeof(byte));
		encoded_bytes.clear(); // clean up already saved values
	}

	SDL_utils::delete_surface(image);
	outfile.close();
}

/**
* \brief Convert pack of 8 bytes into 7 bytes
* \param raw_bytes vector with uint8_ts.
* It must contain at least 8 values (bytes), 8 of them will be manipluated.
* \return 7 converted values
*/
Encoder::DataVector Encoder::packer(DataVector& raw_bytes) const
{
	if (raw_bytes.size() < 8)
		throw Error("In Encoder::packer(): invalid raw_bytes size. It must equals (at least) 8.");

	DataVector v_output;
	v_output.reserve(8);

	uint8_t pack;
	uint8_t x, p;
	bool bit;
	for (int t = 0, n = 0; t < 7; ++t, ++n)
	{
		pack = raw_bytes[t];
		x = 7;
		p = n;
		for (int k = 0; k <= n; ++k, --p, --x)
		{
			//check bit
			bit = (raw_bytes[t + 1] >> x) & 1;

			if (bit) //set bit x to 1
				pack |= 1 << p;
			else //set bit x to 0
				pack &= ~(1 << p);
		}
		raw_bytes[t + 1] <<= t + 1;
		v_output.push_back(std::move(pack));
	}
	raw_bytes.erase(raw_bytes.begin(), raw_bytes.begin() + 8); // clean up already converted values

	return std::move(v_output); // rvo(?) if no move?
}
