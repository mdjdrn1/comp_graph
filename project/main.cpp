#include <SDL/SDL.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "Window.h"
#include "Converter.h"

void test_conv(const std::vector<std::string>& name);

int main(int argc, char** argv)
{
//    freopen("CON", "w", stdout);  // enable console output on debug

    std::fstream log ("errors.log", std::ios_base::out | std::ios_base::trunc);
    std::cerr.rdbuf(log.rdbuf());   // redirect stderr to log file
    std::cout.rdbuf(log.rdbuf());   // redirect stderr to log file

    using StringVector = std::vector<std::string>;

//    StringVector sv = {"test\\pics\\pic1_cut.bmp", "test\\pics\\pic1.bmp",
//       "test\\pics\\pic2.bmp", "test\\pics\\pic3.bmp",
//       "test\\pics\\pic4.bmp", "test\\pics\\pic4_5.bmp",
//       "test\\pics\\pic5.bmp"};
//
    StringVector sv = {"test\\pics\\1.bmp", "test\\pics\\2.bmp",
       "test\\pics\\3.bmp", "test\\pics\\4.bmp",
       "test\\pics\\5.bmp", "test\\pics\\6.bmp",
       "test\\pics\\7.bmp"};

    test_conv(sv);

    std::cerr.rdbuf(nullptr);
    std::cout.rdbuf(nullptr);
    return 0;
}


void test_conv(const std::vector<std::string>& name)
{
    // CONVERSION TEST FOR BITPACK / NO GRAYSCALE
    Converter conv(Converter::BITPACK, 0);

    std::string output_name;
    for(int i=0; i<name.size(); ++i)
    {
        std::cout << std::setw(10) << "File: " << name[i] << std::endl;
        auto start = std::chrono::system_clock::now();
        conv.convert(name[i]);
        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << std::setw(10) << "Coding\t"<< time/1000000. << "s\n";

        output_name = name[i].substr(0, name[i].size()-3)+"bard";

        start = std::chrono::system_clock::now();
        conv.deconvert(output_name);
        end = std::chrono::system_clock::now();
        time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << std::setw(10) << "Decoding\t"<< time/1000000. << "s\n";
    }

}
