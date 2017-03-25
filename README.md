# Computer Graphics project

## BARD format
BARD is file format for compressed BMP files. Compression is performed using one of three algorithms: RLE, Huffman or 8-to-7 bits.

### BARD file stucture
|	Structure name 	| Optional	| Size	| Purpose	|
| ------------- |------------- | ------------- | -------------| 
| (Main) File header     | No | 13 bytes | Contains informations about type of conversion |
| Huffman header      | Yes      |   13 bytes | Contains informations for Huffman algorithm conversion  |
| Pixels array | No      |    Dependent of file | Contains data (pixels' values) |

### File header structure
|Offset | Name | Size | Data type | Purpose|
| ------------- |------------- | ------------- | -------------| -------------|
|0|signature|2 bytes|uint16_t|BARD file identifier|
|2|offset|1 byte|uint8_t|Offset to data (pixels array). However in Huffman coded file - it's offset to Huffman header.|
|3|width|4 bytes|uint32_t|Image width in pixels.|
|7|height|4 bytes|uint32_t|Image height in pixels.|
|11|grayscale|1 byte|bool|Contains information, if file is coded in grayscale.|
|12|conversion type|uint8_t|unsigned int|Contains information about used algorithm: 0=8-to-7 bits, 1=Huffman, 2=RLE. |

### Huffman header structure
|Offset | Name | Size | Data type | Purpose|
| ------------- |------------- | ------------- | -------------| -------------|
|0|channels|8 bytes|uint64_t|Contains amount of coded channels.|
|8|max length of item in map|2 bytes|uint16_t|Contains max length of items in codes map. |
|9|type|1 byte|uint8_t|Describes, how Huffman codes are saved in header.|

## Grayscale
Every compression algorithm allows you to convert file using only "grayscale" colors. To calculate value of "grayscale" pixel, there is used YUV color space. After computations value of every channel is equal to 0,299*R + 0,587*G + 0,114*B. That allows our algorithms to skip two channels during saving file to BARD format. 
E.g. compression with 8-to-7 bit algorithm gives you output **BARD** file with size equal to about **87,5% of original BMP file**. With grayscale, result is much better - output BARD file has size equal to only about **29,2% of original BMP file**!

___


## Requirements
+ C++ compiler (tested on g++, gcc, visual C++ compiler )
+ SDL v2.x

## Running SDL2 program on Linux
**Recommended** for:
+ g++ v4.x
```
g++ foo.cpp -g -pedantic -o foo -lSDL2 -std=c++1y
```
+ g++ v5.x
```
g++ foo.cpp -g -pedantic -o foo -lSDL2 -std=c++14
```

## Running SDL2 program on Windows - Visual Studio 2015

1. Grab the latest Visual C++ Development Libraries (at the time of writing, that would be **SDL2-devel-2.0.5-VC.zip**)

2. Create a folder for your SDL2 libraries in a convenient location, say C:\sdl2. Extract the include and lib folders in the zip file here

3. Create an empty Visual C++ project

4. Open project properties

5. Add the SDL2 **include** folder (C:\sdl2\include) to the *Configuration Properties/VC++ Directories/Include Directories*

6. Add the SDL2 **library** folder (C:\sdl2\lib\x86) – either x86 or x64 - to the *Configuration Properties/VC++ Directories/Library Directories* **and** *Linker/General/Additional Library Directories*

7. Add values **“SDL2.lib; SDL2main.lib“** - to the *Linker/Input/Additional Dependencies*

8. Then, set *Linker/System/SubSystem* to:
+ "Windows (/SUBSYSTEM:WINDOWS)" (for only window apps - without console) 
OR 
+ "Console (/SUBSYSTEM:CONSOLE)" if you want to use both SDL and console

9. Finally, Grab SDL.dll from C:\sdl2\lib\x86 (or x64) and put it in your project’s Debug/Release folder, where the executable is being generated

10. Run test program

#### Test program
 ```c++
#include <SDL.h>

int main(int argc, char ** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Quit();
    return 0;
}
```