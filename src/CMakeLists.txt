set(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(compress suffix_tree_edit.cpp repetition_in_tree.cpp datatype.cpp)
target_link_libraries(compress rt)

# merge algorithm , lcs_trace is mainly used, other is seldom used
add_executable(lcs_trace lcs_merge_trace.cpp datatype.cpp)
target_link_libraries(lcs_trace rt)
add_executable(basic_merge basic_merge.cpp datatype.cpp)
target_link_libraries(basic_merge rt)
add_executable(suffix_merge suffix_merge.cpp suffix_tree_edit.cpp datatype.cpp)
target_link_libraries(suffix_merge rt)

add_executable(replay replay.cpp datatype.cpp)
target_link_libraries(replay rt)
