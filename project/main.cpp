#include <SDL/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "Window.h"
#include "Converter.h"

void test_conv(const std::vector<std::string>& name);

int main(int argc, char** argv)
{
    freopen("CON", "w", stdout);

    std::fstream log ("errors.log", std::ios_base::out | std::ios_base::trunc);
    std::cerr.rdbuf(log.rdbuf());   // redirect stderr to log file

    using StringVector = std::vector<std::string>;

    StringVector sv = {"test\\pics\\pic1_cut.bmp", "test\\pics\\pic1.bmp",
       "test\\pics\\pic2.bmp", "test\\pics\\pic3.bmp",
       "test\\pics\\pic4.bmp", "test\\pics\\pic4_5.bmp",
       "test\\pics\\pic5.bmp"};

    test_conv(sv);

    std::cerr.rdbuf(nullptr);
    return 0;
}


void test_conv(const std::vector<std::string>& name)
{
    // CONVERSION TEST FOR BITPACK / NO GRAYSCALE
    Converter conv(Converter::BITPACK, 0);

    std::string output_name;
    for(int i=0; i<name.size(); ++i)
    {
        std::cout << "Converting " << name[i] << std::endl;
        auto start = std::chrono::system_clock::now();
        conv.convert(name[i]);
        auto end = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Elapsed time="<< time/1000. << "s\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));

        output_name = name[i].substr(0, name[i].size()-3)+"bard";

        std::cout << "Decoding " << output_name << std::endl;
        start = std::chrono::system_clock::now();
        conv.deconvert(output_name);
        end = std::chrono::system_clock::now();
        time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Elapsed time="<< time/1000. << "s\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

}
