cmake_minimum_required(VERSION 3.21)
project(compressexec C)

set(CMAKE_C_STANDARD 11)
add_executable(compressexec reader.c swing.c swing.h paths.h paths.c gorilla.c gorilla.h mod.c mod.h jsonprint.h jsonprint.c error_bound_calculator.h error_bound_calculator.c PMCMean.h PMCMean.c constants.h constants.c compression.h compression.c uncompressed_data_maneger.h uncompressed_data_maneger.c bitreader.c bitreader.h MatLib.h MatLib.c polyswing.c polyswing.h compressed_segment_builder.h vector_based.c vector_based.h argument_handler.h argument_handler.c timestamps.c timestamps.h getopt.c getopt.h distance_calculator.c distance_calculator.h)
#add_executable(compressexec main.c vector_based.c vector_based.h)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -lm")
target_link_libraries(compressexec m)