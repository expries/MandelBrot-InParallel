cmake_minimum_required(VERSION 3.24)
project(Mandelbrot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -static -static-libgcc -static-libstdc++ -O3 -Wall -DNDEBUG")

add_library(tga Tga.cpp)

add_executable(Mandelbrot main.cpp Mandelbrot.cpp Mandelbrot.h)
target_link_libraries(Mandelbrot tga)
