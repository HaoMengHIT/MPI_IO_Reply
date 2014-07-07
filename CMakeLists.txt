cmake_minimum_required(VERSION 2.8)
project(MPI_IO_reply)


find_package(Boost REQUIRED)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(Recorder_All)
