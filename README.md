# ACEngine

This is my C++ engine to create games and stuff in console. Rework of my previous [console_engine](https://github.com/SuperArtyK/console_engine_win32 "console_engine") attempt

The code is divided into projects to ease development and testing. (so I don't need to compile the whooole thing to test one thing...like I did last time lol). 

You can find the engine code documentation [here](https://superartyk.github.io/ACEngine "ACEngine doxygen...docs") 
(It isn't fully done, so for all engine info visit the "Namespaces" or "Data Structures" pages)

You can also track the progress of the engine here: [trello board](https://trello.com/b/egICmbPv/artyks-engine "trello board") 

##### Current project list:

* ACEngine -- main project where everything is combined
* AEMisc -- project where the miscellaneous things, global engine variables, and the general engine utilities. Basically stuff that cannot be put into a module
* AEModuleBase -- project of the most basic module of the engine, implementing features for all engine modules.
* AEFileWriter -- project of file writer module. The fast and convenient file writer of the engine (just wrapper of C FILE write api lol).
* AEFrame -- project of the frame delayer (or rater) module of the engine. Makes the perfectly timed delays in the loop possible..if the code takes less time
* AELogger -- project of engine's logger. Allows to log (random) stuff to file.
* AETimer -- project of the engine's timer. Allows to time the events in the engine and count ticks
* AEVector -- project of the engine's mathematical vectors for. Easy, "scalable", and all constexpr (except conversion to string).
* AEFileReader -- project of the file reader module. The fast and convenient file data reader of the engine (just wrapper of C FILE read api lol).
* AECustomTypes -- project of the engine's custom types. The custom types and structs, stuff used widely in the engine

### ACCOMPLISHED FEATURES

* Be able to write data to files, faster than standard stream stuff
* Be able to read datat from files, faster than standard stream stuff
* Write logs to file with almost no performance impact
* Have a module-based structure
* Have a utility tool kit in engine for easier programming(similar to [func_utils.hpp](https://github.com/SuperArtyK/console_engine_win32/blob/dev/include/engine/func_utils.hpp "func_utils.hpp") in [console_engine](https://github.com/SuperArtyK/console_engine_win32 "console_engine"))

### DESIRED FEATURES

* Engine file format
* Read and parse logs from file
* Get input from keyboard and mouse
* Control console's window properties
* Control characters on the screen: write them/change color individually and in bulk
* Make it crossplatform and support multiple build systems
* Play and synthesize sound

(For now I'm developing the engine on and for visual studio, to get it up to speed with the previous engine, but I'll make the make/nix to work soon too. You know, use as much of crossplatform things like curses, stl, etc)

Yeah, I have yet to release any working version.

**Technical stuff:** When including a header file that belongs to a project, just include it's name in quotes. Otherwise add the "include/" before it's name. Also yeah, make that the include/source path is set up properly - add the "$(SolutionDir)\\(Dependency project name)\include" (or src) to the paths of your project...and add the source file(s) of the module you're including to the project, if they exist.


And all of this stuff is backed by [**MIT License**](./LICENSE.md "**MIT License**"). **Free Software, Hell Yeah!**

