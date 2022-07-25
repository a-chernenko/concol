cmake_minimum_required(VERSION 3.19)
project(concol)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

set(PROJECT_COMPILE_DEFINES)
set(PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PROJECT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/concol.h)
set(PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/concol.cpp)
set(PROJECT_LINK_LIBRARIES)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDE_DIRS})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_COMPILE_DEFINES})
target_compile_options(${PROJECT_NAME} PUBLIC ${PROJECT_COMPILE_OPTIONS})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LINK_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE on) # -fPIC

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/)

if(TEST_ENABLE)
    add_subdirectory(test)
endif()
