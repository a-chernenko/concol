cmake_minimum_required(VERSION 3.10)

project(concol LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(COMPILE_FLAGS -Wall -Wextra -Werror)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

include_directories(include)

set(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(SOURCES ${SOURCE_DIR}/concol.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory(test)
