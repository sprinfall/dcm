set(SRCS
    dcminfo
    dcmcopy
    )

foreach(name ${SRCS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} dcm)
endforeach()
