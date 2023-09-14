cmake_minimum_required(VERSION 3.20)
project(KMR_gamepad)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Source files
add_library(KMR_ps5 
            source/KMR_ps5.cpp)     

# Directories containing header files
target_include_directories(KMR_ps5 PUBLIC include)

# Locations of the used libraries
#target_link_directories(KMR_ps5 PRIVATE /usr/lib)

# Link the used libraries: libevdev
target_link_libraries(KMR_ps5 evdev)


# Generate Docs
option(BUILD_DOCS "Generate Docs" ON)
if(BUILD_DOCS)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/docs)  # path to docs' cmakelists
endif()