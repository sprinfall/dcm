set(UT_TARGET_NAME dcm_unittest)

file(GLOB UT_SRCS *.cpp)

add_executable(dcm_unittest ${UT_SRCS})
target_link_libraries(dcm_unittest dcm gtest)

add_test(dcm_unittest dcm_unittest)
