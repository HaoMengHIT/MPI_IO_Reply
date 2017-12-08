set(ODIR ${CMAKE_CURRENT_BINARY_DIR})

#exec_program(./func-to-decl.py ${CMAKE_CURRENT_SOURCE_DIR}
#   ARGS ${CMAKE_CURRENT_SOURCE_DIR}/func_list ${ODIR}/decl_list )
exec_program(./MPI_wrapper_generator_compress.py ${CMAKE_CURRENT_SOURCE_DIR}
   ARGS decl_list.def
   ${ODIR}/recorder-mpi-io-compress.cpp
   ${ODIR}/func_info
   )

find_package(MPI)

set(CMAKE_CXX_FLAGS "-std=c++11")
add_definitions(
   -DRECORDER_CONFIG_H=recorder-runtime-config.h
   -D_LARGEFILE64_SOURCE
   -DRECORDER_PRELOAD
   )
include_directories(./ ./lib ${MPI_CXX_INCLUDE_PATH})

add_library(recorder-compress SHARED
   lib/simple_compress.cpp
   recorder-mpi-init-finalize-compress.cpp
   ${ODIR}/recorder-mpi-io-compress.cpp
   )
target_link_libraries(recorder-compress
   ${MPI_CXX_LIBRARIES}
   )
set_target_properties(recorder-compress
   PROPERTIES LINK_FLAGS "${MPI_CXX_LINK_FLAGS}"
   )

install(TARGETS recorder-compress DESTINATION lib)
