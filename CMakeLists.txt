cmake_minimum_required(VERSION 2.8)
project(dcmlite)

option(DCMLITE_ENABLE_UT "Enable unit test?" OFF)

if(WIN32)
    option(DCMLITE_ENABLE_VLD "Enable VLD (Visual Leak Detector)?" OFF)

    if(DCMLITE_ENABLE_VLD)
        add_definitions(-DDCMLITE_ENABLE_VLD)
    endif()
endif()

add_definitions(-DUNICODE -D_UNICODE)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if(GDCM_ENABLE_UT)
    enable_testing()
endif()

# Group sources by dir.
# Usage: source_group_by_dir(SRCS)
macro(GROUP_SOURCES_BY_DIR source_files)
    set(sgbd_cur_dir ${CMAKE_CURRENT_BINARY_DIR})
    foreach(sgbd_file ${${source_files}})
        #message("sgbd_fpath=${sgbd_fpath}")
        string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
        string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
        string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
        string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
        if(sgbd_nogroup)
            set(sgbd_group_name "\\")
        endif()
        #message("group name=${sgbd_group_name}")
        source_group(${sgbd_group_name} FILES ${sgbd_file})
    endforeach()
endmacro()

# Add target property.
# Usage: ADD_TARGET_PROPERTY(target COMPILE_FLAGS "/we1234")
macro(ADD_TARGET_PROPERTY target_name property_key property_value)
    get_target_property(CTS_COMPILE_FLAGS ${target_name} ${property_key} )
    #	message("t:${target_name} k:${property_key} v:${property_value} ${CTS_COMPILE_FLAGS}")
    if(CTS_COMPILE_FLAGS STREQUAL CTS_COMPILE_FLAGS-NOTFOUND)
        SET_TARGET_PROPERTIES(${target_name} PROPERTIES
            ${property_key} ${property_value})
    else()
        SET_TARGET_PROPERTIES(${target_name} PROPERTIES
            ${property_key} "${CTS_COMPILE_FLAGS} ${property_value}")
    endif()
endmacro()

# Common target setting.
# Usage: COMMON_TARGET_SETTING(target SRCS)
macro(COMMON_TARGET_SETTING target_name source_list)
    #GROUP_SOURCES_BY_DIR(${source_list})

    if(WIN32)
        if(MSVC)
            # treat c4239 c4482 c4715 as error, as these can cause error on gcc
            # disable c4407 c4290
            ADD_TARGET_PROPERTY(${target_name} COMPILE_FLAGS "/we4239 /we4482 /we4715 /wd4407 /wd4290")
        endif(MSVC)
    endif()
endmacro()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/third_party/include)
if(WIN32)
    include_directories(${PROJECT_SOURCE_DIR}/third_party/win32/include)
    link_directories(${PROJECT_SOURCE_DIR}/third_party/win32/lib)
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(src)

