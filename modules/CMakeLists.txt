set(MODULES concepts.ixx
        deserializer.ixx
        graph.ixx
        graphCake.ixx
        representations/adjacency_matrix.ixx
        representations/adjacency_list.ixx
        representations/representations.ixx
        serializer.ixx
        types.ixx)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fconcepts-diagnostics-depth=3)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET CXX_MODULES FILES ${MODULES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_MODULE_STD ON
        CXX_SCAN_FOR_MODULES ON
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)