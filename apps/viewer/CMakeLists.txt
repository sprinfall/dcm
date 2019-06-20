# dicom viewer based on Qt5

set(TARGET_NAME dcm_viewer)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

file(GLOB SRCS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${SRCS} ${COMMON_SRCS})

target_link_libraries(${TARGET_NAME} ${COMMON_LIBS} Qt5::Widgets)
