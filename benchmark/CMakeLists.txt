cmake_minimum_required(VERSION 3.16)

project(BoidsBenchmark)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Add compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
endif()

# Add executable
add_executable(benchmark
    benchmark_main.cpp
    benchmark_runner.cpp
)

# Link libraries
target_link_libraries(benchmark
    OpenMP::OpenMP_CXX
    pthread
)

# Add compile definitions for optimization
target_compile_definitions(benchmark PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

# Set output directory
set_target_properties(benchmark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Add a custom target for running quick benchmark
add_custom_target(quick
    COMMAND ${CMAKE_BINARY_DIR}/benchmark --quick
    DEPENDS benchmark
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running quick benchmark..."
)

# Add a custom target for running full benchmark
add_custom_target(full
    COMMAND ${CMAKE_BINARY_DIR}/benchmark --full
    DEPENDS benchmark
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running full benchmark..."
)

# Print configuration information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
if(OpenMP_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
    message(STATUS "OpenMP flags: ${OpenMP_CXX_FLAGS}")
else()
    message(WARNING "OpenMP not found - parallel benchmarks will not work")
endif()
