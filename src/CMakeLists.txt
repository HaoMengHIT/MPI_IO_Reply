set(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(../lib)

add_executable(compress repetition_in_tree.cpp )
target_link_libraries(compress rt MPI_IO_Reply)

# merge algorithm , lcs_trace is mainly used, other is seldom used
add_executable(lcs_trace lcs_merge_trace.cpp )
target_link_libraries(lcs_trace rt MPI_IO_Reply)
add_executable(basic_merge basic_merge.cpp )
target_link_libraries(basic_merge rt MPI_IO_Reply)
add_executable(suffix_merge suffix_merge.cpp )
target_link_libraries(suffix_merge rt MPI_IO_Reply)

add_executable(replay replay.cpp )
target_link_libraries(replay rt MPI_IO_Reply)
