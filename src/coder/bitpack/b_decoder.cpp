/**
* Bitpack decoder implementation
*/

#include "../decoder.hpp"

/**
* \brief Deconverting method for 8-to-7 bits mode
* \param filename converted file path
*/
void Decoder::bitpack(const std::string& filename)
{
	// reading values from file
	std::fstream infile(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded

	if (!infile.is_open())
		throw Error("In Decoder::bitpack(): couldn't open input convert file.");

	bardHeader bard_header;
	bard_header.create_from_encoded_file(infile); // reading bardHeader
	infile.seekg(bard_header.offset, std::ios_base::beg); // set file to read after header

	SDL_Surface* decoded_image = SDL_utils::new_empty_surface(bard_header.width, bard_header.height); // creates surface for drawuing pixels (size header.width x header.height)
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
				draw_pixels(decoded_image, decoded_bytes, x, y);
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
		draw_pixels(decoded_image, decoded_bytes, x, y);
	}

	infile.close(); // finished reading. clean up

	std::string decoded_file_name = filename.substr(0, filename.size() - 5) + "_decoded.bmp"; // output file name
	SDL_SaveBMP(decoded_image, decoded_file_name.c_str()); // finally, save file to BMP extension

	SDL_utils::delete_surface(decoded_image); // clean up surface
}

/**
* \brief Deconvert 7 bytes into 8 bytes
* \param encoded_bytes vector with uint8_ts that represent pixels' RGB channels (in BRG order).
* It must contain at least 7 values (bytes), 7 of them will be manipluated
* \return 8 deconverted values
*/
Decoder::DataVector Decoder::unpacker(DataVector& encoded_bytes) const
{
	if (encoded_bytes.size() < 7)
		throw Error("In Decoder::unpacker(): invalid encoded_bytes size. It must equals (at least) 8.");

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

/**
* \brief Draw pixels into SDL_Surface image and cleans them up from 'pixels'
* \param image pixels-input surface
* \param pixels vector with uint8_ts that represent pixels' RGB channels (in BRG order)
* \param x width value for image
* \param y heigth value for image
*/
void Decoder::draw_pixels(SDL_Surface* image, DataVector& pixels, int& x, int& y)
{
	uint8_t* pixelptr = pixels.data(); // first pixels obj pointer
									   // calculate how many pixels (from DataVector pixels) are available to draw in surface
	ull left_to_draw = static_cast<ull>((pixels.size() - pixels.size() % 3) / 3);
	while (y < image->h && x < image->w && left_to_draw > 0)
	{
		SDL_utils::draw_pixel(image, x, y, pixelptr[2], pixelptr[1], pixelptr[0]);
		pixelptr += 3;
		++x;
		--left_to_draw;
		if (x == image->w) // go to next line of image
		{
			x = 0;
			++y;
		}
	}
	pixels.erase(pixels.begin(), pixels.end() - pixels.size() % 3); // remove drew pixels
}
