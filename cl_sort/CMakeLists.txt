add_library(cl_sort src/cl_bitonic_sort.cc)

target_include_directories(cl_sort PUBLIC include)
target_link_libraries(cl_sort PUBLIC cl_app)
