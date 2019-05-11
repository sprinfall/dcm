set(SRCS
    dcminfo
    dcmcopy
    )

foreach(name ${SRCS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} dcm ${Boost_LIBRARIES})
endforeach()

add_executable(dcmdump dcmdump.cpp print.cpp print.h)
target_link_libraries(dcmdump dcm ${Boost_LIBRARIES})

add_executable(dcmdump2 dcmdump2.cpp print.cpp print.h)
target_link_libraries(dcmdump2 dcm ${Boost_LIBRARIES})
