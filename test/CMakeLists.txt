set(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(../src ../Recorder_All/lib)

add_executable(test-reorder test_reorder.cpp)

add_executable(test-cpp-simple-compress test_cpp_simple_compress.cpp ../Recorder_All/lib/simple_compress.cpp)

add_executable(imcpp imcpp.cpp imcpp-main.cpp)
