add_library(cl_app src/cl_app.cc)

target_include_directories(cl_app PUBLIC include)
target_link_libraries(cl_app PUBLIC OpenCL::OpenCL)
target_compile_features(cl_app PUBLIC cxx_std_17)
