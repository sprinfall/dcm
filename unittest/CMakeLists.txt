# Unit test

set(UT_SRCS
    dicom_reader_unittest.cpp
    main.cpp
    )

set(UT_TARGET_NAME dcmlite_unittest)

# Common libraries to link.
set(UT_LIBS dcmlite gtest ${Boost_LIBRARIES})

add_executable(${UT_TARGET_NAME} ${UT_SRCS})
target_link_libraries(${UT_TARGET_NAME} ${UT_LIBS})

add_test(${UT_TARGET_NAME} ${UT_TARGET_NAME})
