set(SRCS
    dcminfo
    dcmdump
    dcmdump2
    dcmcopy
    )

foreach(name ${SRCS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} dcm ${Boost_LIBRARIES})
endforeach()
