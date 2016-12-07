#include <SDL/SDL.h>
#include <iostream>
#include "Window.h"
#include "Converter.h"

int main( int argc, char** argv )
{
    std::fstream log ("errors.log", std::ios_base::out | std::ios_base::trunc);
    std::cerr.rdbuf(log.rdbuf());   // redirect stderr to log file

    // CONVERSION TEST
/*
    Converter conv(Converter::BITPACK);
    const int N = 6;
    Window* mw = new Window[N] {(std::string)"test_pics\\pic1.bmp", (std::string)"test_pics\\pic1_cut.bmp",
        (std::string)"test_pics\\pic2.bmp", (std::string)"test_pics\\pic3.bmp",
        (std::string)"test_pics\\pic4.bmp", (std::string)"test_pics\\pic4.bmp"};
    for(int i=0; i<N; ++i)
        conv.convert(mw+i);


    delete[] mw;
*/

    // PACK TEST
    // First set Converter::packer to public
//	std::fstream out_fs("pack_test.bin",
//                     std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
//
//    uint8_t p1 = static_cast<uint8_t>(255);
//    uint8_t p2 = static_cast<uint8_t>(0);
//    std::vector<uint8_t> pack;
//    for(int i = 0; i < 8; i++)
//    {
//        pack.push_back(p1);
//        pack.push_back(p2);
//    }
//    conv.packer(pack, out_fs);
//
//    out_fs.close();

    log.close();
    return 0;
}
