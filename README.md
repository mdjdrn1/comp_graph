# Computer Graphics project

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