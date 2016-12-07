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

void Converter::convert(Window* win)
{
    switch(m_cur_mode)
    {
        case BITPACK:
            conv_7(win);
            break;
        case HUFF:
            conv_huffman(win);
            break;
        case RLE:
            conv_rle(win);
            break;
        default:
            std::cerr << "Invalid convert mode.\n" << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
}

void Converter::conv_7(Window* win)
{
	// assuming filename is in correct format "path\\foo bar xyz.bmp"
	std::string out_name = win->m_filename.substr(0, win->m_filename.size() - 4) + "_7.bard";
	std::fstream out_fs(out_name.c_str(),
                     std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

    if(!out_fs.is_open())
    {
        std::cerr << "Couldn't open output convert file [converter() / conv_7()].\n" << std::endl;
        exit(EXIT_FAILURE);
    }

	vect<uint8_t> v1, v2;
	uint8_t* pix; // temporary rgbs

	for (int y = 0; y < win->m_bmp->h; ++y)
	{
	    for(int x = 0; x < win->m_bmp->w; ++x)
		{
            pix = SDL::getpixel2(win->m_bmp, x, y);

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

void Converter::conv_huffman(Window* win)
{
    // TODO
}

void Converter::conv_rle(Window* win)
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
