add_executable(dcminfo dcminfo.cpp)
add_executable(dcmdump dcmdump.cpp)
add_executable(dcmcopy dcmcopy.cpp)

target_link_libraries(dcminfo dcm)
target_link_libraries(dcmdump dcm)
target_link_libraries(dcmcopy dcm)
