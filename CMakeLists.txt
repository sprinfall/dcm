cmake_minimum_required(VERSION 3.1.0)

if(POLICY CMP0074)
    # find_package() uses <PackageName>_ROOT variables.
    # This policy was introduced in CMake version 3.12.
    cmake_policy(SET CMP0074 NEW)
endif()

project(dcm)

option(DCM_BUILD_UNITTEST "Build unit test?" OFF)
option(DCM_BUILD_EXAMPLES "Build examples?" ON)
option(DCM_BUILD_APPS "Build applications?" ON)

if(WIN32)
    option(DCM_ENABLE_VLD "Enable VLD (Visual Leak Detector)?" OFF)

    if(DCM_ENABLE_VLD)
        add_definitions(-DDCM_ENABLE_VLD)
    endif()
endif()

set(DCM_ENABLE_LOG 1 CACHE STRING "Enable logging? (1:Yes, 0:No)")
set(DCM_LOG_LEVEL 2 CACHE STRING "Log level (0:VERB, 1:INFO, 2:WARN, 3:ERRO or 4:FATA)")

add_definitions(-DUNICODE -D_UNICODE)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# C++ standard requirements.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

if(DCM_BUILD_UNITTEST)
    enable_testing()
endif()

include_directories(
    # For including its own headers as "dcm/xxx.h".
    ${PROJECT_SOURCE_DIR}
    # For including config.h as "dcm/config.h".
    ${PROJECT_BINARY_DIR}
    )

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third_party)

if(WIN32)
    include_directories(${THIRD_PARTY_DIR}/win32/include)
    link_directories(${THIRD_PARTY_DIR}/win32/lib)
endif()

include_directories(${THIRD_PARTY_DIR}/src)

add_subdirectory(dcm)

if(DCM_BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/examples)
endif()

if(DCM_BUILD_UNITTEST)
    add_subdirectory(${THIRD_PARTY_DIR}/src/gtest)
    add_subdirectory(unittest)
endif()

if(DCM_BUILD_APPS)
    add_subdirectory(apps)
endif()
