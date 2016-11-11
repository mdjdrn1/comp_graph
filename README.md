# comp_graph

# Computer Graphics project.

## Requirements
+ SDL2 v2.0.5
+ Visual Studio 2015 *(highly recommended, but not required)*

## Set up SDL2 with VS2015

1. Grab the latest Visual C++ Development Libraries (at the time of writing, that would be SDL2-devel-2.0.3-VC.zip)

2. Create a folder for your SDL2 libraries in a convenient location, say C:\sdl2. Extract the include and lib folders in the zip file here

3. Create an empty Visual C++ project

4. Open project properties

5. Add the SDL2 include folder (C:\sdl2\include) to the Include Directories in VC++ Directories

6. In Linker -> General, set Additional Library Directories to the path to the SDL2 library (C:\sdl2\lib\x86) – either x86 or x64

7. Then in Linker -> Input, replace the value of Additional Dependencies with “SDL2.lib; SDL2main.lib“

8. Finally, in Linker -> System, set SubSystem to "Windows (/SUBSYSTEM:WINDOWS)" OR "Console (...)" if you want to use both SDL and console

9. Grab SDL.dll from C:\sdl2\lib\x86 and put it in your project’s Debug folder, where the executable is being generated

___


TEST PROGRAM
 ```
#include <SDL.h>

int main(int argc, char ** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
 
    // game code eventually goes here
 
    SDL_Quit();
 
    return 0;
}
```
