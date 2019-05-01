add_executable(dcmdump dcmdump.cpp)
add_executable(dcminfo dcminfo.cpp)

target_link_libraries(dcmdump dcm)
target_link_libraries(dcminfo dcm)
