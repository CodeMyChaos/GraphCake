set(UT AdjacencyListTests.cpp)

add_executable(ut ${UT})
set_target_properties(ut PROPERTIES
    CXX_MODULE_STD ON
    CXX_SCAN_FOR_MODULES ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_compile_features(ut PRIVATE cxx_std_23)

target_link_libraries(ut PRIVATE graphCake gtest_main gtest)

include(GoogleTest)
gtest_discover_tests(ut)