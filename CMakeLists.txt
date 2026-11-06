cmake_minimum_required(VERSION 3.10)

project(sw-rasterizer C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(sw-rasterizer src/main.c)
