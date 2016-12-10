#include "Converter.h"

Converter::Converter(mode selection) : m_cur_mode(selection)
{
}

Converter::~Converter()
{
}


void Converter::change_mode(mode new_mode)
{
    m_cur_mode = new_mode;

}

// TODO: change parameter to filnemame only
void Converter::convert(const std::string& filename)
{
    switch(m_cur_mode)
    {
        case BITPACK:
            conv_7(filename);
            break;
        case HUFF:
            conv_huffman(filename);
            break;
        case RLE:
            conv_rle(filename);
            break;
        default:
            std::cerr << "Invalid convert mode.\n" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
}

// TODO: change parameter to SDL_Surface and std::string (filename)
void Converter::conv_7(const std::string& filename)
{
    // load surface
    SDL_Surface* image = SDL::new_bmp_surface(filename);

	// assuming filename is in correct format "path\\foo bar xyz.bmp"
	std::string out_name = filename.substr(0, filename.size() - 4) + "_7.bard";   // bitpack and rle dustin-huffman
	std::fstream out_fs(out_name.c_str(),
                     std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

    if(!out_fs.is_open())
    {
        std::cerr << "Couldn't open output convert file [converter() / conv_7()].\n" << std::endl;
        exit(EXIT_FAILURE);
    }

    bard_header new_header = create_header(image, BITPACK, 0);
    out_fs.write(reinterpret_cast<char*>(&new_header), sizeof(new_header));
//    out_fs.seekg(new_header.offset, std::ios_base::beg);

	vect<uint8_t> v1;
	uint8_t* pix; // temporary rgbs

	for (int y = 0; y < image->h; ++y)
	{
	    for(int x = 0; x < image->w; ++x)
		{
            pix = SDL::get_pixel2(image, x, y);

            for(int p=2; p>=0; --p)
                v1.push_back(pix[p]);

            delete[] pix;

		    if (v1.size() >= 8)
            {
                packer(v1, out_fs);
                v1.erase(v1.begin(), v1.begin()+8);
            }
		}
	}

	// latest bits, that didn't fill v1 array fully
	if (v1.size() > 0)
	{
	    v1.resize(8);
	    packer(v1, out_fs);
	}
}

void Converter::conv_huffman(const std::string& filename)
{
    // TODO
}

void Converter::conv_rle(const std::string& filename)
{
    // TODO
}

// vals must containt 8 values!
void Converter::packer(vect<uint8_t>& vals, std::fstream& out_file)
{
    if(vals.size()<8)
    {
        std::cerr << "Invalid vals size. It must equals 8." << std::endl;
        exit(EXIT_FAILURE);
    }

    uint8_t pack = 0x00;
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

		out_file.write(reinterpret_cast<char*>(&pack), sizeof(uint8_t));
	}
}

void Converter::deconvert(const std::string& filename)
{
    switch(m_cur_mode)
    {
        case BITPACK:
            dconv_7(filename);
            break;
        case HUFF:
            dconv_huffman(filename);
            break;
        case RLE:
            dconv_rle(filename);
            break;
        default:
            std::cerr << "Invalid convert mode.\n" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
}

void Converter::dconv_7(const std::string& filename)
{
	std::fstream in_file(filename.c_str(), std::ios_base::in | std::ios_base::binary);  // input (bard) file that will be encoded

    bard_header hb = read_header(in_file);  // reading bard_ header

	vect<uint8_t> v_preconv;  // pre converrsion vector. It will contain all "7-bit" values read from file and BEFORE being coverter into 8-bit (order BGR)
	vect<uint8_t> v_temp;
	vect<uint8_t> v_aftconv;  // after conversion. All RGB channels for every pixel in file (order BGR)

//    in_file.seekg(hb.offset, std::ios_base::beg);  // set file to read after header
///////
    // TEMP OVERRIDING THESE DATA AS READING HEADER IS INVALID
    in_file.seekg(16, std::ios_base::beg);  // set file to read after header
    hb.height = 300;
    hb.width = 450;
///////

    uint8_t tmp;
	while (!in_file.eof())
	{
		in_file.read(reinterpret_cast<char*>(&tmp), sizeof(tmp));   // read single byte from file
		v_preconv.push_back(tmp);   // add it into before-conversion-vector
		if (v_preconv.size() >= 7)
		{
			v_temp = unpacker(v_preconv);   // decode 7 7-bit values into 8 8-bit values

			auto end_it = std::next(v_preconv.begin(), 7);  // find next position after 8-th element
			v_preconv.erase(v_preconv.begin(), end_it); // clean up already decoded values

			std::move(v_temp.begin(), v_temp.end(), std::back_inserter(v_aftconv)); // move v_temp values to the end of vector v_aftconv
			v_temp.erase(v_temp.begin(), v_temp.end()); // clean up temps
		}
	}

    in_file.close();    // finished reading. clean up

    if(v_preconv.size() > 0)    // additional iteration for pack of bytes that was not divisible by 7
    {
        v_preconv.resize(7);

        v_temp = unpacker(v_preconv);
		v_preconv.erase(v_preconv.begin(), v_preconv.end());

		std::move(v_temp.begin(), v_temp.end(), std::back_inserter(v_aftconv)); // move v_temp values to the end of vector v_aftconv
		v_temp.erase(v_temp.begin(), v_temp.end()); // clean up
    }

    SDL_Surface* output_surface = SDL::new_empty_surface(hb.width, hb.height); // creates surface for drawuing pixels (size hb.width x hb.height)

    // TODO: put this into funvtion (width, height, &v_aftconv)
    // drawing pixels here
//    for(int y=0; y < hb.height; ++y)
    for(int y=0; y < 300; ++y)
    {
//        for(int x=0; x < hb.width; ++x)
        for(int x=0; x < 450; ++x)
        {
//            std::cerr<< "drawing pixel x="<<x << " y=" << y <<std::endl;
            if(v_aftconv.size() < 3)
            {
                std::cerr << "Not enough values to draw whole pixel";
                exit(EXIT_FAILURE);
            }
            SDL::draw_pixel(output_surface, x, y, v_aftconv[2], v_aftconv[1], v_aftconv[0]); // draw single pixel (pixels are in BGR order in v_aftconv)

            auto end_it = std::next(v_aftconv.begin(), 3);  // find next position after 3rd item
            v_aftconv.erase(v_aftconv.begin(), end_it); // clean up
        }
    }

    std::string out_name = filename.substr(0, filename.size() - 5) + "_unpacked.bmp";   // output file name
    SDL_SaveBMP(output_surface, (out_name).c_str());    // finally, save file to BMP extension

	SDL::delete_surface(output_surface);    // clean up surface
}

void Converter::dconv_huffman(const std::string& filename)
{
    // TODO
}

void Converter::dconv_rle(const std::string& filename)
{
    // TODO
}

Converter::vect<uint8_t> Converter::unpacker(vect<uint8_t>& vals)   // conv_7 auxiliary method
{
    if(vals.size()<7)
    {
        std::cerr << "Invalid vals size. It must equals at least 7." << std::endl;
        exit(EXIT_FAILURE);
    }

	bool bit = 1;
	uint8_t pack_c = 0x00, pack_n = 0x00, pack_p = 0x00;
	int current_next_b = 0,  insert_previous_b = 0;
	int insert_next_b = 0, current_previous_b = 0 ;
	int n_p = -1, n_n = 0;

    vect<uint8_t> v_output;

	for (int i = 0; i < 7; i++)
	{
		current_next_b = n_n;
		pack_c = vals[i];

		for (int k = 0; k <= n_n; k++)
		{
			//check bit
			bit = (pack_c >> current_next_b) & 1;

			if (bit) //set bit x to 1
				pack_n |= 1 << current_next_b;
			else //set bit x to 0
				pack_n &= ~(1 << current_next_b);
			current_next_b--;
		}

		pack_c = pack_c >> n_n;

		current_previous_b = n_p;
		insert_previous_b = 7;
		for (int k = 0; k <= n_p; k++)
		{
			//check bit
			bit = (pack_p >> current_previous_b) & 1;

			if (bit) //set bit x to 1
				pack_c |= 1 << insert_previous_b;
			else //set bit x to 0
				pack_c &= ~(1 << insert_previous_b);

		//	cout << "set bit " << bit << " on position  " << insert_previous_b << endl;
			current_previous_b--;
			insert_previous_b--;
		}
		n_p++;
		n_n++;
		pack_p = 0x00;
		pack_p = pack_n;
		pack_c &= ~(1 << 0);
		v_output.push_back(pack_c);
		pack_c = 0x00;
	}
	pack_n = pack_n << 1;
    v_output.push_back(pack_n);
    return v_output;
}

Converter::bard_header Converter::create_header(SDL_Surface* surface, mode compression_mode, int grayscale)
{
    bard_header new_header;

    new_header.offset = sizeof(bard_header);
    new_header.width = surface->w;
    new_header.height = surface->h;
    new_header.gray = static_cast<ushort>(grayscale);
    new_header.compression = static_cast<ushort>(compression_mode);
}


Converter::bard_header Converter::read_header(std::fstream& input)
{
    bard_header new_header;
    input.seekg(0, std::ios_base::beg); // set input file to begin
    input.read(reinterpret_cast<char*>(&new_header), sizeof(new_header));

    return new_header;
}
