cmake_minimum_required(VERSION 3.1.0)

if(POLICY CMP0074)
    # find_package() uses <PackageName>_ROOT variables.
    # This policy was introduced in CMake version 3.12.
    cmake_policy(SET CMP0074 NEW)
endif()

project(dcmlite)

option(DCMLITE_BUILD_UNITTEST "Enable unit test?" OFF)
option(DCMLITE_BUILD_EXAMPLE "Build examples?" ON)

if(WIN32)
    option(DCMLITE_ENABLE_VLD "Enable VLD (Visual Leak Detector)?" OFF)

    if(DCMLITE_ENABLE_VLD)
        add_definitions(-DDCMLITE_ENABLE_VLD)
    endif()
endif()

add_definitions(-DUNICODE -D_UNICODE)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if(DCMLITE_BUILD_UNITTEST)
    enable_testing()
endif()

# For including its own headers as "dcmlite/xxx.h".
include_directories(${PROJECT_SOURCE_DIR})

if(WIN32)
    include_directories(${PROJECT_SOURCE_DIR}/third_party/win32/include)
    link_directories(${PROJECT_SOURCE_DIR}/third_party/win32/lib)
endif()

add_subdirectory(dcmlite)

if(DCMLITE_BUILD_EXAMPLE)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example)
endif()

if(DCMLITE_BUILD_UNITTEST)
    add_subdirectory(third_party/src/gtest)
    add_subdirectory(unittest)
endif()