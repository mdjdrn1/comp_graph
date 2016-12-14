#include "Converter.h"

/** \brief Converting method for 8-to-7 bits mode
 *
 * \param filename const std::string& converted file path
 * \return void
 *
 */
void Converter::conv_7(const std::string& filename)
{
	// load surface
	SDL_Surface* image = SDL::new_bmp_surface(filename);

	// assuming filename is in correct format e.g. "path\\foo bar xyz.bmp"
	std::string out_name = filename.substr(0, filename.size() - 4) + ".bard";
	std::fstream out_file(out_name.c_str(),
	                      std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	if (!out_file.is_open())
	{
		std::cerr << "Couldn't open output convert file [converter() / conv_7()].\n" << std::endl;
		exit(EXIT_FAILURE);
	}

	// create new header for coded file
	bard_header new_header = create_header(image, BITPACK, 0);
	out_file.write(reinterpret_cast<char*>(&new_header), sizeof(new_header));
	out_file.seekg(new_header.offset, std::ios_base::beg);

	DataVector v1; // vector with converter RGB values (in BGR order)
	DataVector v_temp;
	PixArr pixel; // temporary array of rgbs (3 uint8_ts)

	for (int y = 0; y < image->h; ++y)
	{
		for (int x = 0; x < image->w; ++x)
		{
			pixel = SDL::get_pixel2(image, x, y); // read RGB values
			if (m_grayscale == 1)
				to_gray(pixel);

			for (int p = 2; p >= 0; --p)
				v1.push_back(std::move(pixel[p])); // append read values

			if (v1.size() >= 8)
			{
				v_temp = packer(v1);
				for (uint8_t val : v_temp)
					out_file.write(reinterpret_cast<char*>(&val), sizeof(val));
				v_temp.erase(v_temp.begin(), v_temp.end()); // clean up already saved values
			}
		}
	}

	// latest bits, that didn't fill v1 array fully
	if (v1.size() > 0)
	{
		v1.resize(8);
		v_temp = packer(v1);
		for (uint8_t val : v_temp)
			out_file.write(reinterpret_cast<char*>(&val), sizeof(val));
		v_temp.erase(v_temp.begin(), v_temp.end()); // clean up already saved values
	}
}


/** \brief Convert pack of 8 bytes into 7 bytes
 *
 * \param vals DataVector& vector with uint8_ts. It must constain at least 8 values (bytes).
 * 8 of them will be manipluated
 * \return Converter::DataVector 7 converted values
 *
 */
Converter::DataVector Converter::packer(DataVector& vals)
{
	if (vals.size() < 8)
	{
		std::cerr << "Invalid vals size. It must equals 8." << std::endl;
		exit(EXIT_FAILURE);
	}

	DataVector v_output;

	uint8_t pack;
	uint8_t x, p;
	bool bit = 1;
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

	return v_output;
}

/** \brief Deconverting method for 8-to-7 bits mode
 *
 * \param filename const std::string& converted file path
 * \return void
 *
 */
void Converter::dconv_7(const std::string& filename)
{
	// reading values from file
	std::fstream in_file(filename.c_str(), std::ios_base::in | std::ios_base::binary); // input (bard) file that will be encoded

	if (!in_file.is_open())
	{
		std::cerr << "Couldn't open input convert file [converter() / dconv_7()].\n" << std::endl;
		exit(EXIT_FAILURE);
	}

	bard_header header = read_header(in_file); // reading bard_header
	in_file.seekg(header.offset, std::ios_base::beg); // set file to read after header

	DataVector raw_vals; // before-conversion vector. It will contain all "7-bit" values read from file and BEFORE being decoded into 8-bit values (in order BGR)
	DataVector v_temp;
	DataVector decode_vals; // after-conversion with all RGB channels for every pixel in file (order BGR)

	uint8_t tmp;
	while (!in_file.eof())
	{
		in_file.read(reinterpret_cast<char*>(&tmp), sizeof(tmp)); // read single byte from file
		raw_vals.push_back(tmp); // add it into before-conversion-vector
		if (raw_vals.size() >= 7)
		{
			v_temp = unpacker(raw_vals); // decode 7 7-bit values into 8 8-bit values

			std::move(v_temp.begin(), v_temp.end(), std::back_inserter(decode_vals)); // move v_temp values to the end of vector decode_vals
			v_temp.erase(v_temp.begin(), v_temp.end()); // clean up temps
		}
	}

	if (raw_vals.size() > 0) // additional iteration for pack of bytes that was not divisible by 7
	{
		raw_vals.resize(7);
		v_temp = unpacker(raw_vals);

		std::move(v_temp.begin(), v_temp.end(), std::back_inserter(decode_vals)); // move v_temp values to the end of vector decode_vals
		v_temp.erase(v_temp.begin(), v_temp.end()); // clean up
	}

	in_file.close(); // finished reading. clean up

	SDL_Surface* output_image = SDL::new_empty_surface(header.width, header.height); // creates surface for drawuing pixels (size header.width x header.height)
	draw_pixels(output_image, decode_vals); // drawing pixels here

	std::string out_name = filename.substr(0, filename.size() - 5) + "_decoded.bmp"; // output file name
	SDL_SaveBMP(output_image, (out_name).c_str()); // finally, save file to BMP extension

	SDL::delete_surface(output_image); // clean up surface
}

/** \brief Deconvert 7 bytes into 8 bytes
 *
 * \param vals DataVector& vector with uint8_ts that represent pixels' RGB channels (in BRG order)
 *. It must constain at least 7 values (bytes), 7 of them will be manipluated
 * \return Converter::DataVector 8 decoverted values
 *
 */
Converter::DataVector Converter::unpacker(DataVector& vals)
{
	if (vals.size() < 7)
	{
		std::cerr << "Invalid vals size. It must equals at least 7." << std::endl;
		exit(EXIT_FAILURE);
	}
	bool bit = 1;
	// pack_c current pack, pack_n next tmp pack, pack_p previous pack
	uint8_t pack_c = 0x00, pack_n = 0x00, pack_p = 0x00;

	/** current_to_next which bit copy from current pack to next pack,
	 *  check_previous which bit copy from previous pack to current pack,
	 *  insert_current on which position insert bit from previous pack	*/
	int current_to_next = 0, insert_current = 0, check_previous = 0;

	//n_p how many bits copy from previous pack to current pack, n_n how many bits copy from current pack to next pack
	int n_p = -1, n_n = 0;

	DataVector v_output;

	for (int i = 0; i < 7; i++)
	{
		pack_c = vals[i];
		current_to_next = n_n;

		//copy bit from current pack to next pack
		for (int k = 0; k <= n_n; k++)
		{
			//check bit
			bit = (pack_c >> current_to_next) & 1;

			if (bit) //set bit x to 1
				pack_n |= 1 << current_to_next;
			else //set bit x to 0
				pack_n &= ~(1 << current_to_next);
			current_to_next--;
		}

		//move n_n bits in current pack in right
		pack_c = pack_c >> n_n;

		check_previous = n_p;
		insert_current = 7;

		//copy bit from previous pack to current pack
		for (int k = 0; k <= n_p; k++)
		{
			//check bit
			bit = (pack_p >> check_previous) & 1;

			if (bit) //set bit x to 1
				pack_c |= 1 << insert_current;
			else //set bit x to 0
				pack_c &= ~(1 << insert_current);

			check_previous--;
			insert_current--;
		}
		n_p++;
		n_n++;
		pack_p = pack_n;

		//set bit number 0 in current pack to 0
		pack_c &= ~(1 << 0);

		v_output.push_back(std::move(pack_c));
	}
	pack_n = pack_n << 1;
	v_output.push_back(std::move(pack_n));

	auto end_it = std::next(vals.begin(), 7); // find next position after 7-th element
	vals.erase(vals.begin(), end_it); // clean up already decoded values

	return v_output;
}
