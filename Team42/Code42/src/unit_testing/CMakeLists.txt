file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.h")
add_executable(unit_testing ${srcs} "src/pkb/test_pkb.cpp" "src/pkb/test_pattern_manager.cpp" "src/PQL/test_query_preprocessor.cpp")

target_link_libraries(unit_testing spa)
