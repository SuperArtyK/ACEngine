# ACEngine

This is my C++ engine to create games and stuff in console. Rework of my previous console_engine attempt.

The code is divided into projects to ease development and testing. (so I don't need to compile the whooole thing to test one thing...like I did last time lol)

Current project list:

* ACEngine -- main project where everything is combined

* AEMisc -- project where the miscellaneous things, global engine variables and utilities lie.  Basically stuff that cannot be put into a module

* AEModuleBase -- project of the most basic module of the engine, implementing features for all engine modules.

* AEFileWriter -- project of file writer module. The fast and convenient file writer of the engine.


For now I'm developing the engine on and for visual studio, to get it up to speed with the previous engine. And no, instead of locking it to winapi I'm planning to make it as crossplatform as possible. 

You know, use as much of crossplatform things like curses, stl, etc.

**Technical stuff:** When including a header file that belongs to a project, just include it's name in quotes. Otherwise add the "include/" before it's name. Also yeah, make that the include/source path is set up properly - add the $(SolutionDir)\[Dependency project name]\include (or \src) to the paths of your project.