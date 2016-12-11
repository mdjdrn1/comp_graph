#include "Converter.h"

Converter::Converter(mode selection, bool grayscale) : m_cur_mode(selection), m_grayscale(grayscale)
{
}

Converter::~Converter()
{
}

/** \brief runs proper converting method for current mode
 *
 * \param filename const std::string& converted file path
 * \return void
 *
 */
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

/** \brief Runs proper deconverting method for current mode
 *
 * \param filename const std::string& converted file name
 * \return void
 *
 */
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


/** \brief Draws pixels into SDL_Surface image and cleans them up from 'pixels'
 *
 * \param image SDL_Surface* pixels-input surface
 * \param pixels DataVector& vector with uint8_ts that represent pixels' RGB channels (in BRG order)
 * \return void
 *
 */
void Converter::draw_pixels(SDL_Surface* image, DataVector& pixels)
{
    for(int y=0; y < image->h; ++y)
    {
        for(int x=0; x < image->w; ++x)
        {
            if(pixels.size() < 3)
            {
                std::cerr << "Not enough values to draw whole pixel";
                exit(EXIT_FAILURE);
            }
            SDL::draw_pixel(image, x, y, pixels[2], pixels[1], pixels[0]); // draw single pixel (pixels are in BGR order in decode_vals)

            auto end_it = std::next(pixels.begin(), 3);  // find next position after 3rd item
            pixels.erase(pixels.begin(), end_it); // clean up
        }
    }
}

/** \brief Creates bard header
 *
 * \param image SDL_Surface* image to create header from
 * \param compression_mode mode BITPACK, RLE or HUFF
 * \param grayscale int 1 for grayscale, otherwise 0
 * \return Converter::bard_header new header
 *
 */
Converter::bard_header Converter::create_header(SDL_Surface* image, mode compression_mode, int grayscale)
{
    bard_header new_header;

    new_header.offset = sizeof(bard_header);
    new_header.width = image->w;
    new_header.height = image->h;
    new_header.grayscale = m_grayscale;
    new_header.compression = static_cast<ushort>(compression_mode);

    return std::move(new_header);
}


/** \brief Reads header from bard file
 *
 * \param input std::fstream& input file stream
 * \return Converter::bard_header read header
 *
 */
Converter::bard_header Converter::read_header(std::fstream& input)
{
    bard_header new_header;
    input.seekg(0, std::ios_base::beg); // set input file to begin
    input.read(reinterpret_cast<char*>(&new_header), sizeof(new_header));

    return new_header;
}


/** \brief changing mode of converter
 *
 * \param new_mode mode BITPACK, RLE or HUFF
 * \param grayscale bool 1 if converting image to grayscale, otherwise 0
 * \return void
 *
 */
void Converter::change_mode(mode new_mode, bool grayscale)
{
    m_cur_mode = new_mode;
    m_grayscale = grayscale;
}


/** \brief Altering pixel into grayscale mode
 *
 * \param pixel uint8_t* pointer to array of 3 uint8_ts representing BGR in RGB pixel
 * \return void
 *
 */
void Converter::to_gray(uint8_t* pixel)  // pixel in BGR order
{
    // using luma formula to calculate "relative luminescence"
    uint8_t luma = static_cast<uint8_t>(pixel[2] * 0.2126 + pixel[1] * 0.7152 + pixel[0] * 0.0722);
    pixel[0] = pixel[1] = pixel[2] = luma;
}
