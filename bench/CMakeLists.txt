add_executable(bench bench.cc)

target_link_libraries(bench benchmark::benchmark_main)
target_link_libraries(bench cl_sort)
