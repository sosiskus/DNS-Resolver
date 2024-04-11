cmake_minimum_required(VERSION 3.10)

# Define project
project(DnsResolver CXX)

add_library(${PROJECT_NAME} STATIC)

# Gather source and header files
set(SOURCE_FILES
        src/dnsLookup.cpp
        src/helperFunctions.cpp
)

# Setttings
target_include_directories(${PROJECT_NAME} 
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(${PROJECT_NAME} PRIVATE "-pipe")
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Keep the directory structure in IDE project
list(SORT SOURCE_FILES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})

if(WIN32)
  target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
