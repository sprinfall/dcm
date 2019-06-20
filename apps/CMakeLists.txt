# apps

option(DCM_BUILD_VIEWER_APP "Build the viewer application (need Qt)?" OFF)

set(COMMON_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/print.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/print.h
	)

set(COMMON_LIBS dcm ${Boost_LIBRARIES})

add_executable(dcm_dump dcm_dump.cpp ${COMMON_SRCS})
target_link_libraries(dcm_dump ${COMMON_LIBS})

add_executable(dcm_dump2 dcm_dump2.cpp ${COMMON_SRCS})
target_link_libraries(dcm_dump2 ${COMMON_LIBS})

if(DCM_BUILD_VIEWER_APP)
    add_subdirectory(viewer)
endif()
