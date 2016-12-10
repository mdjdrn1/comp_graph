#include <SDL/SDL.h>
#include <iostream>
#include "Window.h"
#include "Converter.h"

int main( int argc, char** argv )
{
    std::fstream log ("errors.log", std::ios_base::out | std::ios_base::trunc);
    std::cerr.rdbuf(log.rdbuf());   // redirect stderr to log file

    // CONVERSION TEST

    Converter conv(Converter::BITPACK);

    const int N = 2;
    std::string name [N] = {(std::string)"test_pics\\pic1_cut.bmp", (std::string)"test_pics\\pic1.bmp"};
    //    (std::string)"test_pics\\pic2.bmp", (std::string)"test_pics\\pic3.bmp",
    //    (std::string)"test_pics\\pic4.bmp", (std::string)"test_pics\\pic5.bmp"};
    for(int i=0; i<N; ++i)
        conv.convert(name[i]);

    conv.deconvert("test_pics\\pic1_7.bard");

    log.close();
    return 0;
}
