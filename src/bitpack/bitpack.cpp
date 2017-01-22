#include <string>
#include "bitpack.hpp"
#include <iterator>
#include "../error/error.hpp"

/**
 * \brief Encoding method for 8-to-7 bits mode
 * \param filename input BMP file path
 * \param grayscale true if encoding in grayscale
 */
void Bitpack::encode(const std::string& filename, const bool& grayscale)
{
	// load surface
	SDL_Surface_ptr image(new_bmp_surface(filename));

	// assuming filename is in correct format e.g. "path\\foo bar xyz.bmp"
	std::fstream outfile(encoded_filename(filename).c_str(),
	                     std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (!outfile.is_open())
		throw Error("In Bitpack::encode(): couldn't open output convert file.");

	// create new header for coded file
	Header bard_header(image.get(), BITPACK, grayscale);

	outfile.write(reinterpret_cast<char*>(&bard_header), sizeof(bard_header));
	outfile.seekg(bard_header.offset, std::ios_base::beg);

	DataVector raw_bytes; // vector with to-be-converted RGB values (in BGR order)
	DataVector encoded_bytes;

	raw_bytes.reserve(10);
	Pixel pixel; // temporary array of rgbs (3 uint8_ts)

	for (int y = 0; y < image->h; ++y)
	{
		for (int x = 0; x < image->w; ++x)
		{
			pixel = get_pixel(image.get(), x, y); // read RGB values
			if (grayscale)
			{
				to_gray(pixel);
				raw_bytes.push_back(std::move(pixel[0]));	// add only one channel (as others are the same)
			}
			else
				for (auto it = pixel.rbegin(); it != pixel.rend(); ++it)
					raw_bytes.push_back(std::move(*it)); // append reversely read values

			if (raw_bytes.size() >= 8)
			{
				encoded_bytes = packer(raw_bytes);
				for (uint8_t& byte : encoded_bytes)
					outfile.write(reinterpret_cast<char*>(&byte), sizeof(byte));
				encoded_bytes.clear(); // clean up already saved values
			}
		}
	}

	// latest bits, that didn't fill raw_bytes array fully
	// also works as a guard when input image was smaller than 3 pixels in total
	if (raw_bytes.size() > 0)
	{
		raw_bytes.resize(8, 0x00);
		encoded_bytes = packer(raw_bytes);
		for (uint8_t& byte : encoded_bytes)
			outfile.write(reinterpret_cast<char*>(&byte), sizeof(byte));
		encoded_bytes.clear(); // clean up already saved values
	}

	outfile.close();
}

/**
* \brief Convert pack of 8 bytes into 7 bytes
* \param raw_bytes vector with uint8_ts.
* It must contain at least 8 values (bytes), 8 of them will be manipluated.
* \return 7 converted values
*/
auto Bitpack::packer(DataVector& raw_bytes) const -> DataVector
{
	if (raw_bytes.size() < 8)
		throw Error("In Bitpack::packer(): invalid raw_bytes size. It must equals (at least) 8.");

	DataVector v_output;
	v_output.reserve(8);

	uint8_t pack;
	uint8_t x, p;
	bool bit;
	for (int t = 0, n = 0; t < 7; ++t , ++n)
	{
		pack = raw_bytes[t];
		x = 7;
		p = n;
		for (int k = 0; k <= n; ++k , --p , --x)
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

/**
* \brief Decoding method for 8-to-7 bits mode
* \param filename input bard file path
* \param grayscale true if encoding in grayscale
*/
void Bitpack::decode(const std::string& filename, const bool& grayscale)
{
	// reading values from file
	std::fstream infile(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded
	if (!infile.is_open())
		throw Error("In Bitpack::decode(): couldn't open input convert file.");

	Header bard_header(infile); // reading Header
	infile.seekg(bard_header.offset, std::ios_base::beg); // set file to read after header

	SDL_Surface_ptr decoded_image(new_empty_surface(bard_header.width, bard_header.height)); // creates surface for drawuing pixels (size header.width x header.height)
	int x = 0, y = 0; // init current pixel (in surface) position

	DataVector raw_bytes; // before-conversion vector. It will contain all "7-bit" values read from file and BEFORE being decoded into 8-bit values (in order BGR)
	DataVector temp_bytes;
	DataVector decoded_bytes; // after-conversion with all RGB channels for every pixel in file (order BGR)

	raw_bytes.reserve(10);
	temp_bytes.reserve(10);
	decoded_bytes.reserve(10);

	uint8_t tmp;
	while (!infile.eof())
	{
		infile.read(reinterpret_cast<char*>(&tmp), sizeof(tmp)); // read single byte from file
		raw_bytes.push_back(tmp); // add it into before-conversion-vector
		if (raw_bytes.size() >= 7)
		{
			temp_bytes = unpacker(raw_bytes); // decode 7 7-bit values into 8 8-bit values

			std::move(temp_bytes.begin(), temp_bytes.end(), std::back_inserter(decoded_bytes)); // move v_temp values to the end of vector decode_vals
			temp_bytes.clear(); // clean up temps

			if (decoded_bytes.size() > 3)
				draw_pixels(*decoded_image, decoded_bytes, x, y, grayscale);
		}
	}

	// additional iteration for pack of bytes that was not divisible by 7
	// also works as a guard when input image was smaller than 3 pixels in total
	if (raw_bytes.size() > 0)
	{
		raw_bytes.resize(7, 0x00);
		temp_bytes = unpacker(raw_bytes);

		std::move(temp_bytes.begin(), temp_bytes.end(), std::back_inserter(decoded_bytes)); // move v_temp values to the end of vector decode_vals
		temp_bytes.clear(); // clean up
		draw_pixels(*decoded_image, decoded_bytes, x, y, grayscale);
	}

	infile.close(); // finished reading. clean up

	SDL_SaveBMP(decoded_image.get(), decoded_filename(filename).c_str()); // finally, save file to BMP extension
}

/**
* \brief Deconvert 7 bytes into 8 bytes
* \param encoded_bytes vector with uint8_ts that represent pixels' RGB channels (in BRG order).
* It must contain at least 7 values (bytes), 7 of them will be manipluated
* \return 8 deconverted values
*/
auto Bitpack::unpacker(DataVector& encoded_bytes) const -> DataVector
{
	if (encoded_bytes.size() < 7)
		throw Error("In Bitpack::unpacker(): invalid encoded_bytes size. It must equals (at least) 8.");

	bool bit;
	// pack_c current pack, pack_n next tmp pack, pack_p previous pack
	uint8_t pack_c = 0x00, pack_n = 0x00, pack_p = 0x00;

	/** current_to_next which bit copy from current pack to next pack,
	*  check_previous which bit copy from previous pack to current pack,
	*  insert_current on which position insert bit from previous pack	*/
	int current_to_next, insert_current, check_previous;

	// n_p how many bits copy from previous pack to current pack, n_n how many bits copy from current pack to next pack
	int n_p = -1, n_n = 0;

	DataVector v_output;
	v_output.reserve(8);

	for (int i = 0; i < 7; ++i)
	{
		pack_c = encoded_bytes[i];
		current_to_next = n_n;

		// copy bit from current pack to next pack
		for (int k = 0; k <= n_n; ++k)
		{
			//check bit
			bit = (pack_c >> current_to_next) & 1;

			if (bit) //set bit x to 1
				pack_n |= 1 << current_to_next;
			else //set bit x to 0
				pack_n &= ~(1 << current_to_next);
			--current_to_next;
		}

		// move n_n bits in current pack in right
		pack_c = pack_c >> n_n;

		check_previous = n_p;
		insert_current = 7;

		// copy bit from previous pack to current pack
		for (int k = 0; k <= n_p; ++k)
		{
			// check bit
			bit = (pack_p >> check_previous) & 1;

			if (bit) // set bit x to 1
				pack_c |= 1 << insert_current;
			else // set bit x to 0
				pack_c &= ~(1 << insert_current);

			--check_previous;
			--insert_current;
		}
		++n_p;
		++n_n;
		pack_p = pack_n;

		// set bit number 0 in current pack to 0
		pack_c &= ~(1 << 0);

		v_output.push_back(std::move(pack_c));
	}
	pack_n = pack_n << 1;
	v_output.push_back(std::move(pack_n));

	auto end_it = std::next(encoded_bytes.begin(), 7); // find next position after 7-th element
	encoded_bytes.erase(encoded_bytes.begin(), end_it); // clean up already decoded values

	return std::move(v_output); // rvo, if no move (?)
}

