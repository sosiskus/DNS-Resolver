cmake_minimum_required(VERSION 3.10)
project(ExampleUsingCustomParsers)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the executable
add_executable(example_using_custom_parsers main.cpp)

# Link the DnsResolver library to the executable
target_link_libraries(example_using_custom_parsers DnsResolver)

# If you need to link additional libraries, you can do it here
if(WIN32)
    target_link_libraries(example_using_custom_parsers wsock32 ws2_32)
endif()