cmake_minimum_required(VERSION 3.20)
project(MultiThreadedServer VERSION 1.0)

# C++ Standard configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add backend subdirectory
add_subdirectory(backend)

# Enable testing
enable_testing()
# add_subdirectory(tests)
