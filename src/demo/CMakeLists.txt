if(UNIX)
    add_definitions(-D_GLIBCXX_USE_WCHAR_T -std=c++11)
endif()

file(GLOB SRCS *.cpp *.h)

add_executable(viewer ${SRCS})

target_link_libraries(viewer dcmlite)

