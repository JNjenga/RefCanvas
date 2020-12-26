# rubik-cube-game

An implementation of the Rubik's cube puzzle game 

## Setting up

### Windows

- Run the following command to generate visual studio solution

```./deps/premake5.exe vs2017```

Substitute vs2017 with your vs version for example vs2019 if its the 2019 version.

		
- A `workspace` directory will be created containing the visual studio project files.
- Open the `Rubik.sln` to open the project
- Run to test out the project

### NIX systems

- Install dependancies :
```$ sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev libglew-dev```

- Run the following command to generate a make file

```./deps/premake5 gmake2```

- A `workspace` directory will be created containing the project make files.

- To run the project run the following command:

```make --directory=workspace run```
