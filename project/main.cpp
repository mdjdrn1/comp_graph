#include <SDL/SDL.h>
#include <iostream>
#include "Window.h"
#include "Converter.h"

int main(int argc, char** argv)
{
    std::fstream log ("errors.log", std::ios_base::out | std::ios_base::trunc);
    std::cerr.rdbuf(log.rdbuf());   // redirect stderr to log file
    // CONVERSION TEST
    Converter conv(Converter::BITPACK, 0);

    // const int N = 6;
    // std::string name [N] = {"test\\pics\\pic1_cut.bmp", "test\\pics\\pic1.bmp"};
    //    "test\\pics\\pic2.bmp", "test\\pics\\pic3.bmp",
    //    "test\\pics\\pic4.bmp", "test\\pics\\pic5.bmp"};
    // for(int i=0; i<N; ++i)
    //    conv.convert(name[i]);

    conv.convert("test\\pics\\pic1.bmp");
    conv.deconvert("test\\pics\\pic1_7.bard");

    std::cerr.rdbuf(nullptr);
    return 0;
}
