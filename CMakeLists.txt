cmake_minimum_required(VERSION 3.21)
project(compressexec C)

set(CMAKE_C_STANDARD 11)
add_executable(compressexec main.c swing.c swing.h paths.h gorilla.c gorilla.h mod.c mod.h)
