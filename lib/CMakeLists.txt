
set(CMAKE_CXX_FLAGS "-std=c++11")

add_library(MPI_IO_Reply SHARED
   datatype.cpp 
   preprocess.cpp
   suffix_tree_edit.cpp 
   )
