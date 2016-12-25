#include "converter.hpp"


/** \brief Converting method for 8-to-7 bits mode
 *
 * \param filename const std::string& converted file path
 *
 */
void Converter::conv_7(const std::string& filename)
{
	// load surface
	SDL_Surface* image = SDL_utils::new_bmp_surface(filename);

	// assuming filename is in correct format e.g. "path\\foo bar xyz.bmp"
	std::string outfile_name = filename.substr(0, filename.size() - 4) + ".bard";
	std::fstream outfile(outfile_name.c_str(),
	                                 std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	
	if (!outfile.is_open())
		throw Error("In Converter::conv_7(): couldn't open output convert file.");

	// create new header for coded file
	bardHeader bard_header;
	bard_header.create_from_SDLSurface(image, BITPACK, 0);

	outfile.write(reinterpret_cast<char*>(&bard_header), sizeof(bard_header));
	outfile.seekg(bard_header.offset, std::ios_base::beg);

	DataVector v1; // vector with converter RGB values (in BGR order)
	DataVector v_temp;

	v1.reserve(10);
	PixArr pixel; // temporary array of rgbs (3 uint8_ts)

	for (int y = 0; y < image->h; ++y)
	{
		for (int x = 0; x < image->w; ++x)
		{
			pixel = SDL_utils::get_pixel(image, x, y); // read RGB values
			if (m_is_grayscale == 1)
				to_gray(pixel);

			for (int p = 2; p >= 0; --p)
				v1.push_back(std::move(pixel[p])); // append read values

			if (v1.size() >= 8)
			{
				v_temp = packer(v1);
				for (uint8_t val : v_temp)
					outfile.write(reinterpret_cast<char*>(&val), sizeof(val));
				v_temp.clear(); // clean up already saved values
			}
		}
	}

	// latest bits, that didn't fill v1 array fully
	// also works as a guard when input image was smaller than 3 pixels in total
	if (v1.size() > 0)
	{
		v1.resize(8, 0x00);
		v_temp = packer(v1);
		for (uint8_t val : v_temp)
			outfile.write(reinterpret_cast<char*>(&val), sizeof(val));
		v_temp.clear(); // clean up already saved values
	}

	SDL_utils::delete_surface(image);
	outfile.close();
}

/** \brief Convert pack of 8 bytes into 7 bytes
 *
 * \param vals DataVector& vector with uint8_ts. It must constain at least 8 values (bytes).
 * 8 of them will be manipluated
 * \return Converter::DataVector 7 converted values
 *
 */
Converter::DataVector Converter::packer(DataVector& vals) const
{
	if (vals.size() < 8)
		throw Error("In Converter::packer(): invalid vals size. It must equals (at least) 8.");

	DataVector v_output;
	v_output.reserve(8);

	uint8_t pack;
	uint8_t x, p;
	bool bit;
	for (int t = 0, n = 0; t < 7; ++t , ++n)
	{
		pack = vals[t];
		x = 7;
		p = n;
		for (int k = 0; k <= n; ++k , --p , --x)
		{
			//check bit
			bit = (vals[t + 1] >> x) & 1;

			if (bit) //set bit x to 1
				pack |= 1 << p;
			else //set bit x to 0
				pack &= ~(1 << p);
		}
		vals[t + 1] <<= t + 1;
		v_output.push_back(std::move(pack));
	}
	vals.erase(vals.begin(), vals.begin() + 8); // clean up already converted values

	return std::move(v_output); // rvo(?) if no move?
}

/** \brief Deconverting method for 8-to-7 bits mode
 *
 * \param filename const std::string& converted file path
 *
 */
void Converter::dconv_7(const std::string& filename)
{
	// reading values from file
	std::fstream infile(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded

	if (!infile.is_open())
		throw Error("In Converter::dconv_7(): couldn't open input convert file.");

	bardHeader bard_header;
	bard_header.create_from_encoded_file(infile); // reading bardHeader
	infile.seekg(bard_header.offset, std::ios_base::beg); // set file to read after header

	SDL_Surface* decoded_image = SDL_utils::new_empty_surface(bard_header.width, bard_header.height); // creates surface for drawuing pixels (size header.width x header.height)
	int x = 0, y = 0; // init current pixel (in surface) position

	DataVector raw_vals; // before-conversion vector. It will contain all "7-bit" values read from file and BEFORE being decoded into 8-bit values (in order BGR)
	DataVector v_temp;
	DataVector decode_vals; // after-conversion with all RGB channels for every pixel in file (order BGR)

	raw_vals.reserve(10);
	v_temp.reserve(10);
	decode_vals.reserve(10);

	uint8_t tmp;
	while (!infile.eof())
	{
		infile.read(reinterpret_cast<char*>(&tmp), sizeof(tmp)); // read single byte from file
		raw_vals.push_back(tmp); // add it into before-conversion-vector
		if (raw_vals.size() >= 7)
		{
			v_temp = unpacker(raw_vals); // decode 7 7-bit values into 8 8-bit values

			std::move(v_temp.begin(), v_temp.end(), std::back_inserter(decode_vals)); // move v_temp values to the end of vector decode_vals
			v_temp.clear(); // clean up temps

			if (decode_vals.size() > 3)
				draw_pixels(decoded_image, decode_vals, x, y);
		}
	}

	// additional iteration for pack of bytes that was not divisible by 7
	// also works as a guard when input image was smaller than 3 pixels in total
	if (raw_vals.size() > 0)
	{
		raw_vals.resize(7, 0x00);
		v_temp = unpacker(raw_vals);

		std::move(v_temp.begin(), v_temp.end(), std::back_inserter(decode_vals)); // move v_temp values to the end of vector decode_vals
		v_temp.clear(); // clean up
		draw_pixels(decoded_image, decode_vals, x, y);
	}

	infile.close(); // finished reading. clean up

	std::string decoded_file_name = filename.substr(0, filename.size() - 5) + "_decoded.bmp"; // output file name
	SDL_SaveBMP(decoded_image, decoded_file_name.c_str()); // finally, save file to BMP extension

	SDL_utils::delete_surface(decoded_image); // clean up surface
}

/** \brief Deconvert 7 bytes into 8 bytes
 *
 * \param vals DataVector& vector with uint8_ts that represent pixels' RGB channels (in BRG order)
 *. It must constain at least 7 values (bytes), 7 of them will be manipluated
 * \return Converter::DataVector 8 decoverted values
 *
 */
Converter::DataVector Converter::unpacker(DataVector& vals) const
{
	if (vals.size() < 7)
		throw Error("In Converter::unpacker(): invalid vals size. It must equals (at least) 8.");
	
	bool bit;
	// pack_c current pack, pack_n next tmp pack, pack_p previous pack
	uint8_t pack_c = 0x00, pack_n = 0x00, pack_p = 0x00;

	/** current_to_next which bit copy from current pack to next pack,
	 *  check_previous which bit copy from previous pack to current pack,
	 *  insert_current on which position insert bit from previous pack	*/
	int current_to_next, insert_current, check_previous;

	//n_p how many bits copy from previous pack to current pack, n_n how many bits copy from current pack to next pack
	int n_p = -1, n_n = 0;

	DataVector v_output;
	v_output.reserve(8);

	for (int i = 0; i < 7; ++i)
	{
		pack_c = vals[i];
		current_to_next = n_n;

		//copy bit from current pack to next pack
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

		//move n_n bits in current pack in right
		pack_c = pack_c >> n_n;

		check_previous = n_p;
		insert_current = 7;

		//copy bit from previous pack to current pack
		for (int k = 0; k <= n_p; ++k)
		{
			//check bit
			bit = (pack_p >> check_previous) & 1;

			if (bit) //set bit x to 1
				pack_c |= 1 << insert_current;
			else //set bit x to 0
				pack_c &= ~(1 << insert_current);

			--check_previous;
			--insert_current;
		}
		++n_p;
		++n_n;
		pack_p = pack_n;

		//set bit number 0 in current pack to 0
		pack_c &= ~(1 << 0);

		v_output.push_back(std::move(pack_c));
	}
	pack_n = pack_n << 1;
	v_output.push_back(std::move(pack_n));

	auto end_it = std::next(vals.begin(), 7); // find next position after 7-th element
	vals.erase(vals.begin(), end_it); // clean up already decoded values

	return std::move(v_output); // rvo, if no move (?)
}
