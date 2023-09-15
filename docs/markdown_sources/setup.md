# How to setup {#setup}
[TOC]

## Dependencies
This library is dependent on the following libraries that need to be installed first:
- libevdev: https://gitlab.freedesktop.org/libevdev/libevdev 
- CMake
- (Doxygen and Graphviz if you wish to regenerate the documentation locally)

## Installation
This library does not need to be installed per se. It has to be built into a static library (.a) in a "build" folder: 
```bash
mkdir build
cd build
cmake ../
cmake --build .
```
This will generate the static library "libKMR_ps5.a" in the "build" folder. 

In case you wish to regenerate the documentation locally, run
```bash
make docs
```
from the build folder. This will generate a "index.html" file in "docs/generated_docs/html".

## Include in a project
It is very straightforward to include this library in a project. \n
All the headers are located in "KMR_gamepads/include", and, as already mentioned, the static libraries in "KMR_gamepads/build".

In the root CMakeLists.txt of the project, add:
```cmake
# Path to KMR_gamepads' CMakeLists
add_subdirectory(path-to-KMR_gamepads)

# Path to KMR_gamepads' header files
target_include_directories(project PUBLIC path-to-KMR_gamepads/include)

# Path to the static (.a) library
target_link_directories(project PRIVATE path-to-KMR_gamepads/build)

# Link the library
target_link_libraries(project KMR_ps5)

```

In the source code, simply include the header of the gamepad you're interested in, for example:
```cpp
#include "KMR_ps5.hpp"
```

Next: how to [use](#how-to-use)