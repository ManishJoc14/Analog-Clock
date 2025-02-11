cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Fetch SFML
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Find system OpenGL
find_package(OpenGL REQUIRED)

# Create the executable
add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)

# Link against SFML and OpenGL
target_link_libraries(main PRIVATE SFML::Graphics SFML::Window OpenGL::GL)
