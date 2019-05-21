# Unit test

file(GLOB UT_SRCS *.cpp)

set(UT_TARGET_NAME dcm_unittest)

# Common libraries to link.
set(UT_LIBS dcm gtest ${Boost_LIBRARIES})

add_executable(${UT_TARGET_NAME} ${UT_SRCS})
target_link_libraries(${UT_TARGET_NAME} ${UT_LIBS})

add_test(${UT_TARGET_NAME} ${UT_TARGET_NAME})
